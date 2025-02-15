%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../components/tokens.c"

#define yylex tokenizer

%}

DIGIT       [0-9]+
ID          [a-zA-Z_][a-zA-Z0-9_]*
ASSIGN      "="
OPERATOR    [+\-*/]
KEYWORD     "int"|"print"|"fl"|"double"|"char"|"long"
COMMENT     "#"[^\\n]*
SEMICOLON   ";"
LBRACE      "{"
RBRACE      "}"
LPAREN      "\\("
RPAREN      "\\)"
IF          "if"
ELSE        "else"
WHILE       "while"
FUNC        "func"
CALL        "call"
WHITESPACE  [ \t\n\r]+


%%

{KEYWORD}       { addToken(KEYWORD, yytext); printf("TOKEN: KEYWORD, VALUE: %s\n", yytext); }  
{IF}            { addToken(IF, yytext); printf("TOKEN: IF, VALUE: %s\n", yytext); }
{ELSE}          { addToken(ELSE, yytext); printf("TOKEN: ELSE, VALUE: %s\n", yytext); }
{WHILE}         { addToken(WHILE, yytext); printf("TOKEN: WHILE, VALUE: %s\n", yytext); }

{FUNC}          { addToken(FUNC, yytext); printf("TOKEN: FUNC, VALUE: %s\n", yytext); }
{CALL}          { addToken(CALL, yytext); printf("TOKEN: CALL, VALUE: %s\n", yytext); }

{LBRACE}        { addToken(LBRACE, yytext); printf("TOKEN: LBRACE, VALUE: %s\n", yytext); }
{RBRACE}        { addToken(RBRACE, yytext); printf("TOKEN: RBRACE, VALUE: %s\n", yytext); }
{LPAREN}        { addToken(LPAREN, yytext); printf("TOKEN: LPAREN, VALUE: %s\n", yytext); }
{RPAREN}        { addToken(RPAREN, yytext); printf("TOKEN: RPAREN, VALUE: %s\n", yytext); }

{ID}            { addToken(ID, yytext); printf("TOKEN: ID, VALUE: %s\n", yytext); }  
{DIGIT}         { addToken(NUMBER, yytext); printf("TOKEN: NUMBER, VALUE: %s\n", yytext); }  
{ASSIGN}        { addToken(ASSIGN, yytext); printf("TOKEN: ASSIGN, VALUE: %s\n", yytext); }  
{OPERATOR}      { addToken(OPERATOR, yytext); printf("TOKEN: OPERATOR, VALUE: %s\n", yytext); }  
{SEMICOLON}     { addToken(SEMICOLON, yytext); printf("TOKEN: SEMICOLON, VALUE: %s\n", yytext); }  

{COMMENT}       { printf("Skipping comment: %s\n", yytext); }  
{WHITESPACE}    { /* Ignore whitespace */ }  

.               { printf("ERROR: Unrecognized character '%s'\n", yytext); exit(0); }  

%%

int yywrap() { return 1; }
