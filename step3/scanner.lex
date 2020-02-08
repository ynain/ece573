%{
#include<stdio.h>
#include<string>
#include "types.h"
#include "parser.tab.hpp"

/*
enum {
    NONE,
    INTLITERAL,
    FLOATLITERAL,
    STRINGLITERAL,
    KEYWORD,
    OPERATOR,
    IDENTIFIER
};*/
int lookupKeyword(char*);
int lookupOperator(char*);

%}

%option noyywrap

%%
[0-9]+ {
    return INTLITERAL;
}

[0-9]*"."[0-9]+ {
    return FLOATLITERAL;
}

\"[^\"]+\" {
    yytext[yyleng - 1] = 0;
    yylval.name = new std::string(&yytext[1]);
    return STRINGLITERAL;
}

"--"[^\n]*\n /* comment, ignore */

PROGRAM|BEGIN|END|FUNCTION|READ|WRITE|IF|ELSE|ENDIF|WHILE|ENDWHILE|RETURN|INT|VOID|STRING|FLOAT|TRUE|FALSE|FOR|ENDFOR|CONTINUE|BREAK {
    yylval.token = lookupKeyword(yytext);
    return yylval.token;
}

[-+*/=<>();,] {
    return *yytext;
}

":="|"<="|">="|"!=" {
    return lookupOperator(yytext);
}

[a-zA-Z][a-zA-Z0-9]* {
    yylval.name = new std::string(yytext);
    return IDENTIFIER;
}
[\n] { yylineno++; }
[ \t]+ ;/*whitespace*/
. {
    return -1;
}

%%

int lookupKeyword(char * keyword)
{
    return
        !strcmp(keyword, "PROGRAM") ? PROGRAM :
        !strcmp(keyword, "BEGIN") ? _BEGIN :
        !strcmp(keyword, "END") ? END :
        !strcmp(keyword, "FUNCTION") ? FUNCTION :
        !strcmp(keyword, "READ") ? READ :
        !strcmp(keyword, "WRITE") ? WRITE :
        !strcmp(keyword, "IF") ? IF :
        !strcmp(keyword, "ELSE") ? ELSE :
        !strcmp(keyword, "ENDIF") ? ENDIF :
        !strcmp(keyword, "WHILE") ? WHILE :
        !strcmp(keyword, "ENDWHILE") ? ENDWHILE :
        !strcmp(keyword, "RETURN") ? RETURN :
        !strcmp(keyword, "INT") ? INT :
        !strcmp(keyword, "VOID") ? VOID :
        !strcmp(keyword, "STRING") ? STRING :
        !strcmp(keyword, "FLOAT") ? FLOAT :
        !strcmp(keyword, "TRUE") ? TRUE :
        !strcmp(keyword, "FALSE") ? FALSE :
        !strcmp(keyword, "FOR") ? FOR :
        !strcmp(keyword, "ENDFOR") ? ENDFOR :
        !strcmp(keyword, "CONTINUE") ? CONTINUE :
        !strcmp(keyword, "BREAK") ? BREAK : -1;
}

int lookupOperator(char * op)
{
    return
        !strcmp(op, ":=") ? ASSIGN :
        !strcmp(op, "!=") ? NEQ :
        !strcmp(op, "<=") ? LEQ :
        !strcmp(op, ">=") ? GEQ : -1;
}
