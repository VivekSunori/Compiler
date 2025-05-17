#ifndef TOKENS_H
#define TOKENS_H

#include <stdio.h>

// Token types
typedef enum {
    UNKNOWN,
    NUMBER,
    ID,
    OPERATOR,
    LOGICAL_OP,
    RELATIONAL_OP,
    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,
    SEMICOLON,
    COMMA,
    ASSIGN,
    IF,
    ELSE,
    WHILE,
    DO,
    FOR,
    FUNC,
    CALL,
    RETURN,
    VAR,
    PRINT,
    STRING_LITERAL,
    BOOLEAN_LITERAL,
    AND,
    OR,
    RELOP,
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

#define END_OF_FILE 999
