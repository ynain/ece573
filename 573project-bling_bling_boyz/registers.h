#include "asts.h"

#include<vector>
#include<set>
#include<map>
#include<iostream>
#include<cstdlib>


struct dataflow {
    instruction * cur;
    std::set<std::string> liveSet;
    std::vector<dataflow *> pred;
    std::vector<dataflow *> succ;
    int index;
    dataflow(instruction * cur) : cur(cur) {}
};

struct register_info {
    bool dirty;
    bool free;
    std::string value;
};


// dataflow * build(std::vector<instruction>, int, std::map<int, dataflow *>&);
std::vector<dataflow *> build(instruction *, int);
int registerFuckerii(std::vector<dataflow *>);
void populateLiveSets(dataflow *);
int setupTAC(instruction *, int, int);

