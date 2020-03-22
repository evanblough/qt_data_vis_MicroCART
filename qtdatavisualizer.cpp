#include "qtdatavisualizer.h"
#include "ui_qtdatavisualizer.h"
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include "qcustomplot.h"
#include "QTextEdit"

#define GRAPH_WIDTH 1440
#define GRAPH_HEIGHT 720

/**
 * @brief QtDataVisualizer::QtDataVisualizer Widget Class that can be used to display postflight logfiles from MicroCART Drone
 * @param parent
 */

QtDataVisualizer::QtDataVisualizer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QtDataVisualizer)
{
    ui->setupUi(this);
    QObject::connect(ui->xmax, &QTextEdit::textChanged , this, &QtDataVisualizer::update_bounds);
    num_logs = 0;
    num_params = 0;
    num_units = 0;
}

QtDataVisualizer::~QtDataVisualizer()
{
    delete ui;
}

/**
 * @brief QtDataVisualizer::load_file call this method to load the data from a log file into the QTDataVisualizer Object. Call this function before calling display params.
 * @param filename the absolute path of the log file to be parsed
 * @return returns zero on success and -1 if error occured
 */
int QtDataVisualizer::load_file(QString filename)
{
    //Open File
    std::ifstream input_file(filename.toStdString().c_str());
    //Determine if valid File path
    if(!input_file){
        printf("File: %s \tDNE\n", filename.toStdString().c_str());
        return -1;
    }
    int log_flag = 0;
    std::string line;
    std::string buffer;

    //Count Params and Units
    while(std::getline(input_file, line)){
        //Comment Ignore
        if(line.at(0) == '#'){
           continue;
       }
        //Params
        else if(line.at(0) == '%'){
           int i = 0;
           //Parse each Param line and check for tab delimiter
           while(i < line.size()){
               if(line.at(i) == '\t'){
                   num_params++;
               }
               i++;
           }
           num_params++;
       }
        //Units
        else if(line.at(0) == '&'){
            num_units = num_params;
            log_flag = true;
            continue;
        }
        //Count number of data records
        else if(log_flag){
            num_logs++;
            continue;
        }
        else{
            continue;
        }

    }
    num_logs++;

    //Allocate Storage for class fields
    data =  new log_data[num_params];
    for(int i =0; i < num_params; i++){
        data[i].values = (double*) malloc(sizeof(double)*num_logs);
    }

    input_file.close();

    //Read information from log file
    log_flag = false;
    int param_index = 0;
    int unit_index = 0;
    int data_index = 0;
    buffer.clear();
    std::ifstream ifile(filename.toStdString().c_str());
    //Read in Data
    while(std::getline(ifile, line)){
        //Comment Ignore
        if(line.at(0) == '#'){
           continue;
       }
        std::stringstream linestream(line);
        //Params
        if(line.at(0) == '%'){
            line_parse(line);
            continue;
        }
        //Units
        if(line.at(0) == '&'){
            while(unit_index < num_units){
                //TODO check last case
                if(std::getline(linestream, buffer, '\t')){
                    data[unit_index++].units = QString::fromStdString(buffer);
                }
            }
            log_flag = true;
            continue;
        }
        //Data Readings
        if(log_flag && data_index < num_logs){
            int param_type = 0;
            while(param_type < num_params){
                if(std::getline(linestream, buffer, '\t')){
                    data[param_type].values[data_index] = stod(buffer, nullptr);
                }
                param_type++;
            }
            data_index++;
            continue;
        }

    }
    return 0;

}

int QtDataVisualizer::display_params(int x_index, int y_index, double xmin, double xmax, double ymin, double ymax, int bound_flag)
{
    QVector<double> x(num_logs), y(num_logs), z(num_logs);
    //Copy Values to vector format
    for(int i =0; i < num_logs; i++){
        x[i] = data[x_index].values[i];
        y[i] = data[y_index].values[i];
    }

    //Populate Widget Taken from this example
    //https://www.qcustomplot.com/index.php/tutorials/basicplotting
    QCustomPlot* qcp = new QCustomPlot();
    ui->scrollArea->setWidget(qcp);
    qcp->addGraph(qcp->xAxis, qcp->yAxis);
    qcp->graph(0)->setData(x,y);
    qcp->graph(0)->setPen(QPen(Qt::blue));

    //Label Axes
    qcp->xAxis->setLabel(data[x_index].param.toStdString().c_str());
    qcp->yAxis->setLabel(data[y_index].param.toStdString().c_str());

    //Set Ranges
    if(!bound_flag){
        qcp->xAxis->setRange(*std::min_element(x.constBegin(), x.constEnd()), *std::max_element(x.constBegin(), x.constEnd()));
        qcp->yAxis->setRange(*std::min_element(y.constBegin(), y.constEnd()), *std::max_element(y.constBegin(), y.constEnd()));
    }
    else{
        qcp->xAxis->setRange(xmin, xmax);
        qcp->yAxis->setRange(ymin, ymax);
    }
    qcp->replot();
    return 0;
}

int QtDataVisualizer::display_multiplot(QList<int>* y_axis, int x_axis, double xmin, double xmax, double ymin, double ymax, int bound_flag){
    QVector<double> x(num_logs);
    //Yaxis
    QVector<double>y_data[y_axis->size()];
    for(int i = 0; i< y_axis->size(); i++){
        y_data[i] = QVector<double>(num_logs);
        for(int j = 0; j < num_logs; j++){
            x[j] = data[0].values[j];
            y_data[i][j] = data[y_axis->at(i)].values[j];
        }
    }
    QCustomPlot* qcp = new QCustomPlot();
    qcp->setFixedSize(GRAPH_WIDTH, GRAPH_HEIGHT);
    ui->scrollArea->setWidget(qcp);
    QColor temp_color;
    for(int i = 0; i < y_axis->size(); i++){
        if(i % 8 == 0 || i % 8 == 7){
            temp_color = QColor(Qt::black);
        }
        else{
            int scale = ((i / 8)+1) * 255/2;
            //Semi-Random Color
            temp_color = QColor(scale*((i % 6) > 3), scale*((i%4)>1), scale*(i%2), 255);
        }
        qcp->addGraph(qcp->xAxis, qcp->yAxis);
        qcp->graph(i)->setData(x,y_data[i]);
        qcp->graph(i)->setPen(QPen(temp_color));
    }
    qcp->xAxis->setLabel(data[0].param.toStdString().c_str());
    qcp->yAxis->setLabel(data[y_axis->at(0)].param.toStdString().c_str());
    qcp->xAxis->setRange(xmin, xmax);
    qcp->yAxis->setRange(ymin, ymax);

    return 0;
}

int QtDataVisualizer::getNum_params() const
{
    return num_params;
}

int QtDataVisualizer::getNum_units() const
{
    return num_units;
}

int QtDataVisualizer::getNum_logs() const
{
    return num_logs;
}

log_data *QtDataVisualizer::getData() const
{
    return data;
}

void QtDataVisualizer::line_parse(std::string line)
{
    int param_index = 0;
    std::string buffer;

    for(int i = 0; i < line.length(); i++){
        if(line.at(i) != '\t'){
            std::string s(1, line.at(i));
            buffer.append(s);
        }
        else{
            data[param_index++].param = QString::fromStdString(buffer.c_str());
            buffer.clear();
        }
    }

}


void QtDataVisualizer::update_bounds(){
    //Update Xmax
    if(!ui->xmax->toPlainText().isEmpty()){
       printf("1");
    }
    //Update Ymax
    if(!ui->ymax->toPlainText().isEmpty()){
       printf("1");
    }
    //Update Ymin
    if(!ui->ymin->toPlainText().isEmpty()){
        printf("1");
    }
    //Update Xmin
    if(!ui->xmin->toPlainText().isEmpty()){
        printf("1");
    }
}

