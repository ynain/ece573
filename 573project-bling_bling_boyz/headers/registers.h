#include "liveness.h"
#include <map>

struct reg_info {
    std::string value;
    bool dirty;
    bool free;
};

class Allocator {
public:
    Allocator(int);
    std::vector<instruction> result;
    long long stack;

private:
    reg_info regs[4];
    instruction cur;
    long long rx, ry, rz;
    int skip;
    std::map<std::string, std::string> stackLocs;
    std::string getStackLoc(std::string);
public:
    void allocate(instruction);
    void clearRegs();
private:
    int ensure(std::string);
    int allocate(std::string);
    void clear(long long);
};

std::vector<instruction> process(std::vector<instruction>&, long long&);
