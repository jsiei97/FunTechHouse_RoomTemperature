/**
 * @file TemperatureLogic.cpp
 * @author Johan Simonsson
 * @brief Helper functions
 */

#include "TemperatureLogic.h"

/**
 * Split a double into the integer and decimal part,
 * since the arduino sprintf cant handle double.
 *
 * @param value[in] The value to split
 * @param integer[out] The integer part that will be returned
 * @param decimal[out] The decimal part that will be returned
 */
void TemperatureLogic::splitDouble(double value, int* integer, int* decimal)
{
    *integer = (int)(value);
    *decimal = (int)((value-*integer)*10);
}
