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

/**
 * Default constructur,
 * please note that all alarm are disabled.
 */
Sensor::Sensor()
{
    // No sensor
    sensorType = NO_SENSOR;
    connectedPin = 0;

    static int objCnt = 0;
    sensorNumber = objCnt++;

    topicIn = NULL;
    topicOut = NULL;
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

void Sensor::setSensorType(SensorTypes type)
{
    sensorType = type;
}

int Sensor::getSensorType()
{
    return sensorType;
}

void Sensor::setSensorPin(int pin)
{
    connectedPin = pin;
}

int Sensor::getSensorPin()
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
bool Sensor::setTopic(char* topicSubscribe, char* topicPublish)
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

char* Sensor::getTopicSubscribe()
{
    return topicIn;
}

char* Sensor::getTopicPublish()
{
    return topicOut;
}

bool Sensor::checkTopicSubscribe(char* check)
{
    bool res = false;

    if(0 == strcmp(check,topicIn))
    {
        res = true;
    }

    return res;
}
