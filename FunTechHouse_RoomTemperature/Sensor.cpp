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
#include "TemperatureLogic.h"

/**
 * Default constructur,
 * please note that all alarm are disabled.
 */
Sensor::Sensor()
{
    // No sensor
    sensorType = NO_SENSOR;
    connectedPin = 0;

    //Some default values
    valueWork = 0.0;
    valueSent = 0.0;
    valueDiffMax = 0.8;
    valueSendCnt = 0;
    valueOffset = 0;

    alarmHigh = 25.0;
    alarmHighActive = false;
    alarmHighSent = false;

    alarmLow = 20.0;
    alarmLowActive = false;
    alarmLowSent = false;

    alarmHyst = 1.0;

    static int objCnt = 0;
    sensorNumber = objCnt++;
}

/**
 * Is it time to send a new value to the server,
 * this is triggered either on change or timeout.
 *
 * @param value The new temperature
 * @return true if it is time to send
 */
bool Sensor::valueTimeToSend(double value)
{
    valueWork = value+valueOffset;

    //Timeout lets send anyway
    if(0 == valueSendCnt)
    {
        valueSent = value;
        valueSendCnt = ALWAYS_SEND_CNT;
        return true;
    }

    //Check if diff is bigger than diff max
    if(value>valueSent)
    {
        if((value-valueSent) > valueDiffMax)
        {
            valueSent = value;
            valueSendCnt = ALWAYS_SEND_CNT;
            return true;
        }
    }

    //Check if negative diff is bigger than diff max
    if(value<valueSent)
    {
        if((valueSent-value) > valueDiffMax)
        {
            valueSent = value;
            valueSendCnt = ALWAYS_SEND_CNT;
            return true;
        }
    }

    valueSendCnt--;
    return false;
}

/**
 * Activate high and low alarm.
 *
 * @param activeHigh true will active the high alarm
 * @param high the high alarm level
 * @param activeLow true will active the low alarm
 * @param low the low alarm level
 */
void Sensor::setAlarmLevels(bool activeHigh, double high, bool activeLow, double low)
{
    alarmHighActive = activeHigh;
    alarmHigh = high;
    alarmHighSent = false;

    alarmLowActive = activeLow;
    alarmLow = low;
    alarmLowSent = false;
}


/**
 * How much must the value change before we send it?
 *
 * @param value if value is set to X, then we send directly if the messured value diff more than X from the last sent value.
 */
void Sensor::setDiffToSend(double value)
{
    valueDiffMax = value;
}

/**
 * If a sensor has a static measurment error, 
 * this offset value can be added to correct such a error.
 *
 * @param value the offset value that will be added
 */
void Sensor::setValueOffset(double value)
{
    valueOffset = value;
}

bool Sensor::alarmHighCheck(char* responce, int maxSize)
{
    bool sendAlarm = false;
    if(valueWork > alarmHigh)
    {
        if(alarmHighActive && !alarmHighSent)
        {
            alarmHighSent = true;

            int integerPart = 0;
            int decimalPart = 0;
            TemperatureLogic::splitDouble(valueWork, &integerPart, &decimalPart);

            int intAlarm = 0;
            int decAlarm = 0;
            TemperatureLogic::splitDouble(alarmHigh, &intAlarm, &decAlarm);

            snprintf(responce, maxSize, "Alarm: High temperature=%d.%d level=%d.%d",
                    integerPart, decimalPart, intAlarm, decAlarm);
            sendAlarm = true;
        }
    }
    else if( valueWork < (alarmHigh-alarmHyst ) )
    {
        alarmHighSent = false;
    }
    return sendAlarm;
}

bool Sensor::alarmLowCheck(char* responce, int maxSize)
{
    bool sendAlarm = false;
    if(valueWork < alarmLow)
    {
        if(alarmLowActive && !alarmLowSent)
        {
            alarmLowSent = true;

            int integerPart = 0;
            int decimalPart = 0;
            TemperatureLogic::splitDouble(valueWork, &integerPart, &decimalPart);

            int intAlarm = 0;
            int decAlarm = 0;
            TemperatureLogic::splitDouble(alarmLow, &intAlarm, &decAlarm);

            snprintf(responce, maxSize, "Alarm: Low temperature=%d.%d level=%d.%d",
                    integerPart, decimalPart, intAlarm, decAlarm);
            sendAlarm = true;
        }
    }
    else if( valueWork > (alarmLow+alarmHyst) )
    {
        alarmLowSent = false;
    }

    return sendAlarm;
}

/**
 * Tell the logic that we did not send that alarm.
 */
void Sensor::alarmHighFailed()
{
    alarmHighSent = false;
}

/**
 * Tell the logic that we did not send that alarm.
 */
void Sensor::alarmLowFailed()
{
    alarmLowSent = false;
}


/**
 * What kind of sensor is this?
 * And where is it connected?
 *
 * Please note that for analog sensors valid pins are A0..A5,
 * and for OneWire A0..A5, 2..3 and 5..9
 * (since the ethernet board uses 4,10..13).
 *
 * @param type What type it is
 * @param pin What pin it is connected on
 */
void Sensor::setSensor(SensorTypes type, int pin)
{
    sensorType = type;
    connectedPin = pin;
}

int Sensor::getSensorType()
{
    return sensorType;
}

int Sensor::getSensorPin()
{
    return connectedPin;
}
