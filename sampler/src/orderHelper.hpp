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

#ifndef orderHelper_hpp
#define orderHelper_hpp

#include <stdio.h>

class orderHelper {
    public :
    orderHelper(int id, int pos, int compstart, float avg) : id(id), pos(pos), compstart(compstart), avg(avg){};
    int id, pos, compstart;
    float avg;
    bool operator()(orderHelper& a, orderHelper& b) {
        if (a.avg != b.avg)
            return a.avg < b.avg;
        if (a.compstart != b.compstart)
            return a.compstart < b.compstart;
        
        return a.pos < b.pos;
    }
    
};

#endif /* orderHelper_hpp */
