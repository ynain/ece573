#include "registers.h"
#include "parser.tab.hpp"
#include <stdio.h>
#include <map>
extern int yyparse();
extern FILE * yyin;

void run(ProgramAST * prog)
{
    CodeObject * code = prog->generateCode();

    code->print();
}

int main(int argc, char ** argv)
{
    // printf("Argc = %d\n", argc);
    if (argc == 1)
    {
        yyin = stdin;
        // printf("Default stdin\n");
    }
    else
    {
        yyin = fopen(argv[1], "r");
        // printf("Using %s as input\n", argv[1]);
    }
    yyparse();
    fclose(yyin);

    //printf("Accepted\n");

    return 0;
}
