CONFIG += debug
QT     -= gui
TEMPLATE = app
TARGET =

# Test code
DEPENDPATH += .
INCLUDEPATH += .
SOURCES += TestTemperatureSensor.cpp
SOURCES += stub/DS18B20.cpp stub/LVTS.cpp

# Code to test
DEPENDPATH  += ../../FunTechHouse_RoomTemperature/
INCLUDEPATH += ../../FunTechHouse_RoomTemperature/
SOURCES += Sensor.cpp
SOURCES += TemperatureSensor.cpp
SOURCES += MQTT_Logic.cpp
SOURCES += TemperatureLogic.cpp

