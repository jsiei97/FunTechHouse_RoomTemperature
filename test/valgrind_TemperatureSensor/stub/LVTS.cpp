#include "LVTS.h"
#include "SensorTypes.h"
#include <QDebug>

LVTS::LVTS(int pin, FT_SensorType type)
{
    //qDebug() << __func__ << __LINE__ << "pin" << pin << type;
    this->pin = pin;
    this->type = type;
}

double my_global_lm34 = 0;
double my_global_lm35 = 0;

bool LVTS::getTemperature(double* value)
{
    //qDebug() << __func__ << __LINE__;
    switch ( this->type )
    {
        case SENSOR_LVTS_LM34:
            *value = my_global_lm34;
            break;
        case SENSOR_LVTS_LM35:
            *value = my_global_lm35;
            break;
        default :
            break;
    }

    //Just use -1 as bad sensor
    if(*value == -1.00)
        return false;
    return true;
}

/*
double LVTS::lm35(int reading, bool *ok)
{
    qDebug() << __func__ << __LINE__;
}

double LVTS::lm34(int reading, bool *ok)
{
    qDebug() << __func__ << __LINE__;
}

double LVTS::F2C(double degC)
{
    qDebug() << __func__ << __LINE__;
}
*/
