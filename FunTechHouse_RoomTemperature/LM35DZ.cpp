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

    //Datasheet tells us
    //-"Rated for full −55 ̊ to +150 ̊C range"
    if( temperature <= 150 && temperature >= -55)
    {
        return temperature;
    }
    return 0.0;
}

/**
 * Converts a analogRead value into a temperature (Celsius)
 * analogReference(INTERNAL); should be used.
 *
 * @param reading from analogRead
 * @return temperature in degrees celsius
 */
double LM35DZ::analog11_to_temperature(int reading)
{
    double temperature = reading * 1.10; // Internal 1.1V ref
    temperature /= 1024.0;               // ADC resolution
    temperature *= 100;                  // 10mV/C (0.01V/C)

    //Datasheet tells us
    //-"Rated for full −55 ̊ to +150 ̊C range"
    if( temperature <= 150 && temperature >= -55)
    {
        return temperature;
    }
    return 0.0;
}