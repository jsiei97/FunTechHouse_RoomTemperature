#ifndef  __TEMPERATURESENSOR_H
#define  __TEMPERATURESENSOR_H

class TemperatureSensor
{
    public:
        enum SensorTypes {
            NO_SENSOR = 0,
            LM35DZ
        };

    private:
        // Variables for the sensor config
        SensorTypes sensorType;///< What sensor is used here? See enum sensorTypes
        int connectedPin;      ///< What pin is he connected on?
        int sensorNumber;      ///< What object/sensor is this?

        //Variables for the value
        double valueSent;   ///< Last value sent to the server
        double valueDiffMax;///< Value should diff more than this to be sent to the server
        int    valueSendCnt;///< Always send after "cnt time" even if there is no change

        //Varibles for alarm high
        double alarmHigh;      ///< Values higher than this will trigger an alarm
        bool   alarmHighActive;///< Is high alarm activated.
        bool   alarmHighSent;  ///< Have we sent this alarm? We only send alarm ones

        //Variables for alarm low
        double alarmLow;      ///< Values lower than this will trigger an alarm
        bool   alarmLowActive;///< Is low alarm activated.
        bool   alarmLowSent;  ///< Have we sent this alarm? We only send alarm ones

        char* topicIn; ///< MQTT topic for data from the server
        char* topicOut;///< MQTT topic for data to the server

    public:
        TemperatureSensor();

        bool valueTimeToSend(double value);

        void setAlarmLevels(bool activeHigh, double high, bool activeLow, double low);
        bool alarmHighCheck(double value, char* responce, int maxSize);
        bool alarmLowCheck (double value, char* responce, int maxSize);
        void alarmHighFailed();
        void alarmLowFailed();

        void setSensor(SensorTypes type, int pin);
        int getSensorType();
        int getSensorPin();

        bool setTopic(char* topicSubscribe, char* topicPublish);
        char* getTopicSubscribe();
        char* getTopicPublish();

};

#endif  // __TEMPERATURESENSOR_H
