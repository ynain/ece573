#include "liveness.h"

#include <iostream>
#include <set>

int findLabel(std::vector<instruction> code, std::string label)
{
    for(int i = 0; i < code.size(); i++)
    {
        if (code[i].type == LABEL && code[i].op1 == label)
            return i;
    }
    return -1;
}

void addEdge(dataflow * d1, dataflow * d2)
{
    d1->successors.push_back(d2);
    d2->predecessors.push_back(d1);
}

dataflow * getSucc(std::vector<instruction> code, int inst)
{
    dataflow * df = new dataflow(code[inst]);
    int dest;
    switch(code[inst].type)
    {
    case JMP:
        dest = findLabel(code, code[inst].op1);
        if (dest == -1)
        {
            std::cerr << "No matching label found for jump to " << code[inst].op1 << "\n";
            exit(1);
        }
        addEdge(df, getSucc(code, dest));
        break;
    
    case JLE:
    case JGE:
    case JEQ:
    case JNE:
    case JLT:
    case JGT:
        dest = findLabel(code, code[inst].op1);
        if (dest == -1)
        {
            std::cerr << "No matching label found for conditional jump top " << code[inst].op1 << "\n";
            exit(1);
        }
        addEdge(df, getSucc(code, dest));
        if (inst < code.size() - 1)
            addEdge(df, getSucc(code, inst + 1));
        break;
    default:
        if (inst < code.size() - 1)
            addEdge(df, getSucc(code, inst + 1));
        break;    
    }

    return df;

}

std::vector<std::set<std::string>> computeLiveSets(std::vector<instruction> code)
{
    std::vector<std::set<std::string>> liveSets;
    std::set<std::string> currentlyAlive;
    liveSets.insert(liveSets.begin(), currentlyAlive);
    for (std::vector<instruction>::reverse_iterator it = code.rbegin(); it != code.rend(); it++)
    {
        instruction inst = *it;
        std::set<std::string>::iterator f;
        switch(inst.type)
        {
        case STOREI:
        case STOREF:
            // remove the dest from live-set (define)
            if ((f = currentlyAlive.find(inst.op2)) != currentlyAlive.end()) 
                currentlyAlive.erase(f);
            // add the source to live-set (use)
            currentlyAlive.insert(inst.op1);
            break;
        case ADDI:
        case ADDF:
        case MULI:
        case MULF:
        case SUBI:
        case SUBF:
        case DIVI:
        case DIVF:
            // remove the result from live-set (define)
            if ((f = currentlyAlive.find(inst.result)) != currentlyAlive.end())
                currentlyAlive.erase(f);
            // add the two sources to the live-set (use)
            currentlyAlive.insert(inst.op1);
            currentlyAlive.insert(inst.op2);
            break;
        case READI:
        case READF:
            // read = define so remove 
            if ((f = currentlyAlive.find(inst.op1)) != currentlyAlive.end())
                currentlyAlive.erase(f);
            break;
        case WRITEI:
        case WRITEF:
            currentlyAlive.insert(inst.op1);
            break;
        default:
            break;
        }
        liveSets.insert(liveSets.begin(), currentlyAlive);
    }
    return liveSets;
}
