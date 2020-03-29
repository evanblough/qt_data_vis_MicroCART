#include "toolbar.h"
#include "ui_toolbar.h"
#include <QFileSystemModel>

toolbar::toolbar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::toolbar)
{
    ui->setupUi(this);
    model = new QFileSystemModel;
    model->setRootPath(QDir::currentPath());
    ui->file_display->setModel(model);
    open_file = ui->open_file;
    file_display = ui->file_display;
}

toolbar::~toolbar()
{
    delete ui;
}
