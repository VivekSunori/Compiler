#ifndef TOKENS_H
#define TOKENS_H

#include <stdio.h>

// Token types
typedef enum {
    NUMBER,
    ID,
    OPERATOR,
    ASSIGN,
    SEMICOLON,
    LBRACE,
    RBRACE,
    LPAREN,
    RPAREN,
    IF,
    ELSE,
    WHILE,
    FOR,
    DO,
    FUNC,
    CALL,
    COMMA,
    AND,
    OR,
    RELOP,
    VAR,
    PRINT,
    STRING_LITERAL,
    END
} TokenType;

// Token structure
typedef struct Token {
    TokenType type;
    char value[100];
    struct Token* next;
} Token;

// Function declarations
void addToken(TokenType type, const char* value);
const char* tokenTypeToString(TokenType type);
void tokenize(FILE *file);

// Global token variables
extern Token* head;
extern Token* current;

#endif // TOKENS_H
