#include <SPI.h>
#include <Ethernet.h>
#include "PubSubClient.h"

// Update these with values suitable for your network.
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0x01 };
byte ip[]     = { 192, 168, 0, 31 };
byte server[] = { 192, 168, 0, 64 };

char project_name[]  = "FunTechHouse_RoomTemperature";
char topic_in[]  = "FunTechHouse/Room1/TemperatureData"; ///< data from the server
char topic_out[] = "FunTechHouse/Room1/Temperature";     ///< data to the server

PubSubClient client(server, 1883, callback);

volatile bool alarm_high_active = true;
volatile bool alarm_low_active  = true;

volatile double alarm_high = 25;
volatile double alarm_low  = 23;

volatile bool alarm_high_sent = false;
volatile bool alarm_low_sent  = false;



void callback(char* topic, uint8_t* payload, unsigned int length) 
{
    // handle message arrived
    char str[256];
    //snprintf(str, 256, "echo len=%d __%s__:__%s__", length, topic, payload);
    //client.publish(topic_out, str);

    int i=0;
    for(i=0; i<length; i++)
    {
        str[i] = payload[i];
        str[i+1] = '\0';
    }
    client.publish(topic_out, str);
    
}

void setup()
{
    analogReference(EXTERNAL); //3.3V

    Ethernet.begin(mac, ip);
    if (client.connect(project_name)) 
    {
        client.publish(topic_out, "#Hello world");
        client.subscribe(topic_in);
    }
}

void loop()
{
    int reading = analogRead(A2);
    double temperature = reading * 3.30; // 3.3V Aref 
    temperature /= 1024.0;               // ADC resolution
    temperature *= 100;                  // 10mV/C (0.01V/C)

    if(client.loop() == false)
    {
        client.connect(project_name);
    }

    int temp_hel = (int)(temperature);
    int temp_del = (int)((temperature-temp_hel)*10);

    if( temperature < alarm_low ) 
    {
        if(alarm_low_active && !alarm_low_sent) 
        {
            alarm_low_sent = true;
            char alarm_str[40];
            snprintf(alarm_str, 40, "Alarm: Low temperature=%d.%d level=%d", temp_hel, temp_del, (int)alarm_low);
            client.publish(topic_out, alarm_str);
        }
    }
    else 
    {
        alarm_low_sent = false;
    }


    if( temperature > alarm_high ) 
    {
        if(alarm_high_active && !alarm_high_sent) 
        {
            alarm_high_sent = true;
            char alarm_str[40];
            snprintf(alarm_str, 40, "Alarm: High temperature=%d.%d level=%d", temp_hel, temp_del, (int)alarm_high);
            client.publish(topic_out, alarm_str);
        }
    }
    else 
    {
        alarm_high_sent = false;
    }



    char str[40];
    snprintf(str, 40, "temperature=%d.%d ; raw=%04d", temp_hel, temp_del, reading);

    if(client.connected())
    {
        client.publish(topic_out, str);
    }

    delay(10000); 
}
