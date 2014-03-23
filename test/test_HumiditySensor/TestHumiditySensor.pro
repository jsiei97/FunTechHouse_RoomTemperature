CONFIG += qtestlib debug
TEMPLATE = app
TARGET = 
DEFINES += private=public

# Test code
DEPENDPATH += .
INCLUDEPATH += .
SOURCES += TestHumiditySensor.cpp

# Code to test
DEPENDPATH  += ../../FunTechHouse_RoomTemperature/
INCLUDEPATH += ../../FunTechHouse_RoomTemperature/
SOURCES += Sensor.cpp HumiditySensor.cpp StringHelp.cpp

