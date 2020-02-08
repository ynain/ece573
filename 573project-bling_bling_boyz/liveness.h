#include "asts.h"
#include <vector>
#include <set>
struct dataflow {
    instruction current;
    std::set<std::string> alive;
    std::vector<dataflow *> successors;
    std::vector<dataflow *> predecessors;
    dataflow(instruction c) : current(c) {}
};

// dataflow * getSucc(std::vector<instruction>, int);
std::vector<std::set<std::string>> computeLiveSets(std::vector<instruction>);
