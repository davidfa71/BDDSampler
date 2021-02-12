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

#ifndef cuddAlgo_hpp
#define cuddAlgo_hpp

#include <stdio.h>
#include <sstream>
#include <gmpxx.h>
#include <gmp.h>
#include <set>
#include "cuddAdapter.hpp"
#include "Traverser.hpp"

extern int verbose;

void                              compProbabilities(int verbose,
                                                    cuddAdapter* a);
void                              compProbabilitiesMP(int verbose,
                                                      int threads,
                                                      cuddAdapter* a,
                                                      bool fast);

std::vector<bool>                 genRandom(cuddAdapter* a);
std::string                       nameRandom(cuddAdapter *a);
std::string                       nameProduct(cuddAdapter* a, std::vector<bool> v);
#endif /* cuddAlgo_hpp */

