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

#ifndef OneComponent_hpp
#define OneComponent_hpp

#include <stdio.h>
#include <set>

#include "Components.hpp"

class OneComponent : public Components {
    public :
    
    OneComponent();
    OneComponent(std::vector<int>& var2pos, std::vector<int>& pos2var);
    OneComponent(int num);
    std::vector<int> getOrder();

    void    changeOrder(std::vector<int>  x);
    void    newVariable();
    void    replayConstraints();
    int     getComponent(int x) ;
    int     getStart(int x);
    int     getCompSize(int x);
    int     getNumComponents();
    int     getMaxLength();
    int     getMaxReorder(int x);
    bool    join(synExp* s, bool changeOrder);
    void    reorder();
    bool    isOutOfSync();
    void    sync();
    int     find(int x);
    void    showInfo(int x);
    void    setUnchanged(int x);                            
    std::set<std::pair<int, int> >              listComponents();
    std::set<std::pair<int, int> >       listChangedComponents();
    void                                        checkLengths();

    void                                    printComponents();

    float   computeEntropy();
    bool    makeUnion(int x, int y, bool changeOrder = true);

    protected :
    std::vector<int> pos2var;
    int num;
};


#endif /* OneComponent_hpp */
