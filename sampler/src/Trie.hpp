#include <vector>
#include <map>
#include <ostream>
#include "cuddAdapter.hpp"

class Trie;
typedef std::map<bool, Trie*> tTrie;

class Trie {
public:
         Trie();
    int  size();
    void add(std::vector<bool> v);
    void print(cuddAdapter* a, bool names, std::ostream& ost);
private:
    void traverse(cuddAdapter* a,
                  bool names,
                  int i,
                  tTrie* p,
                  std::ostream& ost);
    std::vector<bool> b;
    int len;
    unsigned int _size;
    tTrie *t;
};
