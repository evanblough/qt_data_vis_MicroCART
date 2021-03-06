#include "qtdatavisualizer.h"
#include "ui_qtdatavisualizer.h"
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <QModelIndex>
#include <QSpinBox>
#include "qcustomplot.h"
#include "QTextEdit"



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
    //QCustomPlot is the widget used to plot the data
    qcp = new QCustomPlot();
    //The toolbar allows a user to select log files and parameters to be graphed
    tool_bar = new toolbar();
    button_state = false;
    //Connect signals to update graph bounds based on the textbox files
    QObject::connect(ui->xmax, &QTextEdit::textChanged , this, &QtDataVisualizer::update_bounds);
    QObject::connect(ui->xmin, &QTextEdit::textChanged , this, &QtDataVisualizer::update_bounds);
    QObject::connect(ui->ymax, &QTextEdit::textChanged , this, &QtDataVisualizer::update_bounds);
    QObject::connect(ui->ymin, &QTextEdit::textChanged , this, &QtDataVisualizer::update_bounds);
    //Connect tool bar signals
    QObject::connect(ui->toolBar, &QPushButton::clicked, this, &QtDataVisualizer::open_toolbar);
    QObject::connect(tool_bar->open_file, &QPushButton::clicked, this, &QtDataVisualizer::open_file);
    QObject::connect(tool_bar, SIGNAL(update_graph()), this, SLOT(update_bounds()));

    //Default load_times
    num_logs = 0;
    num_params = 0;
    num_units = 0;

    //Default Params
    x_index = tool_bar->x_index;
    y_index = tool_bar->y_params;
    graph_width = tool_bar->graph_width;
    graph_height = tool_bar->graph_height;
    //Default Display Bounds
    xmin = 0.0;
    xmax = 1.01;
    ymin = 0.0;
    ymax = 1.01;

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
    num_logs = 0;
    num_params = 0;
    num_units = 0;
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
                    if(buffer.at(0) == '&'){
                        buffer.erase(0, 1);
                    }
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
    tool_bar->update_state(num_params, data);
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

/**
 * @brief QtDataVisualizer::display_multiplot This function is called to display a graph on the QCustomPlot widget that
 * matches the data selection of the current data log file, x axis,and  y axis parameters
 * @return zero on success
 */
int QtDataVisualizer::display_multiplot(){
    delete qcp;
    qcp = new QCustomPlot();
    QVector<double> x(num_logs);
    //Y_axis Populate
    QVector<double>y_data[y_index->size()];
    for(int i = 0; i< y_index->size(); i++){
        y_data[i] = QVector<double>(num_logs);
        for(int j = 0; j < num_logs; j++){
            x[j] = data[*x_index].values[j];
            y_data[i][j] = data[y_index->at(i)].values[j];
        }
    }
    //Setup Graph
    qcp->setFixedSize(*graph_width, *graph_height);
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
/**
 * @brief QtDataVisualizer::getNum_params
 * @return
 */
int QtDataVisualizer::getNum_params() const
{
    return num_params;
}
/**
 * @brief QtDataVisualizer::getNum_units
 * @return
 */
int QtDataVisualizer::getNum_units() const
{
    return num_units;
}
/**
 * @brief QtDataVisualizer::getNum_logs
 * @return
 */
int QtDataVisualizer::getNum_logs() const
{
    return num_logs;
}
/**
 * @brief QtDataVisualizer::getData
 * @return
 */
log_data *QtDataVisualizer::getData() const
{
    return data;
}
/**
 * @brief QtDataVisualizer::line_parse this function  is called to parse a single line of log data.
 * @param line
 */
void QtDataVisualizer::line_parse(std::string line)
{
    int param_index = 0;
    std::string buffer;

    for(int i = 0; i < line.length(); i++){
        if(line.at(i) != '\t'){
            if(line.at(i)  == '%'){
                i++;
            }
            std::string s(1, line.at(i));
            buffer.append(s);
        }
        else{
            data[param_index++].param = QString::fromStdString(buffer.c_str());
            buffer.clear();
        }
    }

}
/**
 * @brief QtDataVisualizer::update_bounds this slot is triggered whenever the bounds are updated
 * It updates the new bounds and replots the data.
 */
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
/**
 * @brief QtDataVisualizer::setY_index
 * @param value
 */
void QtDataVisualizer::setY_index(QList<int> *value)
{
    y_index = value;
}
/**
 * @brief QtDataVisualizer::open_toolbar opens the toolbar widget
 * @param checked
 */
void QtDataVisualizer::open_toolbar(bool checked){
    if(!button_state){
        tool_bar->show();
        button_state = true;
    }
    else{
        tool_bar->hide();
        button_state = false;
    }
}
/**
 * @brief QtDataVisualizer::open_file This slot is called to open a new data file
 * @param checked
 */
void QtDataVisualizer::open_file(bool checked)
{
    //Grab File name
    QModelIndex file_index = tool_bar->file_display->currentIndex();
    QString abs_path = tool_bar->model->filePath(file_index);
    printf("%s\n", abs_path.toStdString().c_str());
    load_file(abs_path);
    update_bounds();
}

