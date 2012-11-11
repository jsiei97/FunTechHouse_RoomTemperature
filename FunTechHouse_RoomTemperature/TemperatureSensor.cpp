/**
 * @file TemperatureSensor.cpp
 * @author Johan Simonsson
 * @brief A temperature sensor class with alarm logic
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "TemperatureSensor.h"
#include "TemperatureLogic.h"

/**
 * Default constructur,
 * please note that all alarm are disabled.
 */
TemperatureSensor::TemperatureSensor()
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

    topicIn = NULL;
    topicOut = NULL;
}

/**
 * Is it time to send a new value to the server,
 * this is triggered either on change or timeout.
 *
 * @param value The new temperature
 * @return true if it is time to send
 */
bool TemperatureSensor::valueTimeToSend(double value)
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
void TemperatureSensor::setAlarmLevels(bool activeHigh, double high, bool activeLow, double low)
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
void TemperatureSensor::setDiffToSend(double value)
{
    valueDiffMax = value;
}

/**
 * If a sensor has a static measurment error, 
 * this offset value can be added to correct such a error.
 *
 * @param value the offset value that will be added
 */
void TemperatureSensor::setValueOffset(double value)
{
    valueOffset = value;
}

bool TemperatureSensor::alarmHighCheck(char* responce, int maxSize)
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

bool TemperatureSensor::alarmLowCheck(char* responce, int maxSize)
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
void TemperatureSensor::alarmHighFailed()
{
    alarmHighSent = false;
}

/**
 * Tell the logic that we did not send that alarm.
 */
void TemperatureSensor::alarmLowFailed()
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
void TemperatureSensor::setSensor(SensorTypes type, int pin)
{
    sensorType = type;
    connectedPin = pin;
}

int TemperatureSensor::getSensorType()
{
    return sensorType;
}

int TemperatureSensor::getSensorPin()
{
    return connectedPin;
}

/**
 * What mqtt topics this sensor will use.
 *
 * @param topicSubscribe data from the mqtt server
 * @param topicPublish data to the mqtt server
 * @return true if ok
 */
bool TemperatureSensor::setTopic(char* topicSubscribe, char* topicPublish)
{
    int len = strlen(topicSubscribe);
    topicIn = (char*)malloc(len+1);
    memcpy(topicIn , topicSubscribe, len);
    topicIn[len] = '\0';

    len = strlen(topicPublish);
    topicOut = (char*)malloc(len+1);
    memcpy(topicOut , topicPublish, len);
    topicOut[len] = '\0';

    return true;
}

char* TemperatureSensor::getTopicSubscribe()
{
    return topicIn;
}

char* TemperatureSensor::getTopicPublish()
{
    return topicOut;
}
