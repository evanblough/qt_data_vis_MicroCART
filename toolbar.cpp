#include "toolbar.h"
#include "ui_toolbar.h"
#include <QFileSystemModel>
#include <QPushButton>
#include "qspinparam.h"

#define GRAPH_WIDTH 1440
#define GRAPH_HEIGHT 720

toolbar::toolbar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::toolbar)
{
    ui->setupUi(this);
    y_params = new QList<int>;
    x_index = (int*)malloc(sizeof(int));
    graph_width = (int*)malloc(sizeof(int));
    graph_height = (int*)malloc(sizeof(int));
    logs = nullptr;
    *x_index = 0;
    *graph_width = GRAPH_WIDTH;
    *graph_height = GRAPH_HEIGHT;
    model = new QFileSystemModel;
    model->setRootPath(QDir::currentPath());
    ui->file_display->setModel(model);
    open_file = ui->open_file;
    file_display = ui->file_display;
    layout_y_param = new QVBoxLayout();
    QObject::connect(ui->add_Y, &QPushButton::clicked, this, &toolbar::addYParam);
}

toolbar::~toolbar()
{
    delete ui;
}

void toolbar::update_state(int param_max, log_data* logs)
{
    this->logs = logs;
    ui->y_select->setRange(0, param_max-1);
    ui->x_select->setRange(0, param_max-1);
    y_params->clear();
    //clearLayout(ui->scrollAreaWidgetContents->layout());
    ui->x_select->logs = logs;
    ui->y_select->logs = logs;
}

void toolbar::addYParam(bool checked)
{
    QLabel* label = new QLabel(QString::number(y_params->size()));
    layout_y_param->addWidget(label);
    label->show();
    qSpinParam* qs = new qSpinParam();
    layout_y_param->addWidget(qs);
    ui->multiY->setLayout(layout_y_param);
}

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


