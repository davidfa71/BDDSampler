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

#include "mytime.hpp"
#include <sstream>
#include <iomanip>

std::string showtime(long ttime) {
    std::ostringstream ost;
    int oneSec  = 1000;
    int oneMin  = 60*oneSec;
    int oneHour = 60*oneMin;
    int oneDay  = 24*oneHour;
    if (ttime > oneSec)
        if (ttime > oneMin)
            if (ttime > oneHour)
                if(ttime > oneDay)
                    ost << ttime / oneDay << " day, "
                        << (ttime % oneDay) / oneHour << " hou.";
                else
                    ost << ttime / oneHour << " hours, "
                        << std::setw(2) << (ttime % oneHour) / oneMin << " min. ";
                     // << std::setw(2) << (ttime % oneMin)  / oneSec << " sec.";
            else
                ost << std::setw(2) << (ttime / oneMin) << " min,  "
                    << std::setw(2) << ((ttime % oneMin) / oneSec) << " sec";
        else
                ost << std::setw(2) << (ttime / oneSec) << " sec, " << std::setw(3)
                    << ((ttime % oneSec)) << " ms.";
    else
        ost << std::setw(2) << ttime << " ms.";
    
    return ost.str();
}

#ifndef _WINDOWS_
double get_cpu_time()
{
    //return (double)clock()/(double)(CLOCKS_PER_SEC);
    double res;
    //struct rusage usage;
    struct timespec mySpec;
#ifdef __MACH__ // OS X does not have clock_gettime, use clock_get_time
    clock_serv_t cclock;
    mach_timespec_t mts;
    host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
    clock_get_time(cclock, &mts);
    mach_port_deallocate(mach_task_self(), cclock);
    mySpec.tv_sec = mts.tv_sec;
    mySpec.tv_nsec = mts.tv_nsec;
#else
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &mySpec);
#endif
    //cerr << "mySpec.tv_sec " << mySpec.tv_sec << " and mySpec.tv_nsec " << mySpec.tv_nsec              << endl;
    return mySpec.tv_sec * 1000.0 + mySpec.tv_nsec*0.000001;
    //getrusage(RUSAGE_SELF, &usage);
    
    //res = usage.ru_utime.tv_usec + usage.ru_stime.tv_usec;   //res *= 0.000001;
    //res += usage.ru_utime.tv_sec + usage.ru_stime.tv_sec;
    
    return 1000.0*res;
}
#else
double get_cpu_time()
{
    return clock();
}
#endif

std::string get_timestamp()
{
    std::string res = "";
    
    time_t rawtime;
    struct tm * timeinfo;
    char buffer [80];
    
    time (&rawtime);
    timeinfo = localtime (&rawtime);
    // Wednesday, March 19, 2014 01:06:18 PM
    //strftime (buffer,80,"%A, %B %d, %Y %I:%M:%S %p",timeinfo);
    //strftime (buffer,80,"[%D %T]",timeinfo);
    strftime (buffer,80,"[%T]",timeinfo);
    
    res = buffer;
    return res;
}
