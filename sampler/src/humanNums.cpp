
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

#include "humanNums.hpp"

std::string showHuman(double n) {
    std::ostringstream ost;
    ost.setf(std::ios_base::fixed);
    if (n < 1E3)
        ost << n;
        else
        if (n < 1E6)
            //ost << std::setw(3) << (n/1000) << "k";
            ost << (n/1000) << "k";
        else {
            //std::cout.precision(2);
            //ost << (n/1000000.0) << "M";
            //std::cout.unsetf ( std::ios::floatfield );
            //ost <<  std::setprecision(2) << std::setw(6) << (n/1E6) << "M";
            ost <<  std::setprecision(2) << (n/1E6) << "M";
        }
        return ost.str();
}

std::string showHuman(int n) {
    std::ostringstream ost;
    ost.setf(std::ios_base::fixed);
    if (n < 1E3)
        ost << n;
    else
        if (n < 1E6)
            //ost << std::setw(3) << (n/1000) << "k";
            ost << (n/1000) << "k";
        else {
            std::cout.precision(2);
            //ost << (n/1000000.0) << "M";
            //std::cout.unsetf ( std::ios::floatfield );
            //ost  << std::setprecision(2) << std::setw(6) << (n/1E6) << "M";
            ost  << std::setprecision(2)  << (n/1E6) << "M";
        }
    return ost.str();
}
