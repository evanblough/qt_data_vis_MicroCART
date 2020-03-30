#include "toolbar.h"
#include "ui_toolbar.h"
#include <QFileSystemModel>
#include <QPushButton>
#include "qspinparam.h"

#define GRAPH_WIDTH 1440
#define GRAPH_HEIGHT 720
/**
 * @brief toolbar::toolbar The tool bar widget is responsible for
 * @param parent
 */
toolbar::toolbar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::toolbar)
{
    ui->setupUi(this);
    //Allocatate memory for variables shared between toolbar and qtdatavis
    y_params = new QList<int>;
    x_index = (int*)malloc(sizeof(int));
    graph_width = (int*)malloc(sizeof(int));
    graph_height = (int*)malloc(sizeof(int));
    layout_y_param = new QVBoxLayout();
    y_params->append(0);
    //Set default x and y indexes for qspinparam widgets
    ui->y_select->index = 0;
    ui->x_select->index = -1;
    //Connect value changed signal to update values displayed by qtdata vis to match those in the qspinparam boxes
    QObject::connect(ui->y_select, SIGNAL(valueChanged(int)), this, SLOT(yParamChanged(int)));
    QObject::connect(ui->x_select, SIGNAL(valueChanged(int)), this, SLOT(yParamChanged(int)));
    //init class var values
    logs = nullptr;
    *x_index = 0;
    *graph_width = GRAPH_WIDTH;
    *graph_height = GRAPH_HEIGHT;
    //Setup file browser
    model = new QFileSystemModel;
    model->setRootPath(QDir::currentPath());
    ui->file_display->setModel(model);
    open_file = ui->open_file;
    file_display = ui->file_display;
    //Setup addYparam button
    QObject::connect(ui->add_Y, &QPushButton::clicked, this, &toolbar::addYParam);
}

toolbar::~toolbar()
{
    delete ui;
}
/**
 * @brief toolbar::update_state The update state function is called by the qtdata visualizer widget to
 * reset the graph parameters and layouts after the input log file has been changed. This prevents
 * the program from trying to graph data that doesn't exist anymore
 * @param param_max The number of parameters read in from the new logfile
 * @param logs The data read in from the new logfile
 */
void toolbar::update_state(int param_max, log_data* logs)
{
    //Copy refrence to new data logs
    this->logs = logs;
    //Set new ranges for qspinbox
    ui->y_select->setRange(0, param_max-1);
    ui->x_select->setRange(0, param_max-1);
    //Param max value is used for setting range of the non-default qspinboxes in the multiY scroll area
    this->param_max = param_max - 1;
    y_params->clear();
    clearLayout(layout_y_param);
    ui->multiY->setLayout(layout_y_param);
    ui->x_select->logs = logs;
    ui->y_select->logs = logs;
}
/**
 * @brief toolbar::addYParam This function adds a qspinparam widget to the multi-y scroll area
 * The qspinparam boxes allow a user to select which y axis parameters they want graphed on the plot.
 * @param checked not used
 */
void toolbar::addYParam(bool checked)
{
    qSpinParam* qs = new qSpinParam();
    qs->setRange(0, param_max);
    qs->index = y_params->size();
    qs->logs = this->logs;
    layout_y_param->addWidget(qs);
    ui->multiY->setLayout(layout_y_param);
    y_params->append(0);
    QObject::connect(qs, SIGNAL(valueChanged(int)), this, SLOT(yParamChanged(int)));

}
/**
 * @brief toolbar::yParamChanged this slot is invoked whenever one of the qspinparams have a new value selected.
 * A new value is selected when a user clicks up or down on the box. This allows users to choose wether they want X or Y setpoints graphed.
 * Name is misleading, because it also is used for the X params
 * @param i the new value that was selected by the user
 */
void toolbar::yParamChanged(int i)
{
    //Updates y param list to hold parameters selected by user
    qSpinParam* qs = (qSpinParam*)this->sender();
    if(qs->index != -1){
        y_params->removeAt(qs->index);
        y_params->insert(qs->index, i);
    }
    else{
        *x_index = i;
    }
    //New param was set, so update the graph plot
    emit toolbar::update_graph();

}
/**
 * @brief toolbar::clearLayout this is used to clear out the multi Y layouts whenever a new log file is selected
 * @param layout The layout to be cleared, layout_y_param
 */
void toolbar::clearLayout(QLayout *layout){
    QLayoutItem *item;
    while((item = layout->takeAt(0))) {
        if (item->layout()) {
            clearLayout(item->layout());
            delete item->layout();
        }
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }
}


