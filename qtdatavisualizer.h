#ifndef QTDATAVISUALIZER_H
#define QTDATAVISUALIZER_H

#include <QWidget>
#include <QList>
#include "log_data.h"
#include "qcustomplot.h"
#include "toolbar.h"

struct data_point{
    int param_index;
    float *data;
};

namespace Ui {
class QtDataVisualizer;
}

class QtDataVisualizer : public QWidget
{
    Q_OBJECT

public:
    explicit QtDataVisualizer(QWidget *parent = nullptr);
    ~QtDataVisualizer();
    int load_file(QString filename);
    int display_multiplot();
    int getNum_params() const;
    int getNum_units() const;
    int getNum_logs() const;
    log_data *getData() const;
    void setY_index(QList<int> *value);

public slots:
    void update_bounds();
    void open_toolbar(bool checked);
    void open_file(bool checked);

private:
    Ui::QtDataVisualizer *ui;
    log_data *data;
    double xmin, xmax, ymin, ymax;
    int x_index, graph_width, graph_height;
    QList<int>* y_index;
    QCustomPlot* qcp;
    toolbar* tool_bar;
    bool button_state;
    int num_params, num_units, num_logs;
    void line_parse(std::string line);

};

#endif // QTDATAVISUALIZER_H
