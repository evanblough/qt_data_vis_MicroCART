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
    //UI / Widget Setup
    ui->setupUi(this);
    qcp = new QCustomPlot();
    QObject::connect(ui->xmax, &QTextEdit::textChanged , this, &QtDataVisualizer::update_bounds);
    QObject::connect(ui->xmin, &QTextEdit::textChanged , this, &QtDataVisualizer::update_bounds);
    QObject::connect(ui->ymax, &QTextEdit::textChanged , this, &QtDataVisualizer::update_bounds);
    QObject::connect(ui->ymin, &QTextEdit::textChanged , this, &QtDataVisualizer::update_bounds);

    //Default load_times
    num_logs = 0;
    num_params = 0;
    num_units = 0;

    //Default Params
    x_index = 0;
    y_index = new QList<int>();
    y_index->append(1);

    //Default Display Bounds
    xmin = 0.0;
    xmax = 1.01;
    ymin = 0.0;
    ymax = 1.01;
    graph_width = GRAPH_WIDTH;
    graph_height = GRAPH_HEIGHT;
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

//    //Set Ranges
//    if(!bound_flag){
//        qcp->xAxis->setRange(*std::min_element(x.constBegin(), x.constEnd()), *std::max_element(x.constBegin(), x.constEnd()));
//        qcp->yAxis->setRange(*std::min_element(y.constBegin(), y.constEnd()), *std::max_element(y.constBegin(), y.constEnd()));
//    }
//    else{
//        qcp->xAxis->setRange(xmin, xmax);
//        qcp->yAxis->setRange(ymin, ymax);
//    }

int QtDataVisualizer::display_multiplot(){
    delete qcp;
    qcp = new QCustomPlot();
    QVector<double> x(num_logs);
    //Y_axis Populate
    QVector<double>y_data[y_index->size()];
    for(int i = 0; i< y_index->size(); i++){
        y_data[i] = QVector<double>(num_logs);
        for(int j = 0; j < num_logs; j++){
            x[j] = data[0].values[j];
            y_data[i][j] = data[y_index->at(i)].values[j];
        }
    }
    //Setup Graph
    qcp->setFixedSize(graph_width, graph_height);
    ui->scrollArea->setWidget(qcp);
    QColor temp_color;
    qcp->legend->clearItems();
    for(int i = 0; i < y_index->size(); i++){
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
        qcp->graph(i)->setPen(QPen(temp_color,2,Qt::SolidLine));
        qcp->graph(i)->addToLegend(qcp->legend);
        qcp->legend->setIconSize(20, 20);
        QString name = data[y_index->at(i)].param + " " + data[y_index->at(i)].units;
        qcp->graph(i)->setName(name);

    }
    qcp->xAxis->setLabel(data[0].param.toStdString().c_str());
    qcp->xAxis->setRange(xmin, xmax);
    qcp->yAxis->setRange(ymin, ymax);
    qcp->legend->setVisible(true);

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
       xmax = ui->xmax->toPlainText().toDouble();
    }
    //Update Ymax
    if(!ui->ymax->toPlainText().isEmpty()){
       ymax = ui->ymax->toPlainText().toDouble();
    }
    //Update Ymin
    if(!ui->ymin->toPlainText().isEmpty()){
        ymin = ui->ymin->toPlainText().toDouble();
    }
    //Update Xmin
    if(!ui->xmin->toPlainText().isEmpty()){
        xmin = ui->xmin->toPlainText().toDouble();
    }
    //Update Plot
    qcp->replot();
    ui->scrollArea->setWidget(qcp);
    this->display_multiplot();
}

void QtDataVisualizer::setY_index(QList<int> *value)
{
    y_index = value;
}

