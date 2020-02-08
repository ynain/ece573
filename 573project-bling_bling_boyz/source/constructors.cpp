#include <iostream>
#include <sstream>
#include <stdlib.h>
#include "asts.h"

/* Build semantic stack from actions  */

CallNodeAST::CallNodeAST(IdentifierAST * funcName, paramList * list)
{
    this->funcName = funcName;
    for (int i = 0; list != NULL; i++)
    {
        this->params.push_back(list->cur);
        list = list->next;
    }
}

ReadAST::ReadAST(idList * list)
{
    for (; list != NULL; list = list->next)
    {
        IdentifierAST * ast = new IdentifierAST(list->name);
        ast->type = list->type;
        this->ids.push_back(ast);
    }
}

WriteAST::WriteAST(idList * list)
{
    for (; list != NULL; list = list->next) {
        IdentifierAST * ast = new IdentifierAST(list->name);
        ast->type = list->type;
        this->ids.push_back(ast);
    }
}

ConditionAST * ConditionAST::flip()
{
    this->op =  (this->op == JEQ) ? JNE :
                (this->op == JNE) ? JEQ :
                (this->op == JLT) ? JGE :
                (this->op == JGE) ? JLT :
                (this->op == JGT) ? JLE :
                (this->op == JLE) ? JGT : 0;
    return this;
}
