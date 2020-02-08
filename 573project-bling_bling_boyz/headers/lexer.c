#include<stdio.h>
extern int yylex();
extern FILE * yyin;
int main(int argc, char** argv)
{
    if (argc > 1)
        yyin = fopen(argv[1], "r");
    else
        yyin = stdin;


    int token;
    while((token = yylex()));

    fclose(yyin);

    return 0;
}
