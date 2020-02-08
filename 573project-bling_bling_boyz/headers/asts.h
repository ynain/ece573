#ifndef ASTS_HEADER
#define ASTS_HEADER
#include <set>
#include <string>
#include <stdio.h>
#include <iostream>
#include <vector>
#include "types.h"

extern std::vector<SymbolTable *> tables;

struct instruction {
    std::string op1;
    std::string op2;
    std::string result;
    std::string assCode();
    std::string tacCode();
    std::set<std::string> liveIn, liveOut;

    int type;
    bool real;
};

enum instr_set {
    ADDI, SUBI, MULI, DIVI,
    ADDF, SUBF, MULF, DIVF,
    STOREI, STOREF,
    LOADI, LOADF,
    READI, READF,
    WRITEI, WRITEF, WRITES,
    CMPI, CMPF,
    JMP, JLT, JGT, JNE, JEQ, JLE, JGE,
    LABEL, LITERAL,
    CALL, RETI, RETF,
    PUSH, POP, PUSHREGS, POPREGS,
    LINK, UNLINK, HALT
};

enum instructionType {
    ADDC,
    ASMBLY
};

class CodeObject {
public:
    std::string result;
    std::vector<instruction> tac;
    var_type type;
    void print();
    bool isInt;
    static void append(CodeObject *, CodeObject *);
    static CodeObject * combine(CodeObject *, CodeObject *);
    static std::string nextTemp();
    static std::string nextLabel();

};

class AST {
public:
    virtual CodeObject * generateCode() = 0;
};

class IdentifierAST : public AST {
public:
    var_type type;
    std::string id;
    IdentifierAST(std::string id) : id(id) {}
    // IdentifierAST(std::string, bool);
    CodeObject * generateCode();
};

class LiteralAST : public AST {
public:
    var_type type;
    union {
        int i_val;
        float f_val;
    };
    LiteralAST(int val) : i_val(val), type(TYPE_INT) {}
    LiteralAST(float val): f_val(val), type(TYPE_FLOAT) {}
    CodeObject * generateCode();
};

class ExpressionAST : public AST {
public:
    virtual CodeObject * generateCode() = 0;
};

struct paramList {
    ExpressionAST * cur;
    paramList * next;

    paramList() : next(NULL) {}
};

class BinaryExpressionAST : public ExpressionAST {
public:
    int op;
    ExpressionAST * lhs;
    ExpressionAST * rhs;
    BinaryExpressionAST(int op, ExpressionAST * lhs, ExpressionAST * rhs) : op(op), lhs(lhs), rhs(rhs) {}
    BinaryExpressionAST(int op, ExpressionAST * lhs) : op(op), lhs(lhs), rhs(NULL) {}
    BinaryExpressionAST(ExpressionAST * exp) : op(-1), lhs(exp), rhs(NULL) {}
    CodeObject * generateCode();
};

class CallNodeAST : public ExpressionAST {
public:
    IdentifierAST * funcName;
    std::vector<ExpressionAST *> params;
    CallNodeAST(IdentifierAST *, paramList *);
    CodeObject * generateCode();
};

class SingleNodeExpressionAST : public ExpressionAST {
public:
    union {
        IdentifierAST * id;
        CallNodeAST * call;
        LiteralAST * lit;
    };
    SingleNodeExpressionAST(IdentifierAST * id) : id(id), which(0) {}
    SingleNodeExpressionAST(CallNodeAST * call) : call(call), which(1) {}
    SingleNodeExpressionAST(LiteralAST * lit) : lit(lit), which(2) {}
    int which;
    CodeObject * generateCode();
};

class AssignmentAST : public AST {
public:
    IdentifierAST * dest;
    ExpressionAST * val;
    AssignmentAST(IdentifierAST * id, ExpressionAST * val) : dest(id), val(val) {}
    CodeObject * generateCode();
};

class StatementListAST;

class ConditionAST : public AST {
public:
    ExpressionAST * lhs;
    ExpressionAST * rhs;
    int op;
    ConditionAST * flip();
    ConditionAST(int op, ExpressionAST * lhs, ExpressionAST * rhs) : op(op), lhs(lhs), rhs(rhs) {}
    CodeObject * generateCode();
};

class ConditionalAST : public AST {
public:
    ConditionAST * cond;
    StatementListAST * yes;
    StatementListAST * no;
    ConditionalAST(ConditionAST * cond, StatementListAST * yes, StatementListAST * no) : cond(cond), yes(yes), no(no) {}
    CodeObject * generateCode();
};

class LoopAST : public AST {
public:
    virtual CodeObject * generateCode() = 0;
};

class WhileLoopAST : public LoopAST {
public:
    ConditionAST * cond;
    StatementListAST * body;
    AssignmentAST * incr;
    WhileLoopAST(ConditionAST * cond, StatementListAST * body) : cond(cond), body(body), incr(NULL) {}
    WhileLoopAST(ConditionAST * cond, StatementListAST * body, AssignmentAST * incr) :
        cond(cond), body(body), incr(incr) {}
    CodeObject * generateCode();

};

class ForLoopAST : public LoopAST {
public:
    AssignmentAST * init;
    ConditionAST * cond;
    AssignmentAST * incr;
    StatementListAST * body;
    ForLoopAST(AssignmentAST * init, ConditionAST * cond, AssignmentAST * incr, StatementListAST * body) :
        init(init), cond(cond), incr(incr), body(body) {}
    CodeObject * generateCode();
};

class ReadAST : public AST {
public:
    std::vector<IdentifierAST *> ids;
    ReadAST(idList *);
    CodeObject * generateCode();
};

class WriteAST : public AST {
public:
    std::vector<IdentifierAST *> ids;
    WriteAST(idList *);
    CodeObject * generateCode();
};

class ControlStatementAST : public AST {
public:
    ExpressionAST * expr;
    int op;
    ControlStatementAST(int op) : op(op), expr(NULL) {} // -1 is break, -2 is continue
    ControlStatementAST(ExpressionAST * expr) : op(-3), expr(expr) {} // -3 is return
    CodeObject * generateCode();
};

class StatementListAST : public AST {
public:
    union {
        ConditionalAST * if_stmt;
        WhileLoopAST * while_loop;
        ForLoopAST * for_loop;
        AssignmentAST * assign_stmt;
        ReadAST * read_stmt;
        WriteAST * write_stmt;
        ExpressionAST * return_stmt;
        ControlStatementAST * ctrl_stmt;
    };
    int which;
    StatementListAST * next;

    StatementListAST(ConditionalAST * cond) : if_stmt(cond), which(0), next(NULL) {}
    StatementListAST(WhileLoopAST * loop) : while_loop(loop), which(1), next(NULL) {}
    StatementListAST(ForLoopAST * loop) : for_loop(loop), which(2), next(NULL) {}
    StatementListAST(AssignmentAST * ass) : assign_stmt(ass), which(3), next(NULL) {}
    StatementListAST(ReadAST * read) : read_stmt(read), which(4), next(NULL) {}
    StatementListAST(WriteAST * write) : write_stmt(write), which(5), next(NULL) {}
    StatementListAST(ExpressionAST * ret) : return_stmt(ret), which(6), next(NULL) {}
    StatementListAST(ControlStatementAST * ctrl) : ctrl_stmt(ctrl), which(7), next(NULL) {}
    CodeObject * generateCode();
};

class FuncDeclAST : public AST {
public:
    int returnType;
    IdentifierAST * id;
    StatementListAST * statements;
    FuncDeclAST * next;
    SymbolTable * table;
    int args;
    std::vector<std::string> locals;
    std::vector<SymbolTable *> myScope;
    FuncDeclAST(int retType, IdentifierAST * name, StatementListAST * body, SymbolTable * st, int args) :
        returnType(retType), id(name), statements(body), table(st), args(args), next(NULL)
        {
            int a = 0;
            for (int i = st->id; i < tables.size(); i++)
            {
                SymbolTable * cur = tables[i];
                this->myScope.push_back(cur);
                // this->tables.push_back(cur);
                std::vector<Entry *> entries = cur->entries;
                for (int e = 0; e < entries.size(); e++)
                {
                    if (entries[e]->type == TYPE_STRING) continue;
                    this->locals.push_back(entries[e]->name);
                    char buf[10];

                    if (a < args)
                        sprintf(buf, "$%d", ++a + 1);
                    else
                        sprintf(buf, "$%d", (args - (++a)));

                    // std::cout << "Alt was " << entries[e]->alt << "\n";
                    entries[e]->alt = std::string(buf);
                }

            }

        }

    CodeObject * generateCode();
};

class ProgramAST : public AST {
public:
    IdentifierAST * id;
    FuncDeclAST * funcs_decl;
    ProgramAST(IdentifierAST * name, FuncDeclAST * funcs) : id(name), funcs_decl(funcs) {}
    CodeObject * generateCode();
};
#endif