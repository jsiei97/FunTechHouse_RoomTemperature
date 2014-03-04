
#include "Sensor.h"
#include "SensorTypes.h"

#include <QDebug>


extern double my_global_ds18b20;
extern double my_global_lm34;
extern double my_global_lm35;

#define OUT_STR_MAX 100

int main()
{
    double value;
    char str[OUT_STR_MAX];

    //qDebug() << "Test TemperatureSensor";
    
    Sensor* sensor = new Sensor[3];

    //Config the first sensor
    sensor[0].init(1, SENSOR_LVTS_LM35);
    sensor[0].setAlarmLevels(1.0, true, 17.0, true, 24.0);
    sensor[0].setValueDiff(1.4);
    sensor[0].setValueMaxCnt(30*60); //30*60s=>30min
    sensor[0].setTopic(
            (char*)"FunTechHouse/Room1/TemperatureData",
            (char*)"FunTechHouse/Room1/Temperature"
            );

    //And a second, that is a DS18B20
    sensor[1].init(1, SENSOR_DS18B20);
    sensor[1].setAlarmLevels(1.0, true, 17.0, true, 24.0);
    sensor[1].setValueDiff(1.0);
    sensor[1].setValueMaxCnt(30*60); //30*60s=>30min
    sensor[1].setTopic(
            (char*)"FunTechHouse/Room2/TemperatureData",
            (char*)"FunTechHouse/Room2/Temperature"
            );

    sensor[2].init(1, SENSOR_LVTS_LM34);
    sensor[2].setAlarmLevels(1.0, true, 17.0, true, 24.0);

    my_global_lm35    = 11.0;
    my_global_ds18b20 = 12.0;
    my_global_lm34    = 13.0;

    for( int z=0 ; z<40; z++ )
    {
        my_global_ds18b20 += 0.5;
        my_global_lm34    += 0.5;
        my_global_lm35    += 0.5;

        for( int i=0 ; i<3; i++ )
        {

            if( sensor[i].getTemperatureString(str, OUT_STR_MAX) )
            {
                printf("%s\n", str);
            }

            //Now loop, send and Ack if there is any alarms
            int maxCnt = 20;
            SensorAlarmNumber num;
            do
            {
                num = sensor[i].alarmCheckString(str, OUT_STR_MAX);
                if(num != SENSOR_ALARM_NO)
                {
                    printf("%s\n", str);
                    sensor[i].alarmAck(num);
                }
                maxCnt--;
            }
            while( num != SENSOR_ALARM_NO && maxCnt != 0);
        }
    }

    delete[] sensor;


    Sensor *s1 = new Sensor();

    s1->init(1, SENSOR_DS18B20);
    my_global_ds18b20 = 18.20;
    s1->getTemperature(&value);
    //qDebug() << value << my_global_ds18b20;

    delete s1;

    return 0;
}


