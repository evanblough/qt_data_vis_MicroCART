#ifndef TOOLBAR_H
#define TOOLBAR_H

#include <QWidget>
#include <QTreeView>
#include <QPushButton>
#include <QFileSystemModel>
#include "log_data.h"
#include <QVBoxLayout>

namespace Ui {
class toolbar;
}

class toolbar : public QWidget
{
    Q_OBJECT

public:
    explicit toolbar(QWidget *parent = nullptr);
    ~toolbar();
    QFileSystemModel *model;
    QTreeView* file_display;
    QPushButton* open_file, add_y;
    QList<int>* y_params;
    QVBoxLayout* layout_y_param;
    log_data* logs;
    void clearLayout(QLayout *layout);
    int *x_index;
    int *graph_width;
    int *graph_height;
    int  param_max;
    void update_state(int param_max, log_data* logs);
public slots:
    void addYParam(bool checked);
    void yParamChanged(int i);

signals:
    void update_graph();
private:
    Ui::toolbar *ui;
};

#endif // TOOLBAR_H
