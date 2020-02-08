/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    INTLITERAL = 258,
    FLOATLITERAL = 259,
    STRINGLITERAL = 260,
    KEYWORD = 261,
    OPERATOR = 262,
    IDENTIFIER = 263,
    PROGRAM = 264,
    _BEGIN = 265,
    END = 266,
    FUNCTION = 267,
    READ = 268,
    WRITE = 269,
    IF = 270,
    ELSE = 271,
    ENDIF = 272,
    WHILE = 273,
    ENDWHILE = 274,
    RETURN = 275,
    INT = 276,
    VOID = 277,
    STRING = 278,
    FLOAT = 279,
    TRUE = 280,
    FALSE = 281,
    FOR = 282,
    ENDFOR = 283,
    CONTINUE = 284,
    BREAK = 285,
    ASSIGN = 286,
    NEQ = 287,
    GEQ = 288,
    LEQ = 289
  };
#endif
/* Tokens.  */
#define INTLITERAL 258
#define FLOATLITERAL 259
#define STRINGLITERAL 260
#define KEYWORD 261
#define OPERATOR 262
#define IDENTIFIER 263
#define PROGRAM 264
#define _BEGIN 265
#define END 266
#define FUNCTION 267
#define READ 268
#define WRITE 269
#define IF 270
#define ELSE 271
#define ENDIF 272
#define WHILE 273
#define ENDWHILE 274
#define RETURN 275
#define INT 276
#define VOID 277
#define STRING 278
#define FLOAT 279
#define TRUE 280
#define FALSE 281
#define FOR 282
#define ENDFOR 283
#define CONTINUE 284
#define BREAK 285
#define ASSIGN 286
#define NEQ 287
#define GEQ 288
#define LEQ 289

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
