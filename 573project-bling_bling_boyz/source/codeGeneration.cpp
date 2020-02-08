#include "asts.h"
#include "registers.h"
#include "loops.h"
#include<stdlib.h>
#include<sstream>
#include<iostream>

static int numTemps = 0;
static int numLabels = 0;
static int numRegisters = 0; // delet dis fam

static std::string tempPrefix = "!T";

std::vector<SymbolTable *> curScope;

void CodeObject::append(CodeObject * dest, CodeObject * next)
{
    for (int i = 0; i < next->tac.size(); i++)
        dest->tac.push_back(next->tac[i]);
}

CodeObject * CodeObject::combine(CodeObject * first, CodeObject * last)
{
    CodeObject * combined = new CodeObject();

    combined->tac.reserve(first->tac.size() + last->tac.size());
    combined->tac.insert(combined->tac.end(), first->tac.begin(), first->tac.end());
    combined->tac.insert(combined->tac.end(), last->tac.begin(), last->tac.end());

    combined->result = last->result;

    return combined;
}

std::string CodeObject::nextTemp()
{
    std::ostringstream temp;
    temp << tempPrefix << numTemps;
    numTemps++;
    return temp.str();
}

std::string CodeObject::nextLabel()
{
    std::ostringstream label;
    label << "label" << numLabels;
    numLabels++;
    return label.str();
}

/* TODO: add PUSH, POP, (maybe PUSHREGS/POPREGS) LINK, UNLINK, HALT  */
std::string instructions[] = {
    "ADDI", "SUBI", "MULI", "DIVI",
    "ADDF", "SUBF", "MULF", "DIVF",
    "STOREI", "STOREF",
    "LOADI", "LOADF",
    "READI", "READF",
    "WRITEI", "WRITEF", "WRITES",
    "CMPI", "CMPF",
    "JMP", "JLT", "JGT", "JNE", "JEQ", "JLE", "JGE",
    "LABEL", "LITERAL",
    "CALL", "RETI", "RETF",
    "PUSH", "POP", "PUSHREGS", "POPREGS",
    "LINK", "UNLINK", "HALT"
};

std::string instruction::tacCode()
{
    std::stringstream tac;
    tac << instructions[this->type] << " " << this->op1 << " " << this->op2 << " " << this->result;
    return tac.str();
}

std::string instruction::assCode()
{
    std::stringstream assembly;
    // if (this->op1[0] == '!') this->op1.replace(0, 2, "r");
    // if (this->op2[0] == '!') this->op2.replace(0, 2, "r");
    // if (this->result[0] == '!') this->result.replace(0, 2, "r");

    if (this->type == STOREI || type == STOREF)
    {
        assembly << "move " << this->op1 << " " << this->op2;
    }
    else if (this->type == ADDI || this->type == ADDF)
    {
        assembly << "move " << this->op1 << " " << this->result << "\n" << ((this->type == ADDF) ? "addr " : "addi ") << this->op2 << " " << this->result;
    }
    else if (this->type == SUBF || this->type == SUBI)
    {
        assembly << "move " << this->op1 << " " << this->result << "\n" << ((this->type == SUBF) ? "subr " : "subi ") << this->op2 << " " << this->result;
    }
    else if (this->type == MULF || this->type == MULI)
    {
        assembly << "move " << this->op1 << " " << this->result << "\n" << ((this->type == MULF) ? "mulr " : "muli ") << this->op2 << " " << this->result;
    }
    if (this->type == DIVF || this->type == DIVI)
    {
        assembly << "move " << this->op1 << " " << this->result << "\n" << ((this->type == DIVF) ? "divr " : "divi ") << this->op2 << " " << this->result;
    }
    if (this->type == READF || this->type == READI)
    {
        assembly << "sys " << ((this->type == READF) ? "readr " : "readi ") << this->op1;
    }
    if (this->type == WRITEF || this->type == WRITEI || this->type == WRITES)
    {
        assembly << "sys " << ( (this->type == WRITEF) ? "writer " :
                                (this->type == WRITEI) ? "writei " :
                                (this->type == WRITES) ? "writes " : NULL) << this->op1;
    }

    if (this->type == JMP)
        assembly << "jmp " << this->op1;
    if (this->type == JLT || this->type == JGT || this->type == JNE || this->type == JEQ || this->type == JLE || this ->type == JGE)
    {
        assembly << (this->real ? "cmpr " : "cmpi ") << this->op1 << " " << this->op2 << "\n";
        assembly << ( (this->type == JLT) ? "jlt" :
                       (this->type == JGT) ? "jgt" :
                       (this->type == JNE) ? "jne" :
                       (this->type == JEQ) ? "jeq" :
                       (this->type == JLE) ? "jle" :
                       (this->type == JGE) ? "jge" : NULL ) << " " << this->result;
    }
    if (this->type == LABEL) assembly << "label " << this->op1;
    if (this->type == CALL)
    {
        //assembly << "move " << this->op1 << " " << this->result << "\n";
        /*assembly << "push\npush r0\npush r1\npush r2\npush r3\n";
        int i = 0;
        if (this->op2 != "")
        {
            std::string allTokens = this->op2;
            std::string token;
            std::string delim = ";";
            size_t pos = 0;
            for (i = 0; (pos = allTokens.find(delim)) != std::string::npos; i++)
            {
                token = allTokens.substr(0, pos);
                assembly << "push " << token << "\n";
                allTokens.erase(0, pos + delim.length());
            }
            assembly << "push " << allTokens << "\n";
        }*/
        
        assembly << "jsr " << this->op1 << "\n";
        if (this->op1 == "func_main")
        {
            assembly << "sys halt\n";
        }
        else
        {
            int i = std::stoi(this->op2);
            for (; i > 0; i--) 
            {
                assembly << "pop\n";
            }
            assembly << "pop r3\npop r2\npop r1\npop r0\npop " << this->result << "\n";
        }
        

    }
    if (this->type == RETI || this->type == RETF)
    {
        assembly << "move " << this->op1 << " " << this->op2 << "\n";
        assembly << "unlnk\n";
        assembly << "ret\n";
    }
    if (this->type == PUSH)// How to recognize main function, add 5 push statements before main jsr
    {
        assembly << "push " << this->op1 << "\n";
    }
    if (this->type == PUSHREGS)
    {
        assembly << "push\npush r0\npush r1\npush r2\npush r3\n";
    }
    if (this->type == POP)
    {
        assembly << "pop " << this->op1 << "\n";
    }
    if (this->type == POPREGS)
    {
        assembly << "pop r3\npop r2\npop r1\npop r0\n";
    }
    if (this->type == LINK)
    {
        assembly << "link " << atoi(this->op1.c_str()) << "\n";
    }
    if (this->type == UNLINK)
    {
        assembly << "unlnk\n";
    }
    if (this->type == HALT)
    {
        assembly << "sys halt\n";
    }
    return assembly.str();
}

void CodeObject::print()
{
    std::stringstream ir;
    std::stringstream as;
    for (int i = 0; i < this->tac.size(); i++)
    {
        instruction inst = this->tac[i];
        // std::cout << ";" << inst.tacCode() << "\n";
        std::cout << inst.assCode() << "\n";
    }

    std::cout << ir.str() << "\n";
    std::cout << as.str() << "\n";
}

CodeObject * BinaryExpressionAST::generateCode()
{
    if (this->op == -1) return this->lhs->generateCode();
    else if (this->rhs == NULL) return this->lhs->generateCode();

    CodeObject * lhs = this->lhs->generateCode();
    CodeObject * rhs = this->rhs->generateCode();

    CodeObject * code = CodeObject::combine(lhs, rhs);

    instruction perform;
    perform.op1 = lhs->result;
    perform.op2 = rhs->result;

    if ((lhs->type == TYPE_INT) && (rhs->type == TYPE_INT))
    {
        perform.type = this->op;
        code->type = TYPE_INT;
    }
    else
    {
        perform.type = this->op + 4;
        code->type = TYPE_FLOAT;
    }
    perform.result = CodeObject::nextTemp();

    code->tac.push_back(perform);

    code->result = perform.result;


    return code;
}

CodeObject * CallNodeAST::generateCode()
{
    extern SymbolTable * all_funcs;
    CodeObject * code = new CodeObject();
    instruction i;
    i.type = PUSHREGS;
    code->tac.push_back(i);
    code->type = verify(this->funcName->id, all_funcs);
    if (code->type == ERROR)
    {
        std::cerr << "No function named " << this->funcName->id << " available to be called!\n";
        exit(-1);
    }

    instruction call;
    call.type = CALL;
    call.op1 = "func_" + this->funcName->id;

    std::vector<std::string> args;

    for (int i = 0; i < this->params.size(); i++)
    {   
        CodeObject * a = this->params[i]->generateCode();
        CodeObject::append(code, a);
        args.push_back(a->result);
    }

    for (int i = 0; i < args.size(); i++)
    {
        instruction pushArg;
        pushArg.type = PUSH;
        pushArg.op1 = args[i];
        code->tac.push_back(pushArg);
    }

    call.op2 = std::to_string(static_cast<long long int>(this->params.size()));
    
    call.result = CodeObject::nextTemp();
    code->result = call.result;
    code->tac.push_back(call);

    return code;
}

CodeObject * SingleNodeExpressionAST::generateCode()
{
    //TODO: Implement this
    return
            (this->which == 0) ? this->id->generateCode() :
            (this->which == 1) ? this->call->generateCode() :
            (this->which == 2) ? this->lit->generateCode () : NULL;
}

CodeObject * WhileLoopAST::generateCode()
{
    CodeObject * code = this->cond->flip()->generateCode();

    instruction startlabel;
    startlabel.type = LABEL;
    startlabel.op1 = CodeObject::nextLabel();

    code->tac.insert(code->tac.begin(), startlabel);

    instruction endlabel;
    endlabel.type = LABEL;
    endlabel.op1 = code->result;
    CodeObject * bodyCode = this->body->generateCode();
    for (int i = 0; i < bodyCode->tac.size(); i++)
    {
        if (bodyCode->tac[i].type == -1)
        {
            // replace the break with a jump to the endlabel
            bodyCode->tac[i].type = JMP;
            bodyCode->tac[i].op1 = endlabel.op1;
        }
        else if (bodyCode->tac[i].type == -2)
        {
            // replace the continue with a jump to the startlabel

            bodyCode->tac[i].type = JMP;
            bodyCode->tac[i].op1 = startlabel.op1;
        }
    }
    CodeObject::append(code, bodyCode);

    instruction loopback;
    loopback.type = JMP;
    loopback.op1 = startlabel.op1;
    code->tac.push_back(loopback);
    code->tac.push_back(endlabel);
    return code;
}

CodeObject * ControlStatementAST::generateCode()
{
    if (this->op == -3)
    {
        CodeObject * code = this->expr->generateCode();
        instruction instr;
        instr.type = (code->type == TYPE_FLOAT) ? STOREF : STOREI;
        instr.op1 = code->result;
        instr.op2 = CodeObject::nextTemp();
        code->tac.push_back(instr);

        instr.type = (instr.type == STOREF) ? RETF : RETI;
        instr.op1 = instr.op2;

        code->tac.push_back(instr);

        return code;

    }
    CodeObject * code = new CodeObject();

    instruction flow;
    flow.type = this->op;
    code->tac.push_back(flow);
    return code;
}

CodeObject * ForLoopAST::generateCode()
{
    CodeObject * code = this->cond->flip()->generateCode();

    instruction startlabel;
    startlabel.type = LABEL;
    startlabel.op1 = CodeObject::nextLabel();

    instruction looplabel;
    looplabel.type = LABEL;
    looplabel.op1 = CodeObject::nextLabel();

    code->tac.insert(code->tac.begin(), startlabel);

    instruction endlabel;
    endlabel.type = LABEL;
    endlabel.op1 = code->result;
    CodeObject * bodyCode = this->body->generateCode();
    for (int i = 0; i < bodyCode->tac.size(); i++)
    {
        if (bodyCode->tac[i].type == -1)
        {
            // replace the break with a jump to the endlabel
            bodyCode->tac[i].type = JMP;
            bodyCode->tac[i].op1 = endlabel.op1;
        }
        else if (bodyCode->tac[i].type == -2)
        {
            // replace the continue with a jump to the startlabel

            bodyCode->tac[i].type = JMP;
            bodyCode->tac[i].op1 = looplabel.op1;
        }
    }
    CodeObject::append(code, bodyCode);

    code->tac.push_back(looplabel);
    CodeObject::append(code, this->incr->generateCode());

    instruction loopback;
    loopback.type = JMP;
    loopback.op1 = startlabel.op1;
    code->tac.push_back(loopback);
    code->tac.push_back(endlabel);
    return code;
}


std::string lookupAlt(std::string id)
{
    for (int i = 0; i < curScope.size(); i++)
    {
        for (int j = 0; j < curScope[i]->entries.size(); j++)
        {
            if (curScope[i]->entries[j]->name == id)
                return curScope[i]->entries[j]->alt;
        }
    }

    return id; // if it wasn't found, it must be global (TRUST the P A R S E R)

}


CodeObject * ReadAST::generateCode()
{
    CodeObject * code = new CodeObject();
    for (int i = 0; i < this->ids.size(); i++)
    {
        IdentifierAST * id = this->ids[i];
        instruction instr;
        if (id->type == TYPE_STRING)
        {
            std::cerr << "Cannot read into a string variable!\n";
            exit(-1);
        }
        instr.type =
                    id->type == TYPE_FLOAT ? READF :
                    id->type == TYPE_INT ? READI : READF;
        instr.op1 = lookupAlt(id->id);
        code->tac.push_back(instr);
    }
    return code;
}

CodeObject * WriteAST::generateCode()
{
    CodeObject * code = new CodeObject();
    for (int i = 0; i < this->ids.size(); i++)
    {
        IdentifierAST * id = this->ids[i];
        instruction instr;
        instr.type =
                    id->type == TYPE_STRING ? WRITES :
                    id->type == TYPE_FLOAT ? WRITEF :
                    id->type == TYPE_INT ? WRITEI : WRITES;
        instr.op1 = lookupAlt(id->id);
        code->tac.push_back(instr);
    }
    return code;
}

CodeObject * IdentifierAST::generateCode()
{
    CodeObject * code = new CodeObject();
    code->type = this->type;
    code->result = lookupAlt(this->id);
    return code;
}

CodeObject * LiteralAST::generateCode()
{
    std::stringstream ss;
    CodeObject * code = new CodeObject();
    code->type = this->type;
    ss << (this->type == TYPE_INT ? this->i_val : this->f_val);
    code->result = ss.str();
    return code;
}

CodeObject * ConditionAST::generateCode()
{
    CodeObject * lhs = this->lhs->generateCode();
    CodeObject * rhs = this->rhs->generateCode();

    CodeObject * code = CodeObject::combine(lhs, rhs);

    instruction cmp;
    if (lhs->result[0] == '!' || rhs->result[0] == '!')
    {
        cmp.op1 = lhs->result;
        cmp.op2 = rhs->result;
        cmp.result = CodeObject::nextLabel();
    }
    else
    {
        instruction mov;
        mov.type = lhs->type == TYPE_FLOAT ? STOREF : STOREI;
        mov.op1 = rhs->result;
        mov.op2 = CodeObject::nextTemp();
        code->tac.push_back(mov);
        cmp.op2 = mov.op2;

        cmp.op1 = lhs->result;
        cmp.result = CodeObject::nextLabel();
    }


    cmp.type = op;
    cmp.real = (lhs->type == TYPE_FLOAT);
    code->tac.push_back(cmp);

    code->result = cmp.result;

    return code;
}

CodeObject * ConditionalAST::generateCode()
{
    CodeObject * cmp = this->cond->generateCode();
    CodeObject * yes = this->yes->generateCode();
    CodeObject * no = this->no ? this->no->generateCode() : NULL;

    CodeObject * code = no ? CodeObject::combine(cmp, no) : cmp;

    instruction lbl;
    lbl.type = LABEL;
    lbl.op1 = cmp->result;

    instruction end;
    end.type = LABEL;
    end.op1 = CodeObject::nextLabel();

    instruction jump_to_end;
    jump_to_end.type = JMP;
    jump_to_end.op1 = end.op1;

    code->tac.push_back(jump_to_end);
    code->tac.push_back(lbl);

    CodeObject::append(code, yes);
    code->tac.push_back(end);

    return code;
}

CodeObject * StatementListAST::generateCode()
{
    CodeObject * thisLine = (this->which == 0) ? this->if_stmt->generateCode() :
                            (this->which == 1) ? this->while_loop->generateCode() :
                            (this->which == 2) ? this->for_loop->generateCode() :
                            (this->which == 3) ? this->assign_stmt->generateCode() :
                            (this->which == 4) ? this->read_stmt->generateCode() :
                            (this->which == 5) ? this->write_stmt->generateCode() :
                            (this->which == 6) ? this->return_stmt->generateCode() :
                            (this->which == 7) ? this->ctrl_stmt->generateCode() : NULL;

    if (this->next == NULL) return thisLine;
    return CodeObject::combine(thisLine, this->next->generateCode());

}

CodeObject * AssignmentAST::generateCode()
{
    
    CodeObject * code = this->val->generateCode();
    instruction assign;
    assign.type = (code->type == TYPE_FLOAT) ? STOREF : STOREI;
    this->dest->id = lookupAlt(this->dest->id);
    if (code->result[0] == '!')
    {
        assign.op1 = code->result;
        assign.op2 = this->dest->id;
        code->tac.push_back(assign);
    }
    else
    {
        assign.op1 = code->result;
        assign.op2 = CodeObject::nextTemp();
        code->tac.push_back(assign);
        assign.op1 = assign.op2;
        assign.op2 = this->dest->id;
        code->tac.push_back(assign);
    }
    return code;
}

CodeObject * FuncDeclAST::generateCode()
{
    curScope = this->myScope;

    std::ostringstream label_name;
    label_name << "func_" << this->id->id;

    instruction start;
    start.type = LABEL;
    start.op1 = label_name.str();

    instruction link;
    link.type = LINK;
    char locs[10];
    long long linkSize = this->locals.size() - this->args;
    

    CodeObject * thisFunc = this->statements->generateCode();
    for (int i = 0; i < thisFunc->tac.size(); i++)
    {
        instruction cur = thisFunc->tac[i];
        if (thisFunc->tac[i].type == RETI || thisFunc->tac[i].type == RETF)
        {
            char ret[10];
            sprintf(ret, "$%d", this->args + 6);
            thisFunc->tac[i].op2 = std::string(ret);
        }
    }

    /* Now we have a vector of instructions in thisFunc->tac to process */
    thisFunc->tac = process(thisFunc->tac, linkSize);

    thisFunc->tac = loopOptimize(thisFunc->tac);
    
    sprintf(locs, "%d", linkSize);
    link.op1 = std::string(locs);
    
    thisFunc->tac.insert(thisFunc->tac.begin(), link);
    thisFunc->tac.insert(thisFunc->tac.begin(), start);

    if (this->next == NULL) return thisFunc;
    return CodeObject::combine(thisFunc, this->next->generateCode());
}

CodeObject * ProgramAST::generateCode()
{
    CodeObject * co = this->funcs_decl->generateCode();
    instruction bmain;
    bmain.type = CALL;
    bmain.op1 = "func_main";

    co->tac.insert(co->tac.begin(), bmain);

    return co;
}
