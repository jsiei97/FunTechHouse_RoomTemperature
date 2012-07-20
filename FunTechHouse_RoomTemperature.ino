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

volatile bool larm_over_active  = true;
volatile bool larm_under_active = true;

volatile double larm_over  = 25;
volatile double larm_under = 23;

volatile bool larm_over_sent  = false;
volatile bool larm_under_sent = false;



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

    if( temperature < larm_under ) 
    {
        if(larm_under_active && !larm_under_sent) 
        {
            larm_under_sent = true;
            char larm_str[40];
            snprintf(larm_str, 40, "larm below temperature=%d.%d level=%d", temp_hel, temp_del, (int)larm_under);
            client.publish(topic_out, larm_str);
        }
    }
    else 
    {
        larm_under_sent = false;
    }


    if( temperature > larm_over ) 
    {
        if(larm_over_active && !larm_over_sent) 
        {
            larm_over_sent = true;
            char larm_str[40];
            snprintf(larm_str, 40, "larm over temperature=%d.%d level=%d", temp_hel, temp_del, (int)larm_over);
            client.publish(topic_out, larm_str);
        }
    }
    else 
    {
        larm_over_sent = false;
    }



    char str[40];
    snprintf(str, 40, "temperature=%d.%d ; raw=%04d", temp_hel, temp_del, reading);

    if(client.connected())
    {
        client.publish(topic_out, str);
    }

    delay(10000); 
}
