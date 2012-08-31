CONFIG += qtestlib
TEMPLATE = app
TARGET = 
DEFINES += private=public

# Test code
DEPENDPATH += .
INCLUDEPATH += .
SOURCES += TestValueAvg.cpp

# Code to test
DEPENDPATH  += ../../FunTechHouse_RoomTemperature/
INCLUDEPATH += ../../FunTechHouse_RoomTemperature/
SOURCES += ValueAvg.cpp

