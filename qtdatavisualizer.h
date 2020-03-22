#ifndef QTDATAVISUALIZER_H
#define QTDATAVISUALIZER_H

#include <QWidget>
#include <QList>
#include "log_data.h"

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
    int display_params(int x_index, int y_index, double xmin, double xmax, double ymin, double ymax, int bound_flag);
    int display_multiplot(QList<int>* y_axis, int x_axis, double xmin, double xmax, double ymin, double ymax, int bound_flag);
    int getNum_params() const;
    int getNum_units() const;
    int getNum_logs() const;
    log_data *getData() const;

public slots:
    void update_bounds();

private:
    Ui::QtDataVisualizer *ui;
    log_data *data;
    int num_params, num_units, num_logs;
    void line_parse(std::string line);
};

#endif // QTDATAVISUALIZER_H
