#ifndef QSPINPARAM_H
#define QSPINPARAM_H

#include <QObject>
#include <QWidget>
#include <QSpinBox>
#include "log_data.h"

class qSpinParam : public QSpinBox
{
    Q_OBJECT
public:
    explicit qSpinParam(QWidget *parent = nullptr);
    QString textFromValue(int value) const override;
    int index;
    void update_params(log_data* log_datas);
    log_data* logs;
signals:

public slots:
};

#endif // QSPINPARAM_H
