/**
 * @file LM35DZ.h
 * @author Johan Simonsson
 * @brief Helper for the LM35DZ sensor
 */


#ifndef  __LM35DZ_H
#define  __LM35DZ_H

class LM35DZ
{
     private:
     public:
         static double analog33_to_temperature(int reading);
         static double analog11_to_temperature(int reading);
};

#endif  // __LM35DZ_H
