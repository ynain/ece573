#include "asts.h"
#include "registers.h"

#include <algorithm>
#include <iterator>
#include <iostream>
#include <string>

Allocator::Allocator(int stackSize)
{
    for (int i = 0; i < 4; i++)
    {
        this->regs[i].free = true;
        this->regs[i].dirty = false;
    }
    this->skip = this->rx = this->ry = this->rz = -1;
    this->stack = stackSize;
}

int Allocator::ensure(std::string val)
{
    // check to see if the value is already loaded
    for (int i = 0; i < 4; i++)
    {
        if (this->regs[i].value == val)
            return i;
    }
    
    // otherwise, try to find a spot for it
    long long spot = this->allocate(val);
    // load it into the register
    instruction load;
    load.type = STOREF;
    load.op1 = this->getStackLoc(val);
    load.op2 = "r" + std::to_string(spot);
    this->result.push_back(load);

    return spot;
}

std::string Allocator::getStackLoc(std::string temp)
{
    if (this->stackLocs.find(temp) == this->stackLocs.end())
    {
        // std::cout << temp << " is not in the map.\n";
        // find a new place for it to go
        this->stackLocs[temp] = "$-" + std::to_string(++this->stack);
    }
    // std::cout << temp << " goes in " << this->stackLocs[temp] << "\n";
    return this->stackLocs[temp];
}

void Allocator::clear(long long reg)
{
    if (this->regs[reg].dirty)
    {
        // store dirty result back to memory
        instruction str;
        str.type = STOREF;
        str.op1 = "r" + std::to_string(reg);
        str.op2 = this->getStackLoc(this->regs[reg].value);
        this->result.push_back(str);
    }
    this->regs[reg].free = true;
    this->regs[reg].dirty = false;
}

/* I am so sorry for whoever has to read this code */
#define REG_ASS_FREE(val, reg) \
            this->regs[reg].value = val;\
            this->regs[reg].free = false;\
            return reg;

#define REG_ASS_CLEAR(val, reg) \
            this->clear(reg);\
            this->regs[reg].value = val;\
            return reg;
#define REG_SEARCH(cond, action) \
    for (int i = 0; i < 4; i++)\
    {\
        if (i == this->skip) continue;\
        if (cond) { action(val, i); }\
    }
int Allocator::allocate(std::string val)
{
    // check for a free register
    REG_SEARCH((this->regs[i].free), REG_ASS_FREE)
    // check for a clean register to free and then use
    REG_SEARCH((!this->regs[i].dirty), REG_ASS_CLEAR)
    // we have to spill one, try spilling a slot variable
    REG_SEARCH((this->regs[i].value[0] != '!'), REG_ASS_CLEAR)
}
#undef REG_ASS_CLEAR
#undef REG_ASS_FREE
#undef REG_SEARCH

bool isTemp(std::string op)
{
    return op[0] == '!';
}
#define REG_SKIP(val, inst) \
    this->skip = val;\
    inst;\
    this->skip = -1
void Allocator::allocate(instruction inst)
{
    this->cur = inst;
    /*std::cout << inst.tacCode() << "\n";

    for (int i = 0; i < 4; i++)
        std::cout << (this->regs[i].free ? "&(" : "(") << this->regs[i].value << (this->regs[i].dirty ? ")*" : ")") << " ";
    std::cout << "\n";*/


    switch (inst.type)
    {
    case ADDI:
    case ADDF:
    case SUBI:
    case SUBF:
    case MULI:
    case MULF:
    case DIVI:
    case DIVF:
        this->rz = this->allocate(inst.result);

        inst.result = "r" + std::to_string(this->rz);
        this->regs[this->rz].dirty = true;
    case JLE:
    case JGE:
    case JLT:
    case JGT:
    case JNE:
    case JEQ:
        if (isTemp(inst.op1))
        {
            this->rx = this->ensure(inst.op1);
            inst.op1 = "r" + std::to_string(this->rx);
            // if (inst.liveOut.find(inst.op1) == inst.liveOut.end())
            //    this->clear(this->rx);
        } else this->rx = -1;
        if (isTemp(inst.op2))
        {
            REG_SKIP(this->rx, this->ry = this->ensure(inst.op2));
            inst.op2 = "r" + std::to_string(this->ry);
            // if (inst.liveOut.find(inst.op2) == inst.liveOut.end())
            //     this->clear(this->ry);
        }
    
        break;
    case POP:
    case READI:
    case READF:
        if (isTemp(inst.op1))
        {
            this->rz = this->allocate(inst.op1);
            inst.op1 = "r" + std::to_string(this->rz);
            this->regs[this->rz].dirty = true;
        }
        break;
    case PUSH:
    case WRITEI:
    case WRITEF:
        if (isTemp(inst.op1))
        {
            this->rx = this->ensure(inst.op1);
            inst.op1 = "r" + std::to_string(this->rx);
            // if (inst.liveOut.find(inst.op1) == inst.liveOut.end())
            //     this->clear(this->rx);
        }
        
        break;
    case CALL:
        this->rz = this->allocate(inst.result);
        inst.result = "r" + std::to_string(this->rz);
        this->regs[this->rz].dirty = true;
        break;
    case STOREI:
    case STOREF:
    case RETI:
    case RETF:
        if (isTemp(inst.op1))
        {
            this->rx = this->ensure(inst.op1);
            inst.op1 = "r" + std::to_string(this->rx);
            // if (inst.liveOut.find(inst.op1) == inst.liveOut.end())
            //     this->clear(this->rx);
        }
        if (isTemp(inst.op2))
        {
            this->rz = this->allocate(inst.op2);
            inst.op2 = "r" + std::to_string(this->rz);
        }
        break;
    }
    this->result.push_back(inst);
    /*for (int i = 0; i < 4; i++)
        std::cout << (this->regs[i].free ? "&(" : "(") << this->regs[i].value << (this->regs[i].dirty ? ")*" : ")") << " ";
    std::cout << "\n";
    std::cout << ";" << inst.tacCode() << "\n\n";*/
}
#undef REG_SKIP

void Allocator::clearRegs()
{
    for (int i = 0; i < 4; this->regs[i++].free = true);
}
extern std::string instructions[];
std::vector<instruction> process(std::vector<instruction>& code, long long& stackSize)
{
    Allocator * all = new Allocator(stackSize);
    loadLiveness(code);

    /*std::cout << "---begin liveness---\n";
    for (int i = 0; i < code.size(); i++)
    {
        std::ostream_iterator<std::string> out(std::cout, " ");
        
        std::cout << "[";
        std::copy(code[i].liveIn.begin(), code[i].liveIn.end(), out);
        std::cout << "]\n";

        std::cout << code[i].tacCode() << "\n";

        std::cout << "[";
        std::copy(code[i].liveOut.begin(), code[i].liveOut.end(), out);
        std::cout << "]\n\n";
    }
    std::cout << "---end liveness---\n";*/

    std::vector<std::vector<instruction>> codeBlocks = getBlocks(code);
    std::vector<instruction> allocd;

    for (int i = 0; i < codeBlocks.size(); i++)
    {
        for (int j = 0; j < codeBlocks[i].size(); j++)
        {
            // std::cout << ";" << instructions[codeBlocks[i][j].type] << " " << codeBlocks[i][j].op1 << " " << codeBlocks[i][j].op2 << " " << codeBlocks[i][j].result << "\n";
            // std::cout << ";" << codeBlocks[i][j].tacCode() << "\n";
            all->allocate(codeBlocks[i][j]);
            // allocd.insert(allocd.end(), all->result.begin(), all->result.end());
        }
        all->clearRegs();
    }

    allocd = all->result;

    /*std::cout << "---begin allocated---\n";
    for (int i = 0; i < allocd.size(); i++)
    {
        std::cout << ";" << instructions[allocd[i].type] << " " << allocd[i].op1 << " " << allocd[i].op2 << " " << allocd[i].result << "\n";
        std::cout << allocd[i].assCode() << "\n";
    }
        
    std::cout << "---end allocated---\n";*/
    stackSize = all->stack;
    return allocd;
}
