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
#include "cuddAdapter.hpp"
#include "Trie.hpp"


void usage(std::string name) {
    std::ostringstream ost;
    ost << name << ": Generates a random sampling of a BDD, with variable names or in numbers" << std::endl;
    ost << "Usage: " << name << " [-norep] [-names] [-v] <number of examples> <bdd file>" << std::endl;
    ost << " -norep : No replacement." << std::endl;
    ost << " -v     : verbose." << std::endl;
    std::cerr << ost.str();
}

int main(int argc, char** argv) {
    long first, last;
    first = get_cpu_time();
    if (argc < 2 || argc > 8) {
        usage(argv[0]);
        exit(-1);
    }
    cuddAdapter *adapter = new cuddAdapter();
    int verbose = 0;
    int i       = 1;
    bool names  = false;
    bool rep    = true;
    while (i < argc-2) {
        if (std::string(argv[i]) == "-v") {
            verbose = 1;
            i++;
            continue;
        }
        if (std::string(argv[i]) == "-norep") {
            rep = false;
            i++;
            continue;
        }
        if (std::string(argv[i]) == "-names") {
            names = true;
            i++;
            continue;
        }
        std::cerr << "Unknown option " << argv[i] << std::endl;
        exit(-1);
    }
    if (argc != i+2) {
        std::cerr <<  argc-i << " arguments. Two were expected" << std::endl;
        usage(argv[0]);
        exit(-1);
    }
    int num = atoi(argv[i++]);
    adapter->readBDD(argv[i]);
    compProbabilities(verbose, adapter);
    unsigned int temp = (unsigned int) get_cpu_time() % 1000000;
    srandom(temp);
    // The combination function that computes the probabilites relies on a side effect to
    // insert in a std::map which is not thread-safe, so multithreading is
    // out of the question
    if (!rep) {
        Trie t;
        while (t.size() < num)
            t.add(genRandom(adapter));
        t.print(adapter, names, std::cout);
    }
    else
        if (names) {
            for(int x = 0; x < num; x++) 
              std::cout << nameRandom(adapter) << std::endl;
         }
        else
        for(int x = 0; x < num; x++) {
            for(bool b : genRandom(adapter))
                std::cout << b << " ";
            std::cout << std::endl;
        }
    
    std::cerr << std::endl;
    std::cerr << "Total time: " << showtime(get_cpu_time() - first) << std::endl;
}
