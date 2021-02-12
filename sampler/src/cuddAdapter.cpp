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

#include "cuddAdapter.hpp"
#include "cuddInt.h"
#include "cuddAlgo.hpp"

int cuddAdapter::MAXVAR = 100000;

using namespace cudd;

cuddAdapter::~cuddAdapter() {
    delete pcomp;
}
Cudd             cuddAdapter::getCudd()             { return mgr;                       }
DdNode*          cuddAdapter::getBDD()              { return theBDD.getNode();          }
DdNode*          cuddAdapter::getZero()             { return mgr.bddZero().getNode();   }
DdNode*          cuddAdapter::getOne()              { return mgr.bddOne().getNode();    }
cudd::BDD        cuddAdapter::getBddOne()           { return mgr.bddOne();              }
int              cuddAdapter::getNumComponents()    { return pcomp->getNumComponents(); }
int              cuddAdapter::getMaxComponent()     { return pcomp->getMaxLength();     }
int              cuddAdapter::getComponent(int x)   { return pcomp->getComponent(x);    }
int              cuddAdapter::getCompSize(int x)    { return pcomp->getCompSize(x);     }
int              cuddAdapter::getNumVars()          { return numVars;                   }
float            cuddAdapter::getComponentEntropy() { return pcomp->computeEntropy();   }

void cuddAdapter::checkComponents() {
    std::cerr << "Components:" << std::endl;
    for(std::pair<int, int> qq : pcomp->listComponents())
        if (qq.second > 1)
            std::cerr << "start " << std::setw(4) << qq.first << " length " << std::setw(4) << qq.second << std::endl;
    for(std::pair<int, int> lcomp : pcomp->listComponents()) {
        if (lcomp.second > 1) {
            int tnodes = 0;
            int headNodes = theBDD.getLevelNodes(lcomp.first);
            
            for(int i = lcomp.first; i < lcomp.first+lcomp.second; i++)
                tnodes += theBDD.getLevelNodes(i);
            
            //if (headNodes != 0 && headNodes != 1) {
            
            std::cerr   << "Component start with " << headNodes
                        << " nodes in component with start "
                        << std::setw(5) << lcomp.first << " length "
                        << std::setw(5) << lcomp.second << " and "
                        << std::setw(5) << showHuman(tnodes)
                        << " nodes " << std::endl;
            //std::cerr << "Component members: ";
            for(int x = lcomp.first; x < lcomp.first+lcomp.second; x++) {
                int  pos = mgr.ReadPerm(mgr.ReadInvPerm(x));
                std::cerr   << std::setw(4) <<  inVars[mgr.ReadInvPerm(x)]
                            << "(" << std::setw(4) << mgr.ReadInvPerm(x)
                            << ") pos " << pos << " ";
            }
            std::cerr << std::endl;
            for(int i = lcomp.first; i < lcomp.first+lcomp.second; i++)
                if (theBDD.getLevelNodes(i) > 0)
                    std::cerr   << std::setw(4) << i << " "
                                << std::setw(40) << inVars[mgr.ReadInvPerm(i)]
                                << "(" << mgr.ReadInvPerm(i) << "): "
                                << theBDD.getLevelNodes(i) << std::endl;
            
            
            //  exit(-1);
            //}
        }
    }
    //    std::cerr << std::endl;
    //    for(int i = 0; i < numVars; i++)
    //        if (levelNodes[i] > 0)
    //            std::cerr << i << " " << std::setw(40) << inVars[mgr.ReadInvPerm(i)] << "("
    //            << mgr.ReadInvPerm(i) << "): " << levelNodes[i] << std::endl;
}

void cuddAdapter::reorder(std::string reorderMethod) {
    reorderCounter++;
    std::cerr << "Reorder #" << reorderCounter << ": " ;
    if (reorderMap.find(reorderMethod) != reorderMap.end())
        std::cerr << reorderMethod << std::endl;
    else
        std::cerr << "CUDD_REORDER_SIFT" << std::endl;
    
    if (!withComponents) {
        Cudd_ReorderingType crt;
        crt = CUDD_REORDER_SIFT;
        if (reorderMap.find(reorderMethod) != reorderMap.end())
            crt = reorderMap.at(reorderMethod);
        
        mgr.ReduceHeap(crt, 0);
        return;
    }
        
    std::set<std::pair<int, int> > ts;
    ts = pcomp->listChangedComponents();
    //std::cout << "Reordering. There are " << ts.size()
    //<< " components that need reordering" << std::endl;
    //checkComponents();
    //levelnodes = getLevelNodes(this);
    // Initialize interaction matrix here
    mgr.AllocInteractionMatrix();
    for(std::pair<int, int> lcomp : ts) {
        if (lcomp.first+lcomp.second > numVars) {
            std::cerr   << "There is a component with start "
                        << lcomp.first << " and end "
                        << lcomp.first+lcomp.second
                        << " when there are only " << numVars
                        << " variables." << std::endl;
            throw std::logic_error("");
        }
        
        if (lcomp.second > 1) {
            int tnodes = 0;
            for(int i = lcomp.first;
                i < lcomp.first+lcomp.second;
                i++) {
                //std::cerr << "levelnodes[" << i << "]=" << levelnodes[i] << std::endl;
                //tnodes += levelnodes[i];
                tnodes += theBDD.getLevelNodes(i) - 1;
            }
            // There is a node to hold the variable of each level.
            // That's why 1 must be substracted
            //int headNodes = theBDD.getLevelNodes(lcomp.first) - 1;
            //if (headNodes != 0 && headNodes != 1) {
            //    std::cerr << "Component start with "
            //     << headNodes << " nodes in component with start "
            //    << lcomp.first << " length " << lcomp.second << " and " << tnodes
            //    << " nodes " << std::endl;
            //    exit(-1);
            //}
            
            std::cerr   << "Reordering component start "
                        << std::setw(5) << lcomp.first
                        << " size "  << std::setw(4) << lcomp.second
                        << " nodes " << std::setw(9) << showHuman(tnodes)
                        << "..." << std::flush;
            
            // Why do we need a vector of strings?
            // Because at this point we do not know
            // if there are any trivalued variables
            
            // They are still just variables
            // but the real length of the component
            // for CUDD will be computed very soon
            std::vector<std::string> compVars;
            int c = lcomp.first;
            for(int w = 0; w < lcomp.second; w++) {
                //std::cerr << c << " ReadInv "
                // << mgr.ReadInvPerm(c) << " inVars "
                //        << inVars[mgr.ReadInvPerm(c)] << std::endl;
                compVars.push_back(inVars[mgr.ReadInvPerm(c++)]);
            }
            
            reorderComponent(compVars, reorderMethod);
            std::cerr << "done ";
            int onodes = 0;
            for(int i = lcomp.first; i < lcomp.first+lcomp.second; i++)
                onodes += theBDD.getLevelNodes(i) - 1;
            std::cerr << showHuman(onodes) << std::endl;
            
            
            // This is only necessary for the first component
            pcomp->setUnchanged(mgr.ReadInvPerm(lcomp.first));
        }
        
    }
    // Delete interaction matrix here
    mgr.DeleteInteractionMatrix();
    pcomp->changeOrder(pos2var());
    pcomp->sync();
}

int  cuddAdapter::getLevelNodes(int level) const {
    return theBDD.getLevelNodes(level);
}

int cuddAdapter::getLevel(DdNode const* node) {
    if (Cudd_IsConstant(Cudd_Regular(node)))
        return (mgr.ReadSize());
    else
        return (mgr.ReadPerm(Cudd_Regular(node)->index));
}

int cuddAdapter::getLevel(DdNode* node) {
    if (Cudd_IsConstant(Cudd_Regular(node)))
        return (mgr.ReadSize());
    else
        return (mgr.ReadPerm(Cudd_Regular(node)->index));
}

void   cuddAdapter::existentialQ(const std::set<std::string>& v) {
    cudd::BDD conjunction = mgr.bddOne();
    for(std::string s : v)
        conjunction = conjunction & getVar(s);
    
    theBDD = theBDD.ExistAbstract(conjunction);
}

void cuddAdapter::destroyInternal(const std::set<std::string>& v) {
    theBDD.destroySubtables(v.size());
    numVars = numVars - v.size();
    std::vector<std::string> auxVars;
    for(int x = 0; x < numVars; x++)
        auxVars.push_back(inVars[x]);
    
    inVars.clear();
    inVars = auxVars;
}

cuddAdapter::cuddAdapter(double cacheMultiplier) :
        mgr(0,0,CUDD_UNIQUE_SLOTS,
        cacheMultiplier*CUDD_CACHE_SLOTS, 0) {
            withComponents = false;
            pcomp = new OneComponent();
            init();
            reorderCounter = 0;
}

cuddAdapter::cuddAdapter() :
    mgr(0,0,CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0) {
    withComponents = false;
    // Problematic as it is because we don't know how many variables we have
    pcomp = new OneComponent();
    init();
    reorderCounter = 0;
}

void cuddAdapter::useComponents(Components* xPcomp) {
    delete pcomp;
    pcomp = xPcomp;
}
void cuddAdapter::useComponents() {
    delete pcomp;
    pcomp = new MultiComponents();
    withComponents = true;
}

void cuddAdapter::useComponents(std::vector<int> var2pos, std::vector<int> pos2var) {
    delete pcomp;
    pcomp = new MultiComponents(var2pos, pos2var);
    withComponents = true;
}

void cuddAdapter::init() {
    numVars = 0;
    reorderMap["CUDD_REORDER_NONE"]             = CUDD_REORDER_NONE;
    reorderMap["CUDD_REORDER_SAME"]             = CUDD_REORDER_SAME;
    reorderMap["CUDD_REORDER_RANDOM"]           = CUDD_REORDER_RANDOM;
    reorderMap["CUDD_REORDER_RANDOM_PIVOT"]     = CUDD_REORDER_RANDOM_PIVOT;
    reorderMap["CUDD_REORDER_SIFT"]             = CUDD_REORDER_SIFT;
    reorderMap["CUDD_REORDER_SIFT_CONVERGE"]    = CUDD_REORDER_SIFT_CONVERGE;
    reorderMap["CUDD_REORDER_SYMM_SIFT"]        = CUDD_REORDER_SYMM_SIFT;
    reorderMap["CUDD_REORDER_SYMM_SIFT_CONV"]   = CUDD_REORDER_SYMM_SIFT_CONV;
    reorderMap["CUDD_REORDER_GROUP_SIFT"]       = CUDD_REORDER_GROUP_SIFT;
    reorderMap["CUDD_REORDER_GROUP_SIFT_CONV"]  = CUDD_REORDER_GROUP_SIFT_CONV;
    reorderMap["CUDD_REORDER_WINDOW2_CONV"]     = CUDD_REORDER_WINDOW2_CONV;
    reorderMap["CUDD_REORDER_WINDOW3_CONV"]     = CUDD_REORDER_WINDOW3_CONV;
    reorderMap["CUDD_REORDER_WINDOW4_CONV"]     = CUDD_REORDER_WINDOW4_CONV;
    reorderMap["CUDD_REORDER_ANNEALING"]        = CUDD_REORDER_ANNEALING;
    reorderMap["CUDD_REORDER_GENETIC"]          = CUDD_REORDER_GENETIC;
    reorderMap["CUDD_REORDER_EXACT"]            = CUDD_REORDER_EXACT;
    theBDD = mgr.bddOne();
    mgr.AutodynDisable();
    mgr.SetSiftMaxSwap(std::numeric_limits<int>::max());
    mgr.SetSiftMaxVar(std::numeric_limits<int>::max());
    mgr.SetMaxGrowth(std::numeric_limits<double>::max());
    //mgr.DisableGarbageCollection();
}
void    cuddAdapter::setValue(const std::string& s , synExp* e) {
    if (e == synTrue) {
        apply(new synSymbol(s));
        return;
    }
    if (e == synFalse) {
        apply(makeNot(new synSymbol(s)));
        return;
    }
}

// Supposedly, the ordering in vector ss is the correct one already.
void   cuddAdapter::reorderComponent(std::vector<std::string>& ss,
                                     std::string rmethod) {
    int  length = ss.size();
    int startPos = mgr.ReadPerm(indices[ss.front()]);
    
    //int groupStart = mgr.ReadPerm(indices[ss.front()].first);
    int groupStart = indices[ss.front()];
    int groupLength = length;
    // The first argument to Mtr_InitGroupTree is an index. Internally in cudd
    // it is converted to a position and then it is a position in tree->lower.
    // position, not an index.
    // The proof is shown in this snippet from cuddGroup.c:
    //if (tempTree) {
    //	table->tree = Mtr_InitGroupTree(0,table->size);
    //	table->tree->index = table->invperm[0];
    //}
    mgr.SetTree(Mtr_InitGroupTree(groupStart, length));
    mgr.SetSiftMaxVar(ss.size());
    mgr.SetSiftMaxSwap(std::numeric_limits<int>::max());
    Cudd_ReorderingType crt;
    crt = CUDD_REORDER_SIFT;
    
    if (reorderMap.find(rmethod) != reorderMap.end())
        crt = reorderMap.at(rmethod);
    
    mgr.ReduceHeapMinMax(startPos,
                         startPos+length-1,
                         crt,
                         0);
};

void    cuddAdapter::shuffle(const std::vector<std::string>& extOrder) {
    // If there are no variables...
    if (extOrder.empty())
        return;
    if (extOrder.size() != numVars) {
        std::ostringstream ost;
        ost << "Shuffle size " << extOrder.size() << " != "
            << numVars << " numVars" << std::endl;
        if (numVars == 0)
            ost << "Maybe you are reading a dddmp file in an"
                << "old format (without variable names)";
        for(const std::string& s : extOrder)
            std::cerr << "extOrder " << s << std::endl;
        throw std::logic_error(ost.str());
    }
    int intOrder[numVars], cont = 0;
    std::set<int> check;
    //std::cerr << std::endl << "Starting shuffle" << std::endl;
    for(const std::string& s : extOrder) {
            //std::cerr << std::setw(80) << s << " -> "
            // << indices[s].first << " cont " << cont << std::endl;
        intOrder[cont++] = indices[s];
        check.insert(indices[s]);
        }

    //std::cerr << " cont " << cont << " extOrder size "
    //          << extOrder.size() << " numVars " << numVars
    //          << " check size " << check.size() << std::endl;
    if (cont != numVars || numVars != check.size()) {
        std::cerr   << "Not enough variables for shuffling: cont "
                    << cont << " numVars " << numVars << " check size "
                    << check.size() <<  std::endl;
        exit(-1);
    }
    for(int a = 0; a < numVars; a++)
        if (check.find(a) == check.end()) {
            std::cerr << a << " not found in check set" << std::endl;
            exit(-1);
        }
    //    std::cerr << std::endl << "Calling shuffle... ";
    //    for(int x : intOrder)
    //        std::cerr << x << " ";
    //    std::cerr << std::endl;
    mgr.SetTree(Mtr_InitGroupTree(mgr.ReadInvPerm(0),
                                  numVars));
    //std::cerr << "Calling MyShuffle from cuddAdapter::shuffle(const std::vector<std::string>& extOrder)" << std::endl;
    mgr.MyShuffleHeap(intOrder);
    //std::cerr << " done" << std::flush << std::endl;
};

void    cuddAdapter::shuffle(const std::vector<int>& extOrder) {
    // If there are no variables...
    if (extOrder.empty())
        return;
    int intOrder[numVars], cont = 0;
    std::set<int> check;
    //std::cerr << std::endl << "Starting shuffle" << std::endl;
    for(const int index : extOrder) {
            intOrder[cont++] = index;
            check.insert(index);
        }
    
    if (cont != numVars || numVars != check.size()) {
        std::cerr   << "Not enough variables for shuffling: cont "
                    << cont << " numVars " << numVars << " check size "
                    << check.size() <<  std::endl;
        exit(-1);
    }
    for(int a = 0; a < numVars; a++)
        if (check.find(a) == check.end()) {
            std::cerr << a << " not found in check set" << std::endl;
            exit(-1);
        }

    mgr.SetTree(Mtr_InitGroupTree(mgr.ReadInvPerm(0),
                                  numVars));
    mgr.MyShuffleHeap(intOrder);
};

int    cuddAdapter::getNumComponents()  const { return numComponents;        }
int    cuddAdapter::posOfVar(int var)   const { return mgr.ReadPerm(var);    }
int    cuddAdapter::varAtPos(int pos)   const { return mgr.ReadInvPerm(pos); }


void  cuddAdapter::changeOrder(std::vector<int>& pos2var) {
    pcomp->changeOrder(pos2var);
}

std::vector<int> cuddAdapter::pos2var() {
    std::vector<int> res;
    for(int x = 0; x < numVars; x++)
        res.push_back(mgr.ReadInvPerm(x));
    return res;
}

std::vector<int> cuddAdapter::var2pos() {
    std::vector<int> res;
    for(int x = 0; x < numVars; x++)
        res.push_back(mgr.ReadPerm(x));
    return res;
}

void         cuddAdapter::initblocks() {
    //mgr.SetTree(Mtr_InitGroupTree(mgr.ReadInvPerm(0), numVars - 1));
    mgr.SetTree(Mtr_InitGroupTree(mgr.ReadInvPerm(0),
                                  numVars));
    return;
}

std::pair<int, int> cuddAdapter::findSmallestBlock(int pos,
                                                   int pos2) {
    int min = -1;
    std::pair<int, int> block(0,0);
    for(std::set<std::pair<int, int> >::iterator its =
                                currentBlocks.begin();
        its != currentBlocks.end(); its++)
        if (pos >= its->first && pos2 <= its->second &&
            presentBlocks.find(*its) == presentBlocks.end() &&
            its->second - its->first < min) {
            min = its->second - its->first;
            block = *its;
        }
    return block;
}




std::vector<std::string> cuddAdapter::giveOrder() {
    std::vector<std::string> res;
    for(int x = 0; x < numVars; x++)
        res.push_back(inVars.at(mgr.ReadInvPerm(x)));
                      
    return res;
}


void   cuddAdapter::namevar(std::string name,
                            int index) {
    mgr.pushVariableName(name);
}

void   cuddAdapter::newVar(const std::string& var) {
    //pcomp->newVariable();
    countVar++;
    vars[var] = mgr.bddVar();
    indices[var] = numVars;
    inVars.push_back(var);
    mgr.pushVariableName(var);
    numVars++;
    synExp::numVars(numVars);
    //std::cerr << "var " << var << " indices[var] "
    // << indices[var].first << std::endl;
}
std::string  cuddAdapter::getVarName(int x) { return inVars[x]; }
int          cuddAdapter::getVarIndex(const std::string& var) {
    return indices.at(var);
}

void cuddAdapter::newVar(const std::string& var,
                         int pos) {
    //pcomp->newVariable();
    countVar++;
    vars[var] = mgr.bddNewVarAtLevel(pos);
    indices[var] =numVars;
    inVars.push_back(var);
    mgr.pushVariableName(var);
    numVars++;
    synExp::numVars(numVars);
}

const cudd::BDD cuddAdapter::applyNonSupport(synExp* s) {
    return process(s);
}
void cuddAdapter::deleteBdd(const std::string& var) {
    storage.erase(var);
}
bool cuddAdapter::isDefined(const std::string& var) {
    return storage.find(var) != storage.end();
}
bool cuddAdapter::apply(const std::string& s, synExp * exp) {
    if (storage.find(s) != storage.end()) {
        //std::cerr << s << " found in storage" << std::endl;
        storage[s] = storage[s] & process(exp);
    }
    else {
        //std::cerr << s << " new in storage " << std::endl;
        storage[s] = process(exp);
    }
    //std::cerr   << "apply " << exp << " for " << s
    //            << " is " << storage[s].getNode()
    //            << " zero " << getZero() << std::endl;
    return     pcomp->join(exp, false);
}

bool cuddAdapter::apply(synExp* s) {
    theBDD = theBDD & process(s);
    return pcomp->join(s, false);
}
void cuddAdapter::checkOrder() {
 if (numVars == 0) return;
 int min = std::numeric_limits<int>::max();
 int max = -1;
 bool err = false;
 std::set<int> check;
 for(int i : pcomp->getOrder()) {
        check.insert(i);
        if (i < min) min = i;
        if (i > max) max = i;
    }
    if (check.size() != numVars) {
        std::ostringstream ost;
        std::cerr   << "checkOrder error, check size "
                    << check.size() << " numVars " << numVars;
        err = true;
    }
    if (min != 0 || max != numVars-1) {
        std::cerr   << "checkOrder, check size " << check.size()
                    << " numVars " << numVars;
        std::cerr   << "min is " << min << " and max is "
                    << max << std::endl;
        for(int i : pcomp->getOrder())
            std::cerr << i << " "; std::cerr << std::endl;
        err = true;
    }
    if (err) std::logic_error("unrecoverable error");
}
void cuddAdapter::syncOrder() {
    int nn = nodecount();
    if (withComponents) {
        checkOrder();
        pcomp->reorder();
        int theOrder[numVars];
        int c = 0;
        std::set<int> check;
        for(int i : pcomp->getOrder()) {
            theOrder[c++] = i;
        }
        checkOrder();
        mgr.SetTree(Mtr_InitGroupTree(mgr.ReadInvPerm(0), numVars));
        mgr.MyShuffleHeap(theOrder);
    }
    int nnow = nodecount();
    if (nnow > nn) {
        std::cerr   << "Before syncing " << nn
                    << " nodes, after syncing "
                    << nnow << std::endl;
        mgr.DebugCheck();
    }


};

cudd::BDD  cuddAdapter::getVar(const std::string& var) {
    if (vars.find(var) != vars.end())
        return vars.at(var);
    if (storage.find(var) != storage.end())
        return storage[var];
    
    throw std::logic_error("Looking for "+var+
            " not a previously created variable or bdd");
}

cudd::BDD  cuddAdapter::getStorage(const std::string& var) {
    if (storage.find(var) != storage.end())
           return storage[var];
    
    throw std::logic_error("Looking for "+var+
            " not a previously created bdd");
}

const cudd::BDD cuddAdapter::process(synExp* exp) {
    switch (exp->get_type()) {
        case synExp_Const : {
            if (exp == synTrue)   {
                return mgr.bddOne();
            }
            if (exp == synFalse) {
                return mgr.bddZero();
            }
            if (exp == synModule) {
                return mgr.bddOne();
            }
            std::cerr   << "Error in const synExp "
                        << exp << std::endl; break;
        }
        case synExp_Symbol : {
            std::string var = exp->getSymbol();
            return getVar(var);
        }
        case synExp_Compound :
        {
            switch (exp->get_op()) {
                case synNot        : {
                    return !process(exp->first()) ;
                }
                case synAnd        : {
                    return process(exp->first()) &
                                    process(exp->second());
                }
                case synOr         : {
                    return process(exp->first()) |
                                    process(exp->second());
                }
                case synImplies    : {
                    return  !process(exp->first()) |
                                    process(exp->second());
                }
                case synIfThenElse : {
                    cudd::BDD condPart = process(exp->first());
                    cudd::BDD thenPart = process(exp->second());
                    cudd::BDD elsePart = process(exp->third());
                    
                    return condPart.Ite(thenPart, elsePart);
                }
                case synEqual      : {
                    const BDD x  = process(exp->first());
                    const BDD y  = process(exp->second());
                    return ((x & y) | (!x & !y)); }
                case synXor : {
                        throw std::logic_error("synXor could not be here");
                }
            }

        }
        case synExp_String : {
            std::cerr   << "What is a string doing here? *"
                        << exp->get_string() << "* "
                        << std::endl;
            return mgr.bddZero(); }
        case synExp_XOR : {
            std::vector<DdNode*> theVector;
            // USE ONLY VAR VARIABLES or CONSTS
            for(synExp* var : exp->get_parms()) {
                if (var->isSymbol()) {
                    //std::cerr << "Var is " << var << std::endl;
                    theVector.push_back(process(var).getNode());
                    }
                else
                    if (var == synTrue)
                        theVector.push_back(mgr.bddOne().getNode());
                    else
                        if (var == synFalse)
                            theVector.push_back(mgr.bddZero().getNode());
                        else {
                            std::ostringstream ost;
                            ost << "Error, calling XOR with non-symbol argument"
                                << var << std::endl;
                            throw std::logic_error(ost.str());
                        }
            }

            return BDD(mgr, mgr.onlyOne(theVector));
        }
        case synExp_Unknown : break;
        case synExp_Comment : return mgr.bddOne();
    }
    return mgr.bddZero();
}

const int cuddAdapter::nodecount(const std::string& s) {
    return Cudd_DagSize(storage[s].getNode());
}

const int cuddAdapter::nodecount() {
    //std::cerr << "the node " << theBDD.getNode() << std::endl;
    return Cudd_DagSize(theBDD.getNode());
}

void cuddAdapter::readVarsAndComps(const std::string& filename) {
    // The variables must be created in the manager before we read the file
    // So we scan the header for the info we need and create them in case
    // they don't exist yet.
    
    if (mgr.ReadSize() == 0) {
        int nvars = 0;
        // First we read the header
        std::ifstream dump(filename);
        if (!dump.good()) {
            throw std::invalid_argument("Couldn't open file "+filename);
        }
        
        std::string word;
        std::vector<std::string> shuf;
        for(;;) {
            if (dump >> word) {
                if (word == ".nroots") {
                    dump >> nroots;
                }
                else
                if (word == ".nvars") {
                    dump >> nvars;
                }
                // Now it's not support variable names, it's
                // ALL variable names
                else if (word == ".varnames")
                    for(int x = 0; x < nvars; x++) {
                        dump >> word;
                        newVar(word);
                    }
                else if (word == ".orderedvarnames") {
                          for(int x = 0; x < nvars; x++) {
                               dump >> word;
                              shuf.push_back(word);
                          }
                    shuffle(shuf);
                }
                else if (word == ".rootnames") {
                    for(int x = 0; x < nroots; x++) {
                        dump >> word;
                        stringHeads.push_back(word);
                    }
                }
                else if (word == ".nodes")
                    break;
            }
        }
        dump.close();
    }
    if (pcomp != NULL) delete pcomp;
    pcomp = new OneComponent(numVars);
}

cudd::BDD   cuddAdapter::auxReadBDD(const std::string& filename)
{
    readVarsAndComps(filename);
    Dddmp_VarMatchType  varMatchMode = DDDMP_VAR_MATCHPERMIDS;
    FILE *fp = fopen(filename.c_str(), "r");
    int mode = DDDMP_MODE_TEXT;
    return cudd::BDD(mgr,
                     Dddmp_cuddBddLoad(mgr.getManager(),
                                       varMatchMode,
                                       NULL,
                                       NULL,
                                       NULL,
                                       mode,
                                       (char*)filename.c_str(),
                                       fp)
                     );
}
                            
void    cuddAdapter::readBDD(const std::string& id,
                             const std::string& filename) {
    storage[id] = auxReadBDD(filename);
}

void    cuddAdapter::readBDD(const std::string& filename) {
    readArrayBDD(filename);
    theBDD = cudd::BDD(mgr, functionHeads[0]);
}

void cuddAdapter::readArrayBDD(const std::string& filename) {
    functionHeads.clear();
    readVarsAndComps(filename);
    Dddmp_VarMatchType  varMatchMode = DDDMP_VAR_MATCHPERMIDS;
    FILE *fp = fopen(filename.c_str(), "r");
    int mode = DDDMP_MODE_TEXT;
    Dddmp_RootMatchType rootMatchMode = DDDMP_ROOT_MATCHLIST;
    DdNode**  roots;
    //int readRoots =
    Dddmp_cuddBddArrayLoad(mgr.getManager(),
                           rootMatchMode,
                           NULL,
                           varMatchMode,
                           NULL,
                           NULL,
                           NULL,
                           mode,
                           (char*)filename.c_str(),
                           fp,
                           &roots);
    //std::cerr << "Read " << readRoots << " roots " << std::endl;
    theBDD = mgr.bddOne();
    for(int i = 0; i < nroots; i++) {
        //std::cerr << "Hitting root " << i << " of " << nroots;
        if (roots[i] != NULL) {
            functionHeads.push_back(roots[i]);
            //std::cerr << " is OK" << std::endl;
        }
        else std::cerr << "Hitting root " << i << " of " << nroots << " is NULL" << std::endl;
    }
    
    //std::cerr << "Read " << nroots << " BDDs from "
    // << std::endl          << filename << std::endl;
}

void    cuddAdapter::saveBDD(const std::string& filename)  {
    char *pvars[inVars.size()+1];
    int  mode = DDDMP_MODE_TEXT;
    Dddmp_VarInfoType varInfoType = DDDMP_VARPERMIDS;
    char **ppvars = pvars;
    for(const std::string& s : inVars) {
        *ppvars++ = (char*)s.c_str();
    }
    *ppvars = NULL;
    //std::cout << "Writing to file " << filename << std::endl;
    FILE *fp = fopen(filename.c_str(), "w");
    // My own version of the function that writes ALL
    // the variable names, not just variables in the support set
    Dddmp_cuddBddStore(mgr.getManager(),
                       (char*)filename.c_str(), theBDD.getNode(),
                       pvars,
                       NULL,
                       mode,
                       varInfoType,
                       (char*)filename.c_str(),
                       fp);
    fclose(fp);
    return;
}

void    cuddAdapter::saveBDDArray(const std::string& filename)  {
    char *pvars[inVars.size()+1];
    int  mode = DDDMP_MODE_TEXT;
    Dddmp_VarInfoType varInfoType = DDDMP_VARPERMIDS;
    char **ppvars = pvars;
    for(const std::string& s : inVars) {
        *ppvars++ = (char*)s.c_str();
    }
    *ppvars = NULL;
    DdNode *roots[stringHeads.size()+1];
    DdNode **proots=roots;
    int c = 0;
    for(auto& h : stringHeads) {
        roots[c++] = storage[h].getNode();
    }
    roots[c] = NULL;
    char *headNames[stringHeads.size()+1];
    //headNames[0] = (char*) "root";
    for(int i = 0; i < stringHeads.size(); i++)
        headNames[i] = (char*) stringHeads[i].c_str();
    headNames[stringHeads.size()] = NULL;
    
    FILE *fp = fopen(filename.c_str(), "w");
    // My own version of the function that writes ALL the variable names,
    // not just variables in the support set
    Dddmp_cuddBddArrayStore(mgr.getManager(),
                            (char*)filename.c_str(),
                            stringHeads.size(),
                            proots,
                            headNames,
                            pvars,
                            NULL,
                            mode,
                            varInfoType,
                            (char*)filename.c_str(),
                            fp);
    fclose(fp);
    return;
}

void    cuddAdapter::saveBDD(const std::string& id,
                             const std::string& filename) {
    char *pvars[inVars.size()+1];
    int  mode = DDDMP_MODE_TEXT;
    Dddmp_VarInfoType varInfoType = DDDMP_VARPERMIDS;
    //Dddmp_VarInfoType varInfoType = DDDMP_VARIDS;
    //Dddmp_VarInfoType varInfoType = DDDMP_VARNAMES;
    char **ppvars = pvars;
    for(const std::string& s : inVars) {
        *ppvars++ = (char*)s.c_str();
    }
    *ppvars = NULL;
    std::cerr << "Writing to file " << filename << std::endl;
    FILE *fp = fopen(filename.c_str(), "w");
    Dddmp_cuddBddStore(mgr.getManager(),
                       (char*)filename.c_str(),
                       storage[id].getNode(),
                       pvars,
                       NULL,
                       mode,
                       varInfoType,
                       (char*)filename.c_str(),
                       fp);
    return;
}

bool     cuddAdapter::sameBDD(const std::string& s1,
                              const std::string& s2) {
    return storage[s1] == storage[s2];
}

const std::vector<DdNode*>& cuddAdapter::getFunctionHeads() {
    return functionHeads;
}

void cuddAdapter::addStringHead(std::string head) {
    stringHeads.push_back(head);
}

const std::vector<std::string>& cuddAdapter::getStringHeads() {
    return stringHeads;
}
