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

#ifndef TravInfo_hpp
#define TravInfo_hpp

#include <stdio.h>
#include <iostream>
#include <map>
#include <list>
#include <thread>
#include <mutex>

#include "cudd.h"


template <typename T>
class TravInfo {
    private:
    
    int contRef, contVisit;
    bool mark, usedOften;
    T* pt;
    std::mutex                            visitMutex;
    std::list<std::mutex*>                parentSems;
    std::pair<std::mutex, std::mutex>     childSem;

    public:
    
    TravInfo() {
        contRef   = 0;
        contVisit = 0;
        pt        = NULL;
        mark      = false;
        usedOften = false;
    }
    
    void        setUsedOften(){ usedOften = true;       }
    bool        getUsedOften(){ return usedOften;       }
    void        lockVisit()   { visitMutex.lock();      }
    void        unlockVisit() { visitMutex.unlock();    }

    void        lockThenSem() { childSem.first.lock();  }
    void        lockElseSem() { childSem.second.lock(); }

    void        addParentThenSem(TravInfo<T>& ti)  { parentSems.push_back(&ti.childSem.first); }
    void        addParentElseSem(TravInfo<T>& ti)  { parentSems.push_back(&ti.childSem.second); }

    void        setResMP(T t)   { pt = new T(t);
        for(std::mutex* m : parentSems)
            m->unlock();
    }
    
    void        setRes(T t)   { pt = new T(t);
                              }
    T*          getRes()      { return pt;          }
    int         getRef()      { return contRef;     }
    void        incRef()      { contRef++;          }
    int         getVisit()    { return contVisit;   }
    void        incVisit()    { contVisit++;        }
    bool        getMark()     { return mark;        }
    void        setMark()     { mark = true;        }
 
};
#endif /* TravInfo_hpp */
