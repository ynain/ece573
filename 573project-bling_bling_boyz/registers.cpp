#include "registers.h"
#include <algorithm>
#include <map>


std::vector<instruction> code;
std::vector<int> permed;

int solvePerm(int x)
{
    int shift = 0;
    for (int i = 0; i < permed.size(); shift += permed[i++] < x);
    permed.push_back(x + shift);
    return x + shift;
}

extern std::string instructions[];

register_info regs[4];

int findLabel(instruction * code, int size, std::string label)
{
    for (int i = 0; i < size; i++)
    {
        if (code[i].type == LABEL && code[i].op1 == label)
            return i;
    }
    return -1;
}

void add_edge(dataflow * d1, dataflow * d2)
{
    d1->succ.push_back(d2);
    d2->pred.push_back(d1);
}

std::vector<dataflow *> build(instruction * code, int size)
{
    std::vector<dataflow *> nodes;
    for (int i = 0; i < size; i++)
        nodes.push_back(new dataflow(&code[i]));

    for (int i = 0; i < nodes.size(); i++)
    {
        int dest = -1;
        switch(nodes[i]->cur->type)
        {
        case JMP:
            dest = findLabel(code, size, nodes[i]->cur->op1);
            if (dest == -1)
            {
                std::cerr << "Could not find jump target " << nodes[i]->cur->op1 << "\n";
                exit(-1);
            }
            add_edge(nodes[i], nodes[dest]);
            break;
        case JEQ:
        case JNE:
        case JLT:
        case JGE:
        case JGT:
        case JLE:
            dest = findLabel(code, size, nodes[i]->cur->result);
            if (dest == -1)
            {
                 std::cerr << "Could not find conditional jump target " << nodes[i]->cur->result << "\n";
                 exit(-1);
            }
            add_edge(nodes[i], nodes[dest]);
            if (i < nodes.size() - 1)
                add_edge(nodes[i], nodes[i + 1]);
            break;
        default:
            if (i < nodes.size() - 1)
                add_edge(nodes[i], nodes[i + 1]);
                break;
        }
    }
    return nodes;
}

void makeWorklist(dataflow * d, std::vector<dataflow *>& worklist)
{
    if (std::find(worklist.begin(), worklist.end(), d) == worklist.end())
    {
        worklist.push_back(d);
        for (int i = 0;
            i < d->succ.size();
            makeWorklist(d->succ[i++], worklist));
    }


}


void showSet(std::set<std::string> s)
{
    // for (std::set<std::string>::iterator it = s.begin(); it != s.end(); it++)
        // std::cout << *it << ";";
    // std::cout << "\n";
}

bool isConst(std::string v)
{
    return (isalpha(v[0]) || v[0] == '!' || v[0] == '$');
}

void getGenKill(instruction * inst, std::set<std::string>& gen, std::set<std::string>& kill)
{
    extern std::vector<SymbolTable *> tables;
    switch(inst->type)
    {
    case PUSH:
        if (isConst(inst->op1)) gen.insert(inst->op1);
        break;
    case POP:
        if (isConst(inst->op1)) kill.insert(inst->op1);
        break;
    case WRITEI:
    case WRITEF:
    case WRITES:
        if (isConst(inst->op1)) gen.insert(inst->op1);
        break;
    case READI:
    case READF:
        if (isConst(inst->op1)) kill.insert(inst->op1);
        break;
    case CALL: {
        extern std::vector<SymbolTable *> tables;
        SymbolTable * glob = tables[0];
        for (int i = 0; i < glob->entries.size(); i++)
            gen.insert(glob->entries[i]->name);
        break;
    }
    case STOREI:
    case STOREF:
        if (isConst(inst->op1)) gen.insert(inst->op1);
        if (isConst(inst->op2)) kill.insert(inst->op2);
    case ADDI:
    case ADDF:
    case SUBI:
    case SUBF:
    case MULI:
    case MULF:
    case DIVI:
    case DIVF:
        kill.insert(inst->result);
        if (isConst(inst->op1)) gen.insert(inst->op1);
        if (isConst(inst->op2)) gen.insert(inst->op2);
        break;
    case JEQ:
    case JNE:
    case JLT:
    case JGT:
    case JLE:
    case JGE:
        if (isConst(inst->op1)) gen.insert(inst->op1);
        if (isConst(inst->op2)) gen.insert(inst->op2);
        break;
    }
}

void populateLiveSets(dataflow * d)
{
    std::vector<dataflow *> worklist;
    makeWorklist(d, worklist);
    extern std::vector<SymbolTable *> tables;
    SymbolTable * glob = tables[0];
    std::set<std::string> globals;
    for (int i = 0; i < glob->entries.size(); i++)
        globals.insert(glob->entries[i]->name);
    for (int i = 0; i < worklist.size(); i++)

    {
        if (worklist[i]->succ.empty())
            worklist[i]->cur->liveOut = globals;
    }

    while (!worklist.empty())
    {
        dataflow * cur = worklist[0];
        worklist.erase(worklist.begin());
        std::set<std::string> old = cur->cur->liveIn;
        for (int i = 0; i < cur->succ.size(); i++)
        {
            std::set<std::string> incoming = cur->succ[i]->cur->liveIn;
            cur->cur->liveOut.insert(incoming.begin(), incoming.end());
        }
        cur->cur->liveIn.insert(cur->cur->liveOut.begin(), cur->cur->liveOut.end());
        std::set<std::string> gen, kill;
        getGenKill((cur->cur), gen, kill);

        for (std::set<std::string>::iterator it = kill.begin(); it != kill.end(); it++)
        {
            std::set<std::string>::iterator f;
            if ((f = cur->cur->liveIn.find(*it)) != cur->cur->liveIn.end())
                cur->cur->liveIn.erase(f);
        }
        cur->cur->liveIn.insert(gen.begin(), gen.end());

        if (old.size() != cur->cur->liveIn.size())
        {
            worklist.push_back(cur);
            for (int p = 0; p < cur->pred.size(); p++)
                worklist.push_back(cur->pred[p]);
        }

        for (std::pair<std::set<std::string>::iterator, std::set<std::string>::iterator>
                            its(cur->cur->liveIn.begin(), old.begin());
                its.first != cur->cur->liveIn.end() && its.second != old.end(); its.first++, its.second++)
            {
                if ((*its.first) != (*its.second))
                {
                    // // std::cout << "live-in changed for this node\n";
                    worklist.push_back(cur);
                    for (int p = 0; p < cur->pred.size(); p++)
                        worklist.push_back(cur->pred[p]);
                    break;
                }

            }
    }
}
std::map<std::string, std::string> spilled;
int numLocs;
extern std::string instructions[];
int freeReg(int i, instruction * inst)
{
    // std::cout << "# freeing up slot " << i << "\n";
    if (regs[i].dirty)
    {
        if (regs[i].value[0] == '!')
        {
            std::string stackLoc = "$-" + std::to_string(static_cast<long long>(numLocs + spilled.size() + 1));
            // std::cout << "Temporary " << regs[i].value << " being spileld to stack @ " << stackLoc << "\n";
            spilled[regs[i].value] = stackLoc;
            regs[i].value = stackLoc;
        }
        inst->preceding.push_back(
            instruction(STOREI, "r" + std::to_string(static_cast<long long>(i)), regs[i].value));
        
    }
        
        // // std::cout << "move r" << i << " " << regs[i].value << "\n";
    regs[i].free = true;
    regs[i].dirty = false;
}

int allocate(std::string val, int locked, instruction * inst)
{
    // std::cout << "# allocating spot for " << val << "\n";
    int r;
    for (r = 0; r < 4; r++)
    {
        if (r != locked && regs[r].free)
            break;
    }
    if (r == 4)
    {
        // std::cout << "Nothing free; spilling clean...(" << locked << " is locked)\n";
        for (r = 0; r < 4; r++)
        {
            if (r != locked && !regs[r].dirty)
                break;
        }
    }
    if (r == 4)
    {   
        // std::cout << "Nothing clean; spilling the first available register\n";
        for (r = 0; r < 4; r++)
        {
            if (r != locked)
                break;
        }
    }
    freeReg(r, inst);
    // std::cout << "Allocated to reg" << r << "\n";
    regs[r].value = val;
    regs[r].free = false;
    return r;
}

int allocate(std::string val, instruction * inst)
{
    return allocate(val, -1, inst);

}

int ensure(std::string val, int locked, instruction * inst)
{
    // std::cout << "# ensuring " << val << "\n";
    for (int i = 0; i < 4; i++)

    {
        // std::cout << "# checking reg" << i << "\n"; 
        if (!regs[i].free && regs[i].value == val)
            return i;
    }
    int r = allocate(val, locked, inst);
    // // std::cout << "move " << val << " r" << r << "\n";
    if (spilled.find(val) != spilled.end())
        val = spilled[val];
    inst->preceding.push_back(
        instruction(STOREF, val, "r" + std::to_string(static_cast<long long>(r))));
    return r;
}

int ensure(std::string val, instruction * inst)
{
    return ensure(val, -1, inst);
}

void registerFuckerus(dataflow * d)
{
    long long rx, ry, rz;
    instruction * inst = d->cur;
    // std::cout << "Pre Registers: \n";
    //for (int i = 0; i < 4; i++)
        // std::cout << "r" << i << ": " << regs[i].value + (regs[i].dirty ? "*" : "") << "\n";
    // std::cout << instructions[inst->type] << " " << inst->op1 << " " << inst->op2 << " -> " << inst->result << "\n";
    // // std::cout << "Live-in =";
    // for (std::set<std::string>::iterator it = inst->liveIn.begin(); it != inst->liveIn.end(); it++)
        // // std::cout << " " << *it;
    // // std::cout << "\n";
    // // std::cout << "Live-out =";
    // for (std::set<std::string>::iterator it = inst->liveOut.begin(); it != inst->liveOut.end(); it++)
    //     // std::cout << " " << *it;
    // // std::cout << "\n";

    if (!d->pred.size())
    {
        for (int i = 0; i < 4; i++)
            regs[i].free = true;
    }
    for (int i = 0; i < d->pred.size(); i++)
    {
        instruction * prev = d->pred[i]->cur;
        if (
                prev->type == JMP ||
                prev->type == JLT ||
                prev->type == JGT ||
                prev->type == JLE ||
                prev->type == JGE ||
                prev->type == JEQ ||
                prev->type == JNE)
            {
                // std::cout << "First element in a block; freeing all registers! fuck you\n";
                // std::cout << "Predecessor is: " << instructions[prev->type] << " " << prev->op1 << " " << prev->op2 << " -> " << prev->result << "\n";

                for (int j = 0; j < 4; j++)
                {
                    regs[j].free = true;
                    regs[j].value = "";
                    regs[j].dirty = false;
                }
                    
                break;
            }
    }

    // inst->preceding.push_back(*inst);
    switch(inst->type)
    {
    case ADDI:
    case ADDF:
    case SUBI:
    case SUBF:
    case MULI:
    case MULF:
    case DIVI:
    case DIVF:
        rx = ensure(inst->op1, inst);
        ry = ensure(inst->op2, rx, inst);
        // std::cout << "rx is " << rx << "; ry is " << ry << "\n";
        if (d->cur->liveOut.find(inst->op1) == d->cur->liveOut.end())
            freeReg(rx, inst);

        if (d->cur->liveOut.find(inst->op2) == d->cur->liveOut.end())
            freeReg(ry, inst);

        rz = allocate(inst->result, ry, inst);
        // std::cout << "rz is " << rz << "\n";
        regs[rz].dirty = true;
        inst->op1 = "r" + std::to_string(rx);
        inst->op2 = "r" + std::to_string(ry);
        inst->result = "r" + std::to_string(rz);
        break;
    case READI:
    case READF:
        rz = allocate(inst->op1, inst);
        regs[rz].dirty = true;
        inst->op1 = "r" + std::to_string(rz);
        break;
    case WRITEI:
    case WRITEF:
        rx = ensure(inst->op1, inst);
        inst->op1 = "r" + std::to_string(rx);
        break;
    case STOREI:
    case STOREF:
    case RETI:
    case RETF:
        rx = ensure(inst->op1, inst);
        rz = allocate(inst->op2, rx, inst);
        // std::cout << "rx is " << rx << "; rz is " << rz << "\n";

        inst->op1 = "r" + std::to_string(rx);
        
        if (inst->type == RETI || inst->type == RETF)
        {
            for (int i = 0; i < 4; i++) 
                freeReg(i, inst);
        }
        else
        {
            inst->op2 = "r" + std::to_string(rz);
            regs[rz].dirty = true;
        }
            
        break;
    case JEQ:
    case JNE:
    case JLE:
    case JGE:
    case JLT:
    case JGT:
        rx = ensure(inst->op1, inst);
        ry = ensure(inst->op2, rx, inst);
        // std::cout << "rx is " << rx << "; ry is " << ry << "\n";

        inst->op1 = "r" + std::to_string(rx);
        inst->op2 = "r" + std::to_string(ry);
    case JMP:
        for (int i = 0; i < 4; i++) freeReg(i, inst);

        break;

    case PUSH:
        rx = ensure(inst->op1, inst);
        inst->op1 = "r" + std::to_string(rx);
        break;

    case POP:
        rz = allocate(inst->op1, inst);
        inst->op2 = "r" + std::to_string(rz);
        break;

    case CALL:
        rz = allocate(inst->result, inst);
        inst->result = "r" + std::to_string(rz);
        break;

    }

    for (int n = 0; n < d->succ.size(); n++)
    {
        for (int p = 0; p < d->succ[n]->pred.size(); p++)
        {
            instruction * prev = d->succ[n]->pred[p]->cur;
            if (
                prev->type == JMP ||
                prev->type == JLT ||
                prev->type == JGT ||
                prev->type == JLE ||
                prev->type == JGE ||
                prev->type == JEQ ||
                prev->type == JNE)
            {
                // std::cout << "Last element in a block; die immediately bitch!\n";
                for (int j = 0; j < 4; j++)
                    freeReg(j, inst);
                
                if ((inst->type == STOREI || inst->type == STOREF) && inst->preceding.size())
                {
                    instruction last = inst->preceding[inst->preceding.size() - 1];
                    inst->preceding.insert(inst->preceding.begin(), *inst);

                    // std::cout << "Cycling...\n";
                    // std::cout << "Last is " << instructions[last.type] << " " << last.op1 << " " << last.op2 << " " << last.result << "\n";
                    
                    inst->type = last.type;
                    inst->op1 = last.op1;
                    inst->op2 = last.op2;
                    inst->result = last.result;
                }

                break;
            }
        }
    }

    // std::cout << instructions[inst->type] << " " << inst->op1 << " " << inst->op2 << " -> " << inst->result << "\n";

    // std::cout << "Registers: \n";
    // for (int i = 0; i < 4; i++)
        // std::cout << "r" << i << ": " << regs[i].value + (regs[i].dirty ? "*" : "") << "\n";
    // std::cout << "---------------------------------------------\n\n";

}

int registerFuckerii(std::vector<dataflow *> worklist)
{
    for (int i = 0; i < worklist.size(); i++)
        registerFuckerus(worklist[i]);
    return numLocs + spilled.size();
}

int setupTAC(instruction * tac, int size, int locs)
{
    for (int i = 0; i < 4; i++)
        regs[i].free = true;
    spilled.clear();
    std::vector<dataflow *> worklist = build(tac, size);
    numLocs = locs;
    populateLiveSets(worklist[0]);
    return registerFuckerii(worklist);
}

/*void regAllocate(std::vector<instruction> code)
{
    // dirty, free, value
    for (int i = 0; i < 4; i++) { regs[i].free = true; regs[i].dirty = false; regs[i].value = ""; }
    std::vector<std::set<std::string>> live = computeLiveSets(code);
    for (int i = 0; i < code.size(); i++)
    {
        int rx, ry, rz;
        instruction inst = code[i];

        switch(inst.type)
        {
        case ADDI:
        case ADDF:
        case SUBI:
        case SUBF:
        case MULI:
        case MULF:
        case DIVI:
        case DIVF:
            rx = ensure(inst.op1);
            ry = ensure(inst.op2);

            if (live[i + 1].find(inst.op1) == live[i + 1].end())
                freeReg(rx);

            if (live[i + 1].find(inst.op2) == live[i + 1].end())
                freeReg(ry);

            rz = allocate(inst.result);
            // std::cout << instructions[inst.type] << " r" << rx << " r" << ry << " r" << rz << "\n";
            regs[rz].dirty = true;
            break;
        case READI:
        case READF:
            rz = allocate(inst.op1);
            // std::cout << "read r" << rz << "\n";
            regs[rz].dirty = true;
            break;
        case WRITEI:
        case WRITEF:
            rx = ensure(inst.op1);
            // std::cout << "write r" << rx << "\n";
            break;
        case STOREI:
        case STOREF:
            rx = ensure(inst.op1);
            rz = allocate(inst.op2);
            // std::cout << "move r" << rx << " r" << rz << "\n";
            regs[rz].dirty = true;
            break;
        }
    }
}

void process(std::vector<instruction> program)
{
    std::vector<std::set<std::string>> liveness = computeLiveSets(program);
    for (int i = 0; i < liveness.size(); i++)
    {
        std::set<std::string> liveSet = liveness[i];
        // std::cout << "{ ";
        for(std::set<std::string>::iterator it = liveSet.begin(); it != liveSet.end(); it++)
            // std::cout << *it << " ";
        // std::cout << "}\n";
    }

    // std::cout << "===START GENERATED CODE===\n";
    regAllocate(program);
    // std::cout << "===END GENERATED CODE===\n";

}

int main()
{
    std::vector<instruction> program1 = {
//        instruction(READI, "A", ""), // READ(A)
//        instruction(READI, "B", ""), // READ(B)
        instruction(MULI, "A", "A", "C"), // C = A * A
        instruction(ADDI, "A", "B", "D"), // D = A + B
        instruction(ADDI, "C", "D", "C"), //C = C + D
        instruction(MULI, "B", "B", "E"), // E = B * B
        instruction(ADDI, "A", "C", "A"), // A = A + C
        instruction(ADDI, "A", "E", "F"), // F = A + E
        instruction(ADDI, "A", "F", "B"), // B = A + F
//        instruction(WRITEI, "B", "") // WRITE(B)
    };
    process(program1);
    std::vector<instruction> program2 = {
        instruction(ADDI, "B", "C", "A"), // A = B + C;
        instruction(ADDI, "A", "B", "C"), // C = A + B;
        instruction(ADDI, "B", "C", "T1"), // T1 = B + C;
        instruction(ADDI, "T1", "C", "T2"), // T2 = T1 + C;
        instruction(STOREI, "T2", "D"), // D = T2;
        instruction(ADDI, "A", "B", "E"), // E = A + B;
        instruction(ADDI, "E", "D", "B"), // B = E + D;
        instruction(ADDI, "C", "D", "A"), // A = C + D;
        instruction(ADDI, "A", "B", "T3"), // T3 = A + B;
        instruction(WRITEI, "T3", "")// WRITE(T3);
    };
    process(program2);
    return 0;
}*/

int _main()
{
    std::vector<instruction> program2 = {
        instruction(ADDI, "B", "3", "A"), // A = 3 + C;
        instruction(ADDI, "A", "B", "C"), // C = A + B;
        instruction(ADDI, "B", "C", "T1"), // T1 = B + C;
        instruction(ADDI, "T1", "C", "T2"), // T2 = T1 + C;
        instruction(STOREI, "T2", "D"), // D = T2;
        instruction(ADDI, "A", "B", "E"), // E = A + B;
        instruction(ADDI, "E", "D", "B"), // B = E + D;
        instruction(ADDI, "C", "D", "A"), // A = C + D;
        instruction(ADDI, "A", "B", "T3"), // T3 = A + B;
        instruction(WRITEI, "T3", "")// WRITE(T3);
    };



    // dataflow * df = build(&program2[0], program2.size());
    // populateLiveSets(df);
    // registerFuckerii(df);


}
