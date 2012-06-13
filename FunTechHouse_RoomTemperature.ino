#include <SPI.h>
#include <Ethernet.h>
#include "PubSubClient.h"

// Update these with values suitable for your network.
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0x01 };
byte ip[]     = { 192, 168, 0, 31 };
byte server[] = { 192, 168, 0, 64 };

char project_name[]  = "FunTechHouse_RoomTemperature";
char topic_out[] = "FunTechHouse/Room1/Temperature";

PubSubClient client(server, 1883, callback);

void callback(char* topic, byte* payload,unsigned int length) 
{
    // handle message arrived
}

void setup()
{
    analogReference(EXTERNAL); //3.3V

    Ethernet.begin(mac, ip);
    if (client.connect(project_name)) 
    {
        client.publish(topic_out, "#Hello world");
        //client.subscribe("inTopic");
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

    char str[40];
    int temp_hel = (int)(temperature);
    int temp_del = (int)((temperature-temp_hel)*10);

    snprintf(str, 40, "temperature=%d.%d ; raw=%04d", temp_hel, temp_del, reading);

    if(client.connected())
    {
        client.publish(topic_out, str);
    }

    delay(10000); 
}
