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
    valueSent = 0.0;
    valueDiffMax = 0.5;
    valueSendCnt = 0;

    alarmHigh = 25.0;
    alarmHighActive = false;
    alarmHighSent = false;

    alarmLow = 20.0;
    alarmLowActive = false;
    alarmLowSent = false;

    static int objCnt = 0;
    sensorNumber = objCnt++;

    topicIn = NULL;
    topicOut = NULL;
}

#define ALWAYS_SEND_CNT 180
/**
 * Is it time to send a new value to the server, 
 * this is triggered either on change or timeout.
 *
 * @param value The new temperature
 * @return true if it is time to send
 */
bool TemperatureSensor::valueTimeToSend(double value)
{
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

    alarmLowActive = activeLow;
    alarmLow = low;
}

bool TemperatureSensor::alarmHighCheck(double value, char* responce, int maxSize)
{
    bool sendAlarm = false;
    if(value > alarmHigh)
    {
        if(alarmHighActive && !alarmHighSent)
        {
            alarmHighSent = true;

            int integerPart = 0;
            int decimalPart = 0;
            TemperatureLogic::splitDouble(value, &integerPart, &decimalPart);

            int intAlarm = 0;
            int decAlarm = 0;
            TemperatureLogic::splitDouble(alarmHigh, &intAlarm, &decAlarm);

            snprintf(responce, maxSize, "Alarm: High temperature=%d.%d level=%d.%d",
                    integerPart, decimalPart, intAlarm, decAlarm);
            sendAlarm = true;
        }
    }
    else
    {
        alarmHighSent = false;
    }
    return sendAlarm;
}

bool TemperatureSensor::alarmLowCheck(double value, char* responce, int maxSize)
{
    bool sendAlarm = false;
    if(value < alarmLow)
    {
        if(alarmLowActive && !alarmLowSent)
        {
            alarmLowSent = true;

            int integerPart = 0;
            int decimalPart = 0;
            TemperatureLogic::splitDouble(value, &integerPart, &decimalPart);

            int intAlarm = 0;
            int decAlarm = 0;
            TemperatureLogic::splitDouble(alarmLow, &intAlarm, &decAlarm);

            snprintf(responce, maxSize, "Alarm: Low temperature=%d.%d level=%d.%d",
                    integerPart, decimalPart, intAlarm, decAlarm);
            sendAlarm = true;
        }
    }
    else
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
