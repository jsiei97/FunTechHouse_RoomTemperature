/**
 * @file Sensor.cpp
 * @author Johan Simonsson
 * @brief A temperature sensor class with alarm logic
 */

/*
 * Copyright (C) 2013 Johan Simonsson
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Sensor.h"
#include "StringHelp.h"


bool Sensor::getTemperatureString(char* str, int size)
{
    double temperature;
    if( getTemperature(&temperature) )
    {
        int intPart = 0;
        int decPart = 0;
        StringHelp::splitDouble(temperature, &intPart, &decPart);
        snprintf(str, size, "temperature=%d.%02d", intPart, decPart);
        return true;
    }
    return false;
}


SensorAlarmNumber Sensor::alarmCheckString(char* str, int size)
{
    SensorAlarmNumber num = alarmCheck();
    switch ( num )
    {
        case SENSOR_ALARM_SENSOR:
            snprintf(str, size, "Alarm: Sensor error");
            break;
        case SENSOR_ALARM_HIGH:
            {
                int integerPart = 0;
                int decimalPart = 0;
                StringHelp::splitDouble(valueWork, &integerPart, &decimalPart);

                int intAlarm = 0;
                int decAlarm = 0;
                StringHelp::splitDouble(alarmHighLevel, &intAlarm, &decAlarm);

                snprintf(str, size, "Alarm: High temperature=%d.%02d level=%d.%02d",
                        integerPart, decimalPart, intAlarm, decAlarm);
            }
            break;
        case SENSOR_ALARM_LOW:
            {
                int integerPart = 0;
                int decimalPart = 0;
                StringHelp::splitDouble(valueWork, &integerPart, &decimalPart);

                int intAlarm = 0;
                int decAlarm = 0;
                StringHelp::splitDouble(alarmLowLevel, &intAlarm, &decAlarm);

                snprintf(str, size, "Alarm: Low temperature=%d.%02d level=%d.%02d",
                        integerPart, decimalPart, intAlarm, decAlarm);
            }
            break;
    }

    return num;
}
