#include <SPI.h>
#include <Ethernet.h>
#include "PubSubClient.h"

#include "LM35DZ.h"
#include "TemperatureSensor.h"
#include "TemperatureLogic.h"
#include "ValueAvg.h"

#include "DeviceConfig.h"

PubSubClient client(server, 1883, callback);

#define SENSOR_CNT 1
TemperatureSensor sensors[SENSOR_CNT];

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

void setup()
{
    //analogReference(EXTERNAL); //3.3V

    //INTERNAL: an built-in reference, equal to 1.1 volts on the ATmega168 or ATmega328
    analogReference(INTERNAL); //1.1V

    sensors[0].setAlarmLevels(true, 25.0, true, 22.0);
    sensors[0].setSensor(TemperatureSensor::LM35DZ, A2);
    pinMode(A2, INPUT); //Is this needed?
    sensors[0].setTopic(
            "FunTechHouse/Room1/TemperatureData",
            "FunTechHouse/Room1/Temperature"
            );

    Ethernet.begin(mac, ip);
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
                //temperature = LM35DZ::analog33_to_temperature(reading);
                filter.addValue( LM35DZ::analog11_to_temperature(reading) );
            }
            temperature = filter.getValue();
            readOk = true;
        }

        if(true == readOk)
        {
            if(false == client.connected())
            {
                client.connect(project_name);
            }

            char str[40];

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

            if(sensors[i].alarmHighCheck(temperature, str, 40))
            {
                if( (false == client.connected()) || (false == client.publish(sensors[i].getTopicPublish(), str)) )
                {
                    sensors[i].alarmHighFailed();
                }
            }

            if(sensors[i].alarmLowCheck(temperature, str, 40))
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
