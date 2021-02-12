/* This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

// Written by David Fernandez Amoros 2021

#ifndef BDDSampler_mytime_h
#define BDDSampler_mytime_h

#include <string>

#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#endif

#ifndef _WINDOWS_
#include <sys/time.h>
#include <sys/resource.h>
#else
#include <Windows.h>
#include <ctime>
#include <iostream>
#endif


std::string showtime(long ttime);
double get_cpu_time();
std::string get_timestamp();


#endif
