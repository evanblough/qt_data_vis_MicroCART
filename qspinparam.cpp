#include "qspinparam.h"

qSpinParam::qSpinParam(QWidget *parent) : QSpinBox(parent)
{
    logs = nullptr;
}

QString qSpinParam::textFromValue(int value) const{
    return (logs == nullptr) ? "0" : logs[value].param + " (" + logs[value].units + ")";
}

void qSpinParam::update_params(log_data *log_datas)
{
    free(logs);
    logs = log_datas;
}
