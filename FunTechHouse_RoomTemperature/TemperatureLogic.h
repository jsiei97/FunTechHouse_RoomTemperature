/**
 * @file TemperatureLogic.h
 * @author Johan Simonsson  
 * @brief Helper functions
 */

/*
 * Copyright (C) 2013 Johan Simonsson
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef  __TEMPERATURELOGIC_H
#define  __TEMPERATURELOGIC_H

class TemperatureLogic
{
    private:
    public:
        static void splitDouble(double value, int* integer, int* decimal);
};

#endif  // __TEMPERATURELOGIC_H
