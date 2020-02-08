#include "liveness.h"

std::vector<std::vector<instruction> > getBlocks(std::vector<instruction> code)
{
    std::vector<std::vector<instruction> > blocks;
    std::vector<instruction> curBlock;
    for (int i = 0; i < code.size(); i++)
    {
        instruction inst = code[i];
        switch(inst.type)
        {
        case JMP:
        case JEQ:
        case JNE:
        case JLT:
        case JGT:
        case JLE:
        case JGE:
            curBlock.push_back(inst);
            blocks.push_back(curBlock);
            curBlock.clear();
            break;
        case LABEL:
            blocks.push_back(curBlock);
            curBlock.clear();
            curBlock.push_back(inst);
            break;
        default:
            curBlock.push_back(inst);
            break;
        }
    }
    blocks.push_back(curBlock);
    return blocks;
}

int findLabel(std::string lbl, std::vector<instruction> code)
{
    for (int i = 0; i < code.size(); i++)
    {
        if (code[i].type == LABEL && code[i].op1 == lbl)
            return i;
    }
    return -1;
}

std::vector<dataflow *> buildDataflow(std::vector<instruction>& code)
{
    std::vector<dataflow *> worklist;
    int dest;
    for (int i = 0; i < code.size(); i++)
        worklist.push_back(new dataflow(&code[i]));

    for (int i = 0; i < worklist.size(); i++)
    {
        switch(worklist[i]->cur->type)
        {
        case JMP:
            dest = findLabel(worklist[i]->cur->op1, code);
            worklist[i]->succs.push_back(worklist[dest]);
            worklist[dest]->preds.push_back(worklist[i]);
            break;
        case JEQ:
        case JNE:
        case JLT:
        case JGT:
        case JLE:
        case JGE:
            dest = findLabel(worklist[i]->cur->result, code);
            worklist[i]->succs.push_back(worklist[dest]);
            worklist[dest]->preds.push_back(worklist[i]);
            if (i < worklist.size() - 1)
            {
                worklist[i]->succs.push_back(worklist[i + 1]);
                worklist[i + 1]->preds.push_back(worklist[i]);
            }
            break;
        default:
            if (i < worklist.size() - 1)
            {
                worklist[i]->succs.push_back(worklist[i + 1]);
                worklist[i + 1]->preds.push_back(worklist[i]);
            }
            break;
        }
    }
    return worklist;
}

bool constant(std::string v)
{
    return v[0] == '!';// isalpha(v[0]) || v[0] == '!' || v[0] == '$';
}

void computeGenKill(instruction * inst, 
                    std::set<std::string>& gen, 
                    std::set<std::string>& kill)
{

    switch(inst->type)
    {
    case POP: // pop defines
        if (constant(inst->op1)) kill.insert(inst->op1);
        break;
    case PUSH: // push uses
        if (constant(inst->op1)) gen.insert(inst->op1);
        break;
    case WRITEI: // write uses
    case WRITEF:
    case WRITES:
        if (constant(inst->op1)) gen.insert(inst->op1);
        break;
    case READI: // read defines
    case READF:
        if (constant(inst->op1)) kill.insert(inst->op1);
    case CALL: { // call potentially uses all globals
        extern std::vector<SymbolTable *> tables;
        for (int i = 0; i < tables[0]->entries.size(); i++)
            gen.insert(tables[0]->entries[i]->name);
        break;
    }
    case STOREI: // move from A to B uses A and defines B
    case STOREF:
        if (constant(inst->op1)) gen.insert(inst->op1);
        if (constant(inst->op2)) kill.insert(inst->op2);
        break;
    case ADDI: // arithmetic uses both operands and defines the result field
    case ADDF:
    case SUBI:
    case SUBF:
    case MULI:
    case MULF:
    case DIVI:
    case DIVF:
        if (constant(inst->op1)) gen.insert(inst->op1);
        if (constant(inst->op2)) gen.insert(inst->op2);
        kill.insert(inst->result);
        break;
    case JEQ: // conditional jumps use both comparison operands
    case JNE:
    case JLT:
    case JGT:
    case JLE:
    case JGE:
        if (constant(inst->op1)) gen.insert(inst->op1);
        if (constant(inst->op2)) gen.insert(inst->op2);
        break;
    }
}

void loadLiveness(std::vector<instruction>& code)
{
    // put instructions into dataflow tree
    std::vector<dataflow *> worklist = buildDataflow(code);

    // get list of global variables
    extern std::vector<SymbolTable *> tables;
    SymbolTable * global = tables[0];
    std::set<std::string> globals;
    for (int i = 0; i < global->entries.size(); i++)
        globals.insert(global->entries[i]->alt);

    // if its the last line in a function (explicit/implicit return) its liveOut is all globals
    for (int i = 0; i < worklist.size(); i++)
    {
        if (worklist[i]->succs.empty())
            worklist[i]->cur->liveOut = globals;
        
    }

    // compute until fixed-point
    while (!worklist.empty())
    {
        // pop the first element off the worklist
        dataflow * inst = worklist[0];
        worklist.erase(worklist.begin());
        std::set<std::string> oldLiveIn = inst->cur->liveIn;

        // copy all successor live-ins to live-out
        for (int i = 0; i < inst->succs.size(); i++)
        {
            inst->cur->liveOut.insert(inst->succs[i]->cur->liveIn.begin(),
                                        inst->succs[i]->cur->liveIn.end());
        }

        // copy live-out into live-in before gen/kill
        inst->cur->liveIn.insert(inst->cur->liveOut.begin(),
                                    inst->cur->liveOut.end());

        // add gen and subtract kill
        std::set<std::string> gen, kill;
        computeGenKill(inst->cur, gen, kill);

        // subtract every kill from live-in
        for (std::set<std::string>::iterator it = kill.begin(); 
                it != kill.end(); it++)
        {
            std::set<std::string>::iterator f;
            if ((f = inst->cur->liveIn.find(*it)) != inst->cur->liveIn.end())
                inst->cur->liveIn.erase(f);
        }

        // add all the gens
        inst->cur->liveIn.insert(gen.begin(), gen.end());

        // compare new live-in to old live-in
        if (oldLiveIn.size() != inst->cur->liveIn.size())
        {
            for (int p = 0; p < inst->preds.size(); p++)
                worklist.push_back(inst->preds[p]);
        }
        // element-wise comparison
        for (std::pair<
                std::set<std::string>::iterator,
                std::set<std::string>::iterator> its(
                    oldLiveIn.begin(), inst->cur->liveIn.begin());
            its.first != oldLiveIn.end() && 
                its.second != inst->cur->liveIn.end();
            its.first++, its.second++)
        {
            // if anything is different
            if ((*its.first) != (*its.second))
            {
                // add all the predecessors back into the worklist
                for (int p = 0; p < inst->preds.size(); p++)
                    worklist.push_back(inst->preds[p]);
                break;
            }
        }

    }
}