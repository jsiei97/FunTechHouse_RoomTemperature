/**
 * @file LM35DZ.cpp
 * @author Johan Simonsson
 * @brief Helper for the LM35DZ sensor
 */


#include "LM35DZ.h"


/**
 * Converts a analogRead value into a temperature (Celsius)
 * analogReference(EXTERNAL); should be used and Aref connected to 3.3V.
 *
 * @param reading from analogRead
 * @return temperature in degrees celsius
 */
double LM35DZ::analog33_to_temperature(int reading)
{
    double temperature = reading * 3.30; // 3.3V Aref
    temperature /= 1024.0;               // ADC resolution
    temperature *= 100;                  // 10mV/C (0.01V/C)
    return temperature;
}
