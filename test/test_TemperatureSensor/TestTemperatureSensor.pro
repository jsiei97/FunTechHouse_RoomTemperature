CONFIG += qtestlib
TEMPLATE = app
TARGET = 
DEFINES += private=public

# Test code
DEPENDPATH += .
INCLUDEPATH += .
SOURCES += TestTemperatureSensor.cpp

# Code to test
DEPENDPATH  += ../../FunTechHouse_RoomTemperature/
INCLUDEPATH += ../../FunTechHouse_RoomTemperature/
SOURCES += Sensor.cpp TemperatureSensor.cpp StringHelp.cpp

