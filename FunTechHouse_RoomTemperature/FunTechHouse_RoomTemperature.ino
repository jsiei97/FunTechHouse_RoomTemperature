/**
 * @file FunTechHouse_RoomTemperature.ino
 * @author Johan Simonsson
 * @brief Main file
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
#include "TemperatureLogic.h"


// Update these with values suitable for your network.
uint8_t mac[]    = { 0x90, 0xA2, 0xDA, 0x0D, 0x51, 0xB3 };

// The MQTT device name, this must be unique
char project_name[] = "FunTechHouse_RoomTemperature";

#define SENSOR_CNT 2
Sensor sensor[SENSOR_CNT];

EthernetClient ethClient;
PubSubClient client("mosqhub", 1883, callback, ethClient);

#define OUT_STR_MAX 256


void callback(char* topic, uint8_t* payload, unsigned int length)
{
    //1. What topic is it? aka to what sensor will the result go?
    //2. Parse the payload and update the correct sensor

    // handle message arrived
    char str[256];
    //snprintf(str, 256, "echo len=%d __%s__:__%s__", length, topic, payload);
    //client.publish(sensors[i].getTopicPublish(), str);

    unsigned int i=0;
    for(i=0; i<length; i++)
    {
        str[i] = payload[i];
        str[i+1] = '\0';
    }

    for( int i=0 ; i<SENSOR_CNT; i++ )
    {
        if(true == sensor[i].checkTopicSubscribe(topic))
        {
            //Echo test on the same sensor
            client.publish(sensor[i].getTopicPublish(), str);
        }
    }
}


void configure()
{
    //Config the first sensor
    sensor[0].init(A0, SENSOR_LVTS_LM35);
    sensor[0].setAlarmLevels(1.0, true, 24.0, true, 18.0);
    sensor[0].setValueDiff(1.4);
    sensor[0].setValueMaxCnt(30*60); //30*60s=>30min
    sensor[0].setTopic(
            "FunTechHouse/Room1/TemperatureData",
            "FunTechHouse/Room1/Temperature"
            );

    //And a second, that is a DS18B20
    sensor[1].init(A1, SENSOR_DS18B20);
    sensor[1].setAlarmLevels(1.0, true, 24.0, true, 18.0);
    sensor[1].setValueDiff(1.0);
    sensor[1].setValueMaxCnt(30*60); //30*60s=>30min
    sensor[1].setTopic(
            "FunTechHouse/Room2/TemperatureData",
            "FunTechHouse/Room2/Temperature"
            );
}

void setup()
{
    //INTERNAL: an built-in reference, equal to 1.1 volts on the ATmega168 or ATmega328
    analogReference(INTERNAL); //1.1V

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
        if(false == client.connected())
        {
            client.connect(project_name);
        }

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
                if(
                        (true == client.connected()) &&
                        (true == client.publish(sensor[i].getTopicPublish(), str)) )
                {
                    sensor[i].alarmAck(num);
                }
            }

            maxCnt--;
        }
        while( num != SENSOR_ALARM_NO && maxCnt != 0);


    }
    delay(1000);
}
