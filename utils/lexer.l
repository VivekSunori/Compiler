%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../components/tokens.c"

#define yylex tokenizer

int inVarDeclaration = 0;

%}

DIGIT       [0-9]+
ID          [a-zA-Z_][a-zA-Z0-9_]* // Identifiers (variables, function names)
ASSIGN      "="
OPERATOR    [+\-*/]
KEYWORD     "num"|"log"|"str" // Recognized data types
COMMENT     "#"[^\\n]* // Single-line comment
SEMICOLON   ";"
LBRACE      "{"
RBRACE      "}"
LPAREN      "\("
RPAREN      "\)"
IF          "if"
ELSE        "else"
WHILE       "while"
FUNC        "func"
CALL        "call"
COMMA       ","
WHITESPACE  [ \t\n\r]+
FOR         "for"
DO          "do"
AND         "&&"
OR          "||"
RELOP       "=="|"!="|"<"|">"|"<="|">="

%%

{KEYWORD} { 
    addToken(KEYWORD, yytext);
    printf("TOKEN: KEYWORD, VALUE: %s\n", yytext);
    inVarDeclaration = 1;
}

{ID} { 
    if (strlen(yytext) > MAX_VAR_LENGTH) {
        printf("ERROR: Variable name '%s' exceeds 50 characters!\n", yytext);
        exit(1);  
    }

    if (inVarDeclaration) {
        addToken(VAR, yytext);
        printf("TOKEN: VAR, VALUE: %s\n", yytext);
        inVarDeclaration = 0;
    } else {
        addToken(ID, yytext);
        printf("TOKEN: ID, VALUE: %s\n", yytext);
    }
}

{DIGIT}         { addToken(NUMBER, yytext); printf("TOKEN: NUMBER, VALUE: %s\n", yytext); }  
{ASSIGN}        { addToken(ASSIGN, yytext); printf("TOKEN: ASSIGN, VALUE: %s\n", yytext); }  
{OPERATOR}      { addToken(OPERATOR, yytext); printf("TOKEN: OPERATOR, VALUE: %s\n", yytext); }  
{SEMICOLON}     { addToken(SEMICOLON, yytext); printf("TOKEN: SEMICOLON, VALUE: %s\n", yytext); }  
{COMMA}         { addToken(COMMA, yytext); printf("TOKEN: COMMA, VALUE: %s\n", yytext); }  

{IF}            { addToken(IF, yytext); printf("TOKEN: IF, VALUE: %s\n", yytext); }
{ELSE}          { addToken(ELSE, yytext); printf("TOKEN: ELSE, VALUE: %s\n", yytext); }
{WHILE}         { addToken(WHILE, yytext); printf("TOKEN: WHILE, VALUE: %s\n", yytext); }

{FUNC}          { addToken(FUNC, yytext); printf("TOKEN: FUNC, VALUE: %s\n", yytext); }
{CALL}          { addToken(CALL, yytext); printf("TOKEN: CALL, VALUE: %s\n", yytext); }

{LBRACE}        { addToken(LBRACE, yytext); printf("TOKEN: LBRACE, VALUE: %s\n", yytext); }
{RBRACE}        { addToken(RBRACE, yytext); printf("TOKEN: RBRACE, VALUE: %s\n", yytext); }
{LPAREN}        { addToken(LPAREN, yytext); printf("TOKEN: LPAREN, VALUE: %s\n", yytext); }
{RPAREN}        { addToken(RPAREN, yytext); printf("TOKEN: RPAREN, VALUE: %s\n", yytext); }

{RELOP}           { addToken(RELOP, yytext); printf("TOKEN: RELOP, VALUE: %s\n", yytext); }  
{DO}           { addToken(DO, yytext); printf("TOKEN: DO, VALUE: %s\n", yytext); }  
{FOR}           { addToken(FOR, yytext); printf("TOKEN: FOR, VALUE: %s\n", yytext); }  
{AND}           { addToken(AND, yytext); printf("TOKEN: AND, VALUE: %s\n", yytext); }  
{OR}           { addToken(OR, yytext); printf("TOKEN: OR, VALUE: %s\n", yytext); }  

{COMMENT}       { printf("Skipping comment: %s\n", yytext); }  

{WHITESPACE}    { /* Do nothing */ }  

. { 
    printf("ERROR: Unrecognized character '%s'\n", yytext); 
    exit(0); 
}  

%%

int yywrap() { return 1; }
