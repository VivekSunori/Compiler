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
WHITESPACE  [ \t\n\r]+

%%

{KEYWORD}       { addToken(KEYWORD, yytext); printf("TOKEN: KEYWORD, VALUE: %s\n", yytext); }  
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
