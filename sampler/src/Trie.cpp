#include "Trie.hpp"

Trie::Trie()     { t = new tTrie; _size = 0; }
int Trie::size() { return _size;   }
void Trie::add(std::vector<bool> v) {
    len = v.size();
    tTrie* p = t;
    bool found = true;
    for(auto b : v) {
        if (p->find(b) != p->end()) {
            p = (*p)[b]->t;
            
        }
        else {
            found = false;
            (*p)[b] = new Trie();
            p = (*p)[b]->t;
        }
    }
    if (!found) _size++;
  }

void Trie::print(cuddAdapter* a,
                 bool names,
                 std::ostream& ost) {
    b.resize(len);
    tTrie *p = t;
    traverse(a, names, 0, p, ost);
}

void Trie::traverse(cuddAdapter* a,
                    bool names,
                    int i,
                    tTrie* p,
                    std::ostream& ost) {
    if (p->find(false) != p->end()) {
        b[i] = false;
        traverse(a, names, i+1, (*p)[false]->t, ost);
    }
    if (p->find(true) != p->end()) {
        b[i] = true;
        traverse(a, names, i+1, (*p)[true]->t, ost);
    }
    if (p->find(false) == p->end() &&
        p->find(true)  == p->end()) {
        
        if (names) {
            for(int q = 0; q < b.size(); q++)
                if (b[q])
                    ost << a->getVarName(q) << " ";
                else
                    ost << " not " << a->getVarName(q) << " ";
            ost << std::endl;
        }
        else {
            for(int q = 0; q < b.size(); q++)
                ost << b[q] << " ";
            ost << std::endl;
        }
    }
}
