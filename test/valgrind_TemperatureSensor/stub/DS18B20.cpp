#include "DS18B20.h"
#include <QDebug>

DS18B20::DS18B20(int pin)
{
    //qDebug() << __func__ << __LINE__ << "pin" << pin;
    this->pin = pin;
}

double my_global_ds18b20 = 0;

bool DS18B20::getTemperature(double* value)
{
    //qDebug() << __func__ << __LINE__;
    *value = my_global_ds18b20;

    //Just use -1 as bad sensor
    if(*value == -1.00)
        return false;
    return true;
}

