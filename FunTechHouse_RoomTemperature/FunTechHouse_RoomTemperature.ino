#include <SPI.h>
#include <Ethernet.h>
#include "PubSubClient.h"
#include "DS18B20.h"
#include "LM35DZ.h"
#include "ValueAvg.h"
#include "TemperatureSensor.h"
#include "TemperatureLogic.h"

// Update these with values suitable for your network.
uint8_t mac[]    = { 0x90, 0xA2, 0xDA, 0x0D, 0x51, 0xB3 };

// The MQTT device name, this must be unique
char project_name[] = "FunTechHouse_RoomTemperature";

#define SENSOR_CNT 3
TemperatureSensor sensors[SENSOR_CNT];

PubSubClient client("mosqhub", 1883, callback);

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
    client.publish(sensors[0].getTopicPublish(), str);
}


void configure()
{
    //Config the first sensor
    sensors[0].setAlarmLevels(true, 25.0, true, 22.0);
    sensors[0].setSensor(TemperatureSensor::LM35DZ, A2);
    sensors[0].setDiffToSend(1.4);
    pinMode(A2, INPUT); //Is this needed?
    sensors[0].setTopic(
            "FunTechHouse/Room1/TemperatureData",
            "FunTechHouse/Room1/Temperature"
            );

    //Then configure a second sensor
    sensors[1].setAlarmLevels(true, 25.0, true, 22.0);
    sensors[1].setSensor(TemperatureSensor::LM35DZ, A3);
    sensors[1].setDiffToSend(1.4);
    pinMode(A3, INPUT); //Is this needed?
    sensors[1].setTopic(
            "FunTechHouse/Room2/TemperatureData",
            "FunTechHouse/Room2/Temperature"
            );

    //And a third, that is a DS18B20
    sensors[2].setAlarmLevels(true, 25.0, true, 22.0);
    sensors[2].setSensor(TemperatureSensor::DS18B20, 2);
    sensors[2].setDiffToSend(0.2);
    sensors[2].setTopic(
            "FunTechHouse/Room3/TemperatureData",
            "FunTechHouse/Room3/Temperature"
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
        bool readOk = false;

        if( ((int)TemperatureSensor::LM35DZ) == sensors[i].getSensorType() )
        {
            //There is some noice so take a avg on some samples
            //so we don't see the noice as much...
            filter.init();
            for( int j=0 ; j<9 ; j++ )
            {
                int reading = analogRead(sensors[i].getSensorPin());
                filter.addValue( LM35DZ::analog11_to_temperature(reading) );
            }
            temperature = filter.getValue();

            //No sensor connected becomes 109deg,
            //so lets just ignore values higher than 105
            if(temperature <= 105.0 && temperature != 0.0)
            {
                readOk = true;
            }
        }
        else if( ((int)TemperatureSensor::DS18B20) == sensors[i].getSensorType() )
        {
            temperature = DS18B20::getTemperature(sensors[i].getSensorPin());
            //This functions returns 0.0 when something is wrong,
            //maybe not the best value... let's change that some day....
            //
            //And 85.0 is the DS18B20 default error value,
            //so ignore that as well...
            if(temperature != 0.0 && temperature != 85.0)
            {
                readOk = true;
            }
        }

        if(true == readOk)
        {
            if(false == client.connected())
            {
                client.connect(project_name);
            }

            char str[40];

            //Check and save the current value
            if( sensors[i].valueTimeToSend(temperature) )
            {
                int intPart = 0;
                int decPart = 0;
                TemperatureLogic::splitDouble(temperature, &intPart, &decPart);
                snprintf(str, 40, "temperature=%d.%d", intPart, decPart);

                if(client.connected())
                {
                    client.publish(sensors[i].getTopicPublish(), str);
                }
            }

            if(sensors[i].alarmHighCheck(str, 40))
            {
                if( (false == client.connected()) || (false == client.publish(sensors[i].getTopicPublish(), str)) )
                {
                    sensors[i].alarmHighFailed();
                }
            }

            if(sensors[i].alarmLowCheck(str, 40))
            {
                if( (false == client.connected()) || (false == client.publish(sensors[i].getTopicPublish(), str)) )
                {
                    sensors[i].alarmLowFailed();
                }
            }
        }
    }

    delay(1000);
}
