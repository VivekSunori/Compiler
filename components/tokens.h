#ifndef TOKENS_H
#define TOKENS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum { 
    ID, NUMBER, KEYWORD, OPERATOR, ASSIGN, END, SEMICOLON, COMMENT,
    IF, ELSE, WHILE, LBRACE, RBRACE, LPAREN, RPAREN, FUNC, CALL, COMMA
} TokenType;

typedef struct Token {
    TokenType type;
    char value[100];
    struct Token* next;
} Token;

extern Token *head, *tail;

void addToken(TokenType type, const char *value);

#endif
