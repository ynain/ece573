#include "types.h"
#include "parser.tab.hpp"
#include <stdio.h>

extern int yyparse();
extern FILE * yyin;

int main()
{
    yyin = stdin;
    yyparse();
    fclose(yyin);

    //printf("Accepted\n");

    return 0;
}
