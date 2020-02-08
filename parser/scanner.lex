%{
#include<stdio.h>

enum {
    NONE,
    INTLITERAL,
    FLOATLITERAL,
    STRINGLITERAL,
    KEYWORD,
    OPERATOR,
    IDENTIFIER
};

%}

%option noyywrap

%%
[0-9]+ {
    printf("Token Type: INTLITERAL\nValue: %d\n", atoi(yytext));
    return INTLITERAL;
}

[0-9]*"."[0-9]+ {
    printf("Token Type: FLOATLITERAL\nValue: %s\n", yytext);
    return FLOATLITERAL;
}

\"[^\"]+\" {
    printf("Token Type: STRINGLITERAL\nValue: %s\n", yytext);
    return STRINGLITERAL;
}

"--"[^\n]*\n /* comment, ignore */

PROGRAM|BEGIN|END|FUNCTION|READ|WRITE|IF|ELSE|ENDIF|WHILE|ENDWHILE|RETURN|INT|VOID|STRING|FLOAT|TRUE|FALSE|FOR|ENDFOR|CONTINUE|BREAK {
    printf("Token Type: KEYWORD\nValue: %s\n", yytext);
    return KEYWORD;
}

":="|"+"|"-"|"*"|"/"|"="|"!="|"<"|">"|"("|")"|";"|","|"<="|">=" {
    printf("Token Type: OPERATOR\nValue: %s\n", yytext);
    return OPERATOR;
}

[a-zA-Z][a-zA-Z0-9]* {
    printf("Token Type: IDENTIFER\nValue: %s\n", yytext);
    return IDENTIFIER;
}

[ \t\n]+ return -1;/*whitespace*/
. {
    printf("Unrecognized char\n");
    return -1;
}

%%

/*int carrot(int argc, char** argv)
{
    yyin = stdin;
    while(1)
        printf("Return val: %d\n", yylex());

    return 0;
}*/
