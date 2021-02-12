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

#ifndef __myKconf__cuddAdapter__
#define __myKconf__cuddAdapter__

#include "OneComponent.hpp"
#include "MultiComponents.hpp"
#include "humanNums.hpp"
#include "mytime.hpp"
#include "humanNums.hpp"

#include <fstream>
#include <mtr.h>
#include <limits>
#include <cuddObj.hh>
#include <cudd.h>
#include <dddmp.h>
#include <iomanip>
#include <list>
#include <map>
#include <set>

using namespace cudd;


class cuddAdapter {
    public :
    
                                cuddAdapter();
                                cuddAdapter(double cacheMultiplier);
                                ~cuddAdapter();

    void                        useComponents();
    void                        useComponents(Components* pcomp);
    void                        useComponents(std::vector<int> var2pos,
                                              std::vector<int> pos2var);
    void                        init();
    int                         getNumComponents();
    int                         getMaxComponent();
    float                       getComponentEntropy();
    int                         getComponent(int x);
    int                         getCompSize(int x) ;
    int                         getNumVars();
    std::vector<int>            pos2var();
    std::vector<int>            var2pos();
    void                        changeOrder(std::vector<int>& pos2var);
    DdNode*                     getBDD();
    DdNode*                     getZero();
    DdNode*                     getOne();
    cudd::BDD                   getBddOne();

    Cudd                        getCudd();
    int                         getLevel(DdNode const * node);
    int                         getLevel(DdNode* node);
    int                         varAtPos(int pos) const;
    int                         posOfVar(int var) const;
    void                        destroyQuantified(int n);
    void                        setValue(const std::string&, synExp*);
    void                        reorderComponent(std::vector<std::string>& ss,
                                                 std::string rmethod);
    void                        printBDD();
    void                        reorder(std::string reorderMethod);
    int                         addblock(std::string x);
    void                        initblocks();
    std::vector<std::string>    giveOrder();
    void                        namevar(std::string name, int index);
    void                        newVar(const std::string& var);
    void                        newVar(const std::string& var, int pos);
    const cudd::BDD             applyNonSupport(synExp* s);
    bool                        apply(synExp* s);
    bool                        apply(const std::string& s, synExp * exp);
    const   int                 nodecount();
    const   int                 nodecount(const std::string& s);
    void                        saveBDD(const std::string& filename);
    void                        saveBDDArray(const std::string& filename);
    void                        saveBDD(const std::string& id,
                                        const std::string& b);
    void                        readBDD(const std::string& b);
    void                        readBDD(const std::string& id,
                                        const std::string& b);
    void                        readArrayBDD(const std::string& filename);
    bool                        sameBDD(const std::string& s1,
                                        const std::string& s2);
    static  int MAXVAR;
    Cudd    mgr;
    const   cudd::BDD           process(synExp* exp);
    int                         getLevelNodes(int level) const;
    int                         getNumComponents()       const;
    void                        shuffle(const std::vector<int>& order);
    void                        shuffle(const std::vector<std::string>& order);
    void                        existentialQ(const std::set<std::string>& v);
    void                        destroyInternal(const std::set<std::string>& v);
    std::string                 getVarName(int x);
    int                         getVarIndex(const std::string& var);
    void                        reorderComponents(int nreorder);
    void                        checkComponents();
    void 			            checkOrder();
    void                        syncOrder();
    void                        setXORBlocks(synExp* s);
    cudd::BDD                   getVar(const std::string& var);
    cudd::BDD                   getStorage(const std::string& var);
    void                        deleteBdd(const std::string& var);
    bool                        isDefined(const std::string& var);
    std::map<std::string, Cudd_ReorderingType> reorderMap;
    const std::vector<DdNode*>& getFunctionHeads();
    void                        addStringHead(std::string head);
    const std::vector<std::string>& getStringHeads();
   
private:
    
    void                                auxXOR(synExp* exp);
    void                                createXORBlock(std::vector<std::string>& vec);
    void                                internalRefs();
    std::map<std::string, cudd::BDD>    vars;
    std::map<std::string, int>          indices;
    std::vector<std::string>            inVars;
    std::vector<bool>                   startComponent;
    int                                 numComponents;
    bool                                withComponents;
    std::pair<int, int>                 findSmallestBlock(int pos, int pos2);
    int                                 numVars;
    std::set<std::pair<int, int> >      currentBlocks, presentBlocks;
    std::map<std::string, cudd::BDD>    storage;
    int                                 reorderCounter;
    void                                readVarsAndComps(const std::string& filename);
    cudd::BDD                           auxReadBDD(const std::string& filename);
    int                                 nroots;
    Components*                         pcomp;
    std::vector<int>                    levelnodes;
    int                                 countVar = 0;
    cudd::BDD                           theBDD;
    std::vector<DdNode*>                functionHeads;
    std::vector<std::string>            stringHeads;
};

#endif /* defined(__myKconf__cuddAdapter__) */
