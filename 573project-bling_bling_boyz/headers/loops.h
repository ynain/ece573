
#include <vector>
#include <set>

// Control flow graph
class CFG {
public:
    std::vector<std::vector<instruction>> nodes;
    std::vector<std::vector<int>> edges;
    int enter;
    int exit;
    CFG(std::vector<std::vector<instruction>> nodes, std::vector<std::vector<int>> edges, int enter, int exit) : 
            nodes(nodes), edges(edges), enter(enter), exit(exit) {}
};

std::vector<instruction> loopOptimize(std::vector<instruction>&);
