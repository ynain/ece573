#include "asts.h"

struct dataflow {
    std::vector<dataflow *> preds;
    std::vector<dataflow *> succs;

    instruction * cur;

    dataflow(instruction * cur) : cur(cur) {}
};

void loadLiveness(std::vector<instruction>&);
std::vector<std::vector<instruction> > getBlocks(std::vector<instruction>);