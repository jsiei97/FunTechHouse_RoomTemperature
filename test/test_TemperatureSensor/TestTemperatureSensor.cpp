#include <QtCore>
#include <QtTest>

#include "TemperatureSensor.h"

class TestTemperatureSensor : public QObject
{
    Q_OBJECT

    private:
    public:

    private slots:
        //void test_add_data();
        //void test_add();

        void test_valueTimeToSend();
        void test_alarm();
        void test_setSensor();
        void test_setTopic();
};

/*
void TestTemperatureSensor::test_add_data()
{
    QTest::addColumn<int>("num1");
    QTest::addColumn<int>("num2");
    QTest::addColumn<int>("sum");

    QTest::newRow("add test 01") << 1 << 1 << 2;
    QTest::newRow("add test 02") << 2 << 2 << 4;
};


void TestTemperatureSensor::test_add()
{
    QFETCH(int, num1);
    QFETCH(int, num2);
    QFETCH(int, sum);


    //Calc calc;
    //QCOMPARE(calc.add(num1,num2), sum);
}
*/

/**
 * Test that we do not send to many duplicate data.
 */
void TestTemperatureSensor::test_valueTimeToSend()
{
    TemperatureSensor sensor;

    bool res = sensor.valueTimeToSend(10.0);
    QCOMPARE(res, true); //Always send the first time...
    for(int i=1; i<=180; i++)
    {
        res = sensor.valueTimeToSend(10.0);
        if(true == res)
        {
            qDebug() << "Error we should not send now" << i;
            QFAIL("Error valueSendCnt wrong");
        }
    }

    //Then we shall send
    QCOMPARE(sensor.valueTimeToSend(10.0), true);
    //And we are back in the loop
    QCOMPARE(sensor.valueTimeToSend(10.0), false);

    //Now check with a +diff
    QCOMPARE(sensor.valueTimeToSend(10.7), true);  //diff > 0.5 => OK
    QCOMPARE(sensor.valueTimeToSend(11.0), false); //diff < 0.5 => Not ok
    QCOMPARE(sensor.valueTimeToSend(11.3), true);  //diff 11.3-10.7=0.6 > 0.5 => OK

    //Now check with a -diff
    QCOMPARE(sensor.valueTimeToSend(10.3), true);  //diff > 0.5 => OK
    QCOMPARE(sensor.valueTimeToSend(10.0), false); //diff < 0.5 => Not ok
    QCOMPARE(sensor.valueTimeToSend( 9.7), true);  //diff 10.3-9.7=0.6 > 0.5 => OK
}


/**
 * Test the alarm functions 
 * setAlarmLevels, alarmHighCheck, alarmLowCheck, alarmHighFailed, alarmLowFailed
 */
void TestTemperatureSensor::test_alarm()
{
    TemperatureSensor sensor;

    //No alarms, never return true
    sensor.setAlarmLevels(false, 25.0, false, 20.0);

    char str[40];
    QCOMPARE(sensor.alarmHighCheck(15.0, str, 40), false);
    QCOMPARE(sensor.alarmHighCheck(22.0, str, 40), false);
    QCOMPARE(sensor.alarmHighCheck(35.0, str, 40), false);

    QCOMPARE(sensor.alarmLowCheck(15.0, str, 40), false);
    QCOMPARE(sensor.alarmLowCheck(22.0, str, 40), false);
    QCOMPARE(sensor.alarmLowCheck(25.0, str, 40), false);


    //High alarm active, but not low alarm.
    sensor.setAlarmLevels(true, 25.0, false, 25.0);
    QCOMPARE(sensor.alarmHighCheck(15.0, str, 40), false);
    QCOMPARE(sensor.alarmHighCheck(26.0, str, 40), true);
    QCOMPARE(sensor.alarmHighCheck(27.0, str, 40), false); //Only 1 alarm!
    sensor.alarmHighFailed();
    QCOMPARE(sensor.alarmHighCheck(26.0, str, 40), true);
    QCOMPARE(sensor.alarmHighCheck(26.0, str, 40), false);
    QCOMPARE(sensor.alarmHighCheck(24.9, str, 40), false); //Check that the hyst is working
    QCOMPARE(sensor.alarmHighCheck(25.1, str, 40), false);
    QCOMPARE(sensor.alarmHighCheck(23.0, str, 40), false); //Reset alarm, by dropping under the level
    QCOMPARE(sensor.alarmHighCheck(26.0, str, 40), true); // Then we get a new alarm

    //Low is still deactivated
    QCOMPARE(sensor.alarmLowCheck(20.0, str, 40), false);
    QCOMPARE(sensor.alarmLowCheck(30.0, str, 40), false);

    //Low alarm active, but no high alarm
    sensor.setAlarmLevels(false, 25.0, true, 20.0);
    QCOMPARE(sensor.alarmLowCheck(30.0, str, 40), false); //Higher no alarm
    QCOMPARE(sensor.alarmLowCheck(18.0, str, 40), true); //lower -> alarm
    QCOMPARE(sensor.alarmLowCheck(18.0, str, 40), false); //but only once
    QCOMPARE(sensor.alarmLowCheck(20.1, str, 40), false); //Check that hyst is working
    QCOMPARE(sensor.alarmLowCheck(19.9, str, 40), false); 
    QCOMPARE(sensor.alarmLowCheck(22.0, str, 40), false); //Reset by going over
    QCOMPARE(sensor.alarmLowCheck(18.0, str, 40), true);  //Then we get a new alarm
    QCOMPARE(sensor.alarmLowCheck(18.0, str, 40), false); //but only once

    sensor.alarmLowFailed(); //Send failed so we can mark that the alarm was not sent
    QCOMPARE(sensor.alarmLowCheck(18.0, str, 40), true); // Then we get a new alarm
    QCOMPARE(sensor.alarmLowCheck(18.0, str, 40), false); //but only once

    //High is deactivated
    QCOMPARE(sensor.alarmHighCheck(24.0, str, 40), false);
    QCOMPARE(sensor.alarmHighCheck(26.0, str, 40), false);

    // Both alarms active
    sensor.setAlarmLevels(true, 22.0, true, 20.0);

    QCOMPARE(sensor.alarmHighCheck(21.0, str, 40), false);
    QCOMPARE(sensor.alarmLowCheck(21.0, str, 40), false);

    QCOMPARE(sensor.alarmHighCheck(23.0, str, 40), true);
    QCOMPARE(sensor.alarmLowCheck(23.0, str, 40), false);

    QCOMPARE(sensor.alarmHighCheck(19.0, str, 40), false);
    QCOMPARE(sensor.alarmLowCheck(19.0, str, 40), true);
}


/**
 * Test the set sensor type and what pin is used
 */
void TestTemperatureSensor::test_setSensor()
{
    TemperatureSensor sensors[3];

    for( int i=0 ; i<3 ; i++ )
    {
        QCOMPARE(sensors[i].getSensorType(), (int)TemperatureSensor::NO_SENSOR);
    }

    sensors[1].setSensor(TemperatureSensor::LM35DZ, 10);

    QCOMPARE(sensors[0].getSensorType(), (int)TemperatureSensor::NO_SENSOR);
    QCOMPARE(sensors[0].getSensorPin(), 0);

    QCOMPARE(sensors[1].getSensorType(), (int)TemperatureSensor::LM35DZ);
    QCOMPARE(sensors[1].getSensorPin(), 10);

    QCOMPARE(sensors[2].getSensorType(), (int)TemperatureSensor::NO_SENSOR);
    QCOMPARE(sensors[2].getSensorPin(), 0);
}

void TestTemperatureSensor::test_setTopic()
{
    TemperatureSensor sensors[4];

    QCOMPARE( true, sensors[0].setTopic("in_0", "out_0") );
    QCOMPARE( true, sensors[1].setTopic("in_1", "out_1") );
    QCOMPARE( true, sensors[2].setTopic("in_2", "out_2") );
    QCOMPARE( true, sensors[3].setTopic("in_3", "out_3") );

    QCOMPARE( sensors[0].getTopicSubscribe(), "in_0" );
    QCOMPARE( sensors[0].getTopicPublish(),   "out_0" );

    QCOMPARE( sensors[3].getTopicSubscribe(), "in_3" );
    QCOMPARE( sensors[3].getTopicPublish(),   "out_3" );
}

QTEST_MAIN(TestTemperatureSensor)
#include "TestTemperatureSensor.moc"
