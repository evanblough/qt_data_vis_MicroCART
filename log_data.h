#ifndef LOG_DATA_H
#define LOG_DATA_H

#include <QString>

class log_data
{
public:
    log_data();
    QString param;
    QString units;
    double* values;

};

#endif // LOG_DATA_H
