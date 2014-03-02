/**
 * @file TemperatureSensor.h
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

#ifndef  __SENSOR_H
#define  __SENSOR_H

#include "MQTT_Logic.h"

// If value is the "same" for "cnt" questions, then send anyway.
// If sleep is 1s (1000ms) and there is 1 question per rotation
// then we have 600/1s=600s or always send every 10min
// 1200/1s/60s=20min
#define ALWAYS_SEND_CNT 1200

class Sensor : public MQTT_Logic
{
    public:
        enum SensorTypes {
            NO_SENSOR = 0,
            LM35DZ,
            DS18B20 
        };

    private:
        // Variables for the sensor config
        SensorTypes sensorType;///< What sensor is used here? See enum sensorTypes
        int connectedPin;      ///< What pin is he connected on?
        int sensorNumber;      ///< What object/sensor is this?

        //Variables for the value
        double valueWork;   ///< Active value that we work with right now
        double valueSent;   ///< Last value sent to the server
        double valueDiffMax;///< Value should diff more than this to be sent to the server
        int    valueSendCnt;///< Always send after "cnt time" even if there is no change
        double valueOffset; ///< Offset calibration value, this will just be added to the messured value

        //Varibles for alarm high
        double alarmHigh;      ///< Values higher than this will trigger an alarm
        bool   alarmHighActive;///< Is high alarm activated.
        bool   alarmHighSent;  ///< Have we sent this alarm? We only send alarm ones

        //Variables for alarm low
        double alarmLow;      ///< Values lower than this will trigger an alarm
        bool   alarmLowActive;///< Is low alarm activated.
        bool   alarmLowSent;  ///< Have we sent this alarm? We only send alarm ones

        double alarmHyst; ///< alarm level must go back this much to be reseted

    public:
        Sensor();

        bool valueTimeToSend(double value);
        void setDiffToSend(double value);
        void setValueOffset(double value);

        void setAlarmLevels(bool activeHigh, double high, bool activeLow, double low);
        bool alarmHighCheck(char* responce, int maxSize);
        bool alarmLowCheck (char* responce, int maxSize);
        void alarmHighFailed();
        void alarmLowFailed();

        void setSensor(SensorTypes type, int pin);
        int getSensorType();
        int getSensorPin();

};

#endif  // __SENSOR_H 
