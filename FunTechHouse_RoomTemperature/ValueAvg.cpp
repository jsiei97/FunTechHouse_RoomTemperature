/**
 * @file ValueAvg.cpp
 * @author Johan Simonsson
 * @brief A basic filter
 */

#include "ValueAvg.h"

ValueAvg::ValueAvg()
{
    init();
}

void ValueAvg::init()
{
    firstTime = true;
    higest = 0.0;
    smallest = 0.0;
    sum = 0.0;
    cnt = 0.0;
}

void ValueAvg::addValue(double data)
{
    sum += data;
    cnt++;
    if(true == firstTime)
    {
        higest = data;
        smallest = data;
    }
    else
    {
        if(data > higest)
        {
            higest = data;
        }
        else if(data < smallest)
        {
            smallest = data;
        }
    }

    firstTime = false;
}

double ValueAvg::getValue()
{
    if(cnt == 0)
    {
        return 0.0;
    }

    //If we have more than 7 numbers, then
    //get rid of the two extreems and calc avg on the rest.
    if(cnt >= 7)
    {
        sum -= smallest;
        sum -= higest;

        cnt -= 2;
    }

    return sum/cnt;
}

