%{
    #include<stdio.h>
    #include<stdlib.h>
    int yylex();
    int yywrap();
    void yyerror(const char*);
%}


%token INTLITERAL
%token FLOATLITERAL
%token STRINGLITERAL
%token KEYWORD
%token OPERATOR
%token IDENTIFIER
%token PROGRAM
%token _BEGIN
%token END
%token FUNCTION
%token READ
%token WRITE
%token IF
%token ELSE
%token ENDIF
%token WHILE
%token ENDWHILE
%token RETURN
%token INT
%token VOID
%token STRING
%token FLOAT
%token TRUE
%token FALSE
%token FOR
%token ENDFOR
%token CONTINUE
%token BREAK
%token ASSIGN
%token NEQ
%token GEQ
%token LEQ

%error-verbose
%define parse.lac full

%%

program: 
    PROGRAM id _BEGIN body END {return 0;}
    ;

id:
    IDENTIFIER
    ;

body:
    decl func_decl
    ;

decl:
    string_decl decl
    | var_decl decl
    | %empty
    ;


string_decl:
    STRING id ASSIGN str ';'
    ;

str:
    STRINGLITERAL
    ;


var_decl:
    var_type id_list ';'
    ;

var_type:
    FLOAT
    | INT
    ;

any_type:
    var_type
    | VOID
    ;

id_list:
    id id_tail
    ;

id_tail:
    ',' id id_tail
    | %empty
    ;


param_decl_list:
	param_decl param_decl_tail 
	| %empty
	;

param_decl:
	var_type id
	;

param_decl_tail:
	',' param_decl param_decl_tail 
	| %empty
	;

func_decl:
	FUNCTION 
	any_type 
	id 
	'('
	param_decl_list
	')' 
	_BEGIN 
	func_body 
	END
	;

func_body:
	decl stmt_list 
	;

stmt_list:
	stmt stmt_list 
	| %empty
	;

stmt:
	base_stmt 
	| if_stmt 
	| loop_stmt 
	;

base_stmt:
	assign_stmt 
	| read_stmt 
	| write_stmt 
	| control_stmt
	;

assign_stmt:
	assign_expr 
	;

assign_expr:
	id ASSIGN 
	expr
	;

read_stmt:
	READ '(' 
	id_list 
	')'
	;

write_stmt:
	WRITE '(' 
	id_list 
	')'
	;

return_stmt:
	RETURN expr 
	;

/* Expressions */
expr:
	expr_prefix factor
	;

expr_prefix:
	expr_prefix factor addop 
	| %empty
	;

factor:	
	factor_prefix postfix_expr
	;

factor_prefix:
	factor_prefix postfix_expr mulop 
	| %empty
	;

postfix_expr:
	primary 
	| call_expr
	;

call_expr:
	id '(' 
	expr_list 
	')'

expr_list:
	expr expr_list_tail 
	| %empty
	;

expr_list_tail:
	',' expr 
	expr_list_tail 
	| %empty
	;

primary:
	'(' 
	expr 
	')' 
	| id 
	| INTLITERAL 
	| FLOATLITERAL
	;

addop:
	'+' 
	| '-'
	;

mulop:
	'*' 
	| '/'
	;

/* Complex Statements and Condition */ 
if_stmt:
	IF '(' 
	cond 
	')' 
	decl stmt_list else_part ENDIF

else_part:
	ELSE decl stmt_list 
	| %empty
	;

cond:
	expr compop expr 
	| TRUE 
	| FALSE
	;

compop:
	'<' 
	| '>' 
	| '=' 
	| NEQ 
	| LEQ 
	| GEQ
	;

while_stmt:
	WHILE '(' 
	cond 
	')' 
	decl stmt_list ENDWHILE

control_stmt:
	return_stmt 
	| CONTINUE 
	; 
	| BREAK 
	;

loop_stmt:
	while_stmt 
	| for_stmt
	;

init_stmt:
	assign_expr 
	| %empty
	;

incr_stmt:
	assign_expr 
	| %empty
	;

for_stmt:
	FOR '(' 
	init_stmt ';' 
	cond ';' 
	incr_stmt 
	')'
	decl stmt_list ENDFOR

%%

int yywrap(void)
{
    return 1;
}

void yyerror(const char* s)
{
    printf("Not Accepted\n");
    exit(-1);
}
