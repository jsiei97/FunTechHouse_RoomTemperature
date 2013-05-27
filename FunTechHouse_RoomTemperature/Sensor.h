/**
 * @file Sensor.h
 * @author Johan Simonsson
 * @brief A generic sensor class with mqtt logic
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

class Sensor
{
    public:
        enum SensorTypes {
            NO_SENSOR = 0,
            LM35DZ,
            DS18B20 
        };

    protected:
        // Variables for the sensor config
        SensorTypes sensorType;///< What sensor is used here? See enum sensorTypes
        int connectedPin;      ///< What pin is he connected on?
        int sensorNumber;      ///< What object/sensor is this?

        char* topicIn; ///< MQTT topic for data from the server
        char* topicOut;///< MQTT topic for data to the server

    public:
        Sensor();

        void setSensor(SensorTypes type, int pin);
		void setSensorType(SensorTypes type);
		void setSensorPin(int pin);
        int getSensorType();
        int getSensorPin();

        bool setTopic(char* topicSubscribe, char* topicPublish);
        char* getTopicSubscribe();
        char* getTopicPublish();
        bool checkTopicSubscribe(char* check);

};

#endif  // __SENSOR_H 
