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

#include "cuddAlgo.hpp"
//#include <random>
int verbose = 0;
// For each node, the probability to reach the true node choosing the then child
std::unordered_map<DdNode*, unsigned long>                                 probabilities;




void getProducts(int plevel, int tlevel, int elevel, const mpz_class& tr, const mpz_class& er, mpz_class& thenPart, mpz_class& elsePart) {
    // thenPart = 2 ^ (tlevel-plevel-1) * tr
    mpz_ui_pow_ui(thenPart.get_mpz_t(), 2, tlevel - plevel -1);
    thenPart *= tr;
    
    // elsePart = 2 ^(elevel - plevel -1) * er
    mpz_ui_pow_ui(elsePart.get_mpz_t(), 2, elevel - plevel -1);
    elsePart *= er;
}

mpz_class _probabilities(int plevel, int tlevel, int elevel, const mpz_class& tr, const mpz_class& er, DdNode* node) {
    mpz_class thenPart, elsePart;
    getProducts(plevel, tlevel, elevel, tr, er, thenPart, elsePart);

    // If we set probability to 1, comparison with a
    // random number equal to one will fail as in
    // if random < probability then choose the high child
    // This is important for generating random samples
    if (elsePart == 0)
        probabilities[node] = (RAND_MAX>>1)+1;
    else {
        mpz_class x = (RAND_MAX>>1)*thenPart / (thenPart + elsePart);
        probabilities[node] = x.get_ui();

    }
    return (thenPart + elsePart);
}

void compProbabilitiesMP(int verbose, int threads, cuddAdapter* a, bool fast) {
    unsigned int temp = (unsigned int) get_cpu_time() % 1000000;
    //cout << "Initializing random seed " << temp << endl;
    srandom(temp);
    traverseMP(verbose, threads, a, mpz_class(0), mpz_class(1), &_probabilities, fast);
}

void compProbabilities(int verbose, cuddAdapter* a) {
    unsigned int temp = (unsigned int) get_cpu_time() % 1000000;
    //cout << "Initializing random seed " << temp << endl;
    srandom(temp);
    traverse(verbose, a, mpz_class(0), mpz_class(1), &_probabilities);
}

bool fiftyfifty() {
    //float randNum = (float)random()/(float)RAND_MAX;
    //return (randNum < 0.5);
    return (random() & 1) == 0;
}

std::string nameProduct(cuddAdapter*a, std::vector<bool> v) {
    std::ostringstream ost;
    int var = 0;
    for(bool b : v) {
        std::string nameVar = a->getVarName(var++);
        if (b)
            ost <<  "    " << nameVar << " ";
        else
            ost << "not " << nameVar << " ";
    }
    return ost.str();
}

std::string nameRandom(cuddAdapter *a) {
    std::ostringstream ost;
    return nameProduct(a, genRandom(a));
}


std::vector<bool> genRandom(cuddAdapter* a) {
    // We assume the probabilities have been computed already
    DdNode *g1, *g0;
    int index;
    std::vector<bool> exemplar(a->getNumVars());
    DdNode *one  = a->getOne();
    DdNode *zero = a->getZero();
    // For
    DdNode *trav = a->getBDD();

    int    pos   = 0;
    if (trav == zero)
        return exemplar;
    
    index = a->getLevel(trav);
    for(int i = 0; i< index; i++)
        exemplar[a->varAtPos(pos++)] = fiftyfifty();
    
    while (trav != one) {
        cuddGetBranches(trav, &g1, &g0);
        if (random()>>1 < probabilities[trav]) {
            trav = g1;
            exemplar[a->varAtPos(pos++)] = true;
        }
        else {
            exemplar[a->varAtPos(pos++)] = false;
            trav = g0;
        }
        for(int i = index+1; i < a->getLevel(trav); i++)
            exemplar[a->varAtPos(pos++)] = fiftyfifty();
        
        index = a->getLevel(trav);
    }
    return exemplar;
}









