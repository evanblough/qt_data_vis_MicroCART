#include "qspinparam.h"
/**
 * @brief qSpinParam::qSpinParam the qspin param is a subclass of the Qspinbox class
 * This class works the same as a Qspinbox, but it has an overided textfromValue funciton to display the parameter name and units
 * @param parent
 */
qSpinParam::qSpinParam(QWidget *parent) : QSpinBox(parent)
{
    logs = nullptr;
}
/**
 * @brief qSpinParam::textFromValue overrided method of QSpinBox that displays the parameter of the
 * index as its name and units
 * @param value the value of the index ex: index 0 maps to string time usually
 * @return
 */
QString qSpinParam::textFromValue(int value) const{
    return (logs == nullptr) ? "0" : logs[value].param + " (" + logs[value].units + ")";
}
/**
 * @brief qSpinParam::update_params
 * @param log_datas
 */
void qSpinParam::update_params(log_data *log_datas)
{
    free(logs);
    logs = log_datas;
}
