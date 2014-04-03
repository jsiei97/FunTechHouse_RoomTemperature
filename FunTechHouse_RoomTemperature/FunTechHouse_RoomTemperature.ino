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
#include "DHT.h"
#include "DS18B20.h"
#include "LM35DZ.h"
#include "ValueAvg.h"
#include "HumiditySensor.h"
#include "TemperatureSensor.h"
#include "StringHelp.h"

// Update these with values suitable for your network.
uint8_t mac[]    = { 0x90, 0xA2, 0xDA, 0x0D, 0x51, 0xDB };

// The MQTT device name, this must be unique
char project_name[] = "FunTechHouse_KCC_735EA";

#define SENSOR_CNT 1
HumiditySensor sensors[SENSOR_CNT];

PubSubClient client("mosqhub", 1883, callback);

/// @todo Make it play nicer with the others...
DHT dht(2, DHT22);
int outPWM = 6; // PWM to KCC board


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
        if(true == sensors[i].checkTopicSubscribe(topic))
        {
            //Echo test on the same sensor
            client.publish(sensors[i].getTopicPublish(), str);
        }
    }
}


void configure()
{
    //Config the first sensor
    sensors[0].setAlarmLevels(
            25.0, false, 15.0, false, 
            70.0, true,  40.0, false);
    sensors[0].setSensor(Sensor::DHT_22, 2);
    sensors[0].setDiffToSend(0.5, 2.0);
    sensors[0].setTopic(
            "Granev5/0v/avfuktData",
            "Granev5/0v/avfukt"
            );

}

void setup()
{
    pinMode(outPWM, OUTPUT);   // sets the pin as output

    //INTERNAL: an built-in reference, equal to 1.1 volts on the ATmega168 or ATmega328
    analogReference(INTERNAL); //1.1V

    //Configure this project.
    configure();

    /// @todo Make it play nicer with the others...
    dht.begin();

    //Start ethernet, if no ip is given then dhcp is used.
    Ethernet.begin(mac);

    //Loop the sensors and tell mqtt server that we are alive.
    if (client.connect(project_name))
    {
        for( int i=0 ; i<SENSOR_CNT; i++ )
        {
            client.publish( sensors[i].getTopicPublish(), "#Hello world" );
            client.subscribe( sensors[i].getTopicSubscribe() );
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


    ValueAvg filter;
    for( int i=0 ; i<SENSOR_CNT; i++ )
    {
        double temperature = 0;
        double humidity    = 0;
        bool readOk = false;

        if( ((int)Sensor::DHT_22) == sensors[i].getSensorType() )
        {
            //Add the DHT set pin and reinit here, if needed...

            // Reading temperature or humidity takes about 250 milliseconds!
            // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
            humidity    = dht.readHumidity();
            temperature = dht.readTemperature();

            // check if returns are valid, if they are NaN (not a number) then something went wrong!
            if (isnan(temperature) || isnan(humidity)) {
                //Serial.println("Failed to read from DHT");
                readOk = false;
            } else {
                readOk = true;

                //Send a value to the KCC

                // 5V/0xFF = 0,0196V/steg
                // 1V/0,0196=>51 steg
                //val = 0xFF; //Max vid 3,8V pga OP
                //val = 0x80; //2.5V
                //val = 0x40; //1,27V

                //humidity=humidity*1.63;
                // analogWrite values from 0 to 255
                analogWrite(outPWM, (int)(humidity*1.63));  

            }
        }

        if(true == readOk)
        {
            if(false == client.connected())
            {
                client.connect(project_name);
            }

            char str[80];

            //Check and save the current value
            if( sensors[i].valueTimeToSend(temperature, humidity) )
            {
                int intPart = 0;
                int decPart = 0;
                StringHelp::splitDouble(temperature, &intPart, &decPart);

                int intPartHum = 0;
                int decPartHum = 0;
                StringHelp::splitDouble(humidity, &intPartHum, &decPartHum);
                snprintf(str, 80, "temperature=%d.%02d ; rh=%d.%02d", 
                        intPart, decPart, intPartHum, decPartHum);

                if(client.connected())
                {
                    client.publish(sensors[i].getTopicPublish(), str);
                }
            }

            if(sensors[i].alarmCheck(str, 80))
            {
                if( (false == client.connected()) || (false == client.publish(sensors[i].getTopicPublish(), str)) )
                {
                    sensors[i].alarmFailed();
                }
            }
        }
    }

    delay(1000);
}
