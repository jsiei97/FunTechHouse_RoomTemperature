/**
 * @file ValueAvg.h
 * @author Johan Simonsson
 * @brief A basic filter
 */

#ifndef  __VALUEAVG_H
#define  __VALUEAVG_H

class ValueAvg
{
    private:
        bool firstTime;
        double higest;
        double smallest;
        double sum;
        double cnt;

    public:
        ValueAvg();
        void init();
        void addValue(double data);
        double getValue();

};

#endif  // __VALUEAVG_H
