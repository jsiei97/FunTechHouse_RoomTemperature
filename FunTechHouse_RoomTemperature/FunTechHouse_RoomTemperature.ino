/**
 * @file FunTechHouse_RoomTemperature.ino
 * @author Johan Simonsson
 * @brief Main file
 *
 * @mainpage The FunTechHouse RoomTemperature
 *
 * Room temperature sensor for the FunTechHouse project.
 * This project uses a Arduino with a Ethernet shield, 
 * and sends its results using MQTT to a Mosquitto server. 
 *
 * @see http://fun-tech.se/FunTechHouse/RoomTemperature/
 * @see https://github.com/jsiei97/FunTechHouse_RoomTemperature
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

#include <SPI.h>
#include <Ethernet.h>
#include "PubSubClient.h"
#include "Sensor.h"
#include "TemperatureSensor.h"
#include "SensorTypes.h"


/// This device MAC adress, it is written on the Shield and must be uniq.
uint8_t mac[]    = { 0x90, 0xA2, 0xDA, 0x0D, 0x51, 0xB3 };

/// The MQTT device name, this must be unique
char project_name[] = "FunTechHouse_RoomTemperature";

/// How many sensors shall the sensor array contain.
#define SENSOR_CNT 2
/// The sensor array with active sensors
Sensor sensor[SENSOR_CNT];

/// Max size for the out string used in the main loop
#define OUT_STR_MAX 100

/// Life blink led is connected to IO pin.
int led = 2;


/**
 * The MQTT subscribe callback function.
 *
 * @param[in] topic What mqtt topic triggered this callback
 * @param[in] payload The actual message
 * @param[in] length The message size
 */
void callback(char* topic, uint8_t* payload, unsigned int length)
{
}

/// The MQTT client
PubSubClient client("mosqhub", 1883, callback);

/**
 * Configure this project with device uniq sensor setup.
 */
void configure()
{
    //Config the first sensor
    sensor[0].init(A0, SENSOR_LVTS_LM35);
    sensor[0].setAlarmLevels(1.0, true, 16.0, true, 24.0);
    sensor[0].setValueDiff(1.4);
    sensor[0].setValueMaxCnt(30*60); //30*60s=>30min
    sensor[0].setTopic(
            "FunTechHouse/Room1/TemperatureData",
            "FunTechHouse/Room1/Temperature"
            );

    //And a second, that is a DS18B20
    sensor[1].init(A1, SENSOR_DS18B20);
    sensor[1].setAlarmLevels(1.0, true, 16.0, true, 24.0);
    sensor[1].setValueDiff(1.0);
    sensor[1].setValueMaxCnt(30*60); //30*60s=>30min
    sensor[1].setTopic(
            "FunTechHouse/Room2/TemperatureData",
            "FunTechHouse/Room2/Temperature"
            );
}

/**
 * First setup, runs once
 */
void setup()
{
    //INTERNAL: an built-in reference, equal to 1.1 volts on the ATmega168 or ATmega328
    analogReference(INTERNAL); //1.1V

    pinMode(led, OUTPUT);

    //Configure this project.
    configure();

    //Start ethernet, if no ip is given then dhcp is used.
    Ethernet.begin(mac);

    //Loop the sensors and tell mqtt server that we are alive.
    if (client.connect(project_name))
    {
        for( int i=0 ; i<SENSOR_CNT; i++ )
        {
            client.publish( sensor[i].getTopicPublish(), "#Hello world" );
            client.subscribe( sensor[i].getTopicSubscribe() );
        }
    }
}

/**
 * The main loop, runs all the time, over and over again.
 */
void loop()
{
    //Talk with the server so he dont forget us.
    if(client.loop() == false)
    {
        client.connect(project_name);
    }

    char str[OUT_STR_MAX];

    for( int i=0 ; i<SENSOR_CNT; i++ )
    {

        if( sensor[i].getTemperatureString(str, OUT_STR_MAX) )
        {
            if(client.connected())
            {
                client.publish(sensor[i].getTopicPublish(), str);
            }
        }

        //Now loop, send and Ack if there is any alarms
        int maxCnt = 20;
        SensorAlarmNumber num;
        do
        {
            num = sensor[i].alarmCheckString(str, OUT_STR_MAX);
            if(num != SENSOR_ALARM_NO)
            {
                if( client.connected() )
                {
                    if( client.publish(sensor[i].getTopicPublish(), str) )
                    {
                        sensor[i].alarmAck(num);
                    }
                }
            }

            maxCnt--;
        }
        while( num != SENSOR_ALARM_NO && maxCnt != 0);
    }


    digitalWrite(led, HIGH);
    delay(500);
    digitalWrite(led, LOW);
    delay(500);
}
