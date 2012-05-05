#include <SPI.h>
#include <Ethernet.h>
//#include <PubSubClient.h>
#include "PubSubClient.h"

// Update these with values suitable for your network.
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0x01 };
byte ip[]     = { 192, 168, 0, 31 };
byte server[] = { 192, 168, 0, 64 };


void callback(char* topic, byte* payload,unsigned int length) 
{
    // handle message arrived
}

PubSubClient client(server, 1883, callback);

void setup()
{
    analogReference(EXTERNAL); //3.3V

    Ethernet.begin(mac, ip);
    if (client.connect("FunTechHouse_RoomTemperature")) 
    {
        client.publish("FunTechHouse/Room1/temperature","#hello world");
        //client.subscribe("inTopic");
    }
}

//uint8_t miss_cnt = 0;

void loop()
{
    int reading = analogRead(A2);  
    float temperature = reading * 330; //Aref 3,3V 10mV/C
    temperature /= 1024.0; 

    if(client.loop() == false)
    {
        client.connect("FunTechHouse_RoomTemperature");
        //miss_cnt++;
    }

    char str[30];
    int temp_hel = (int)(temperature);
    int temp_del = (int)((temperature-temp_hel)*10);

    snprintf(str, 30, "temperature=%d.%d ; raw=%04d", temp_hel, temp_del, reading);

    if(client.connected())
    {
        client.publish("FunTechHouse/Room1/temperature", str);
    }

    delay(10000); 
}

