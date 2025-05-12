#include "tokens.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>  // Add this for isdigit()

// Global token variables
Token* head = NULL;
Token* current = NULL;

// External declarations for the lexer
extern FILE *yyin;
extern int tokenizer();

// Add a token to the linked list
void addToken(TokenType type, const char* value) {
    Token* token = malloc(sizeof(Token));
    token->type = type;
    strcpy(token->value, value);
    token->next = NULL;
    
    if (!head) {
        head = token;
        current = token;
    } else {
        Token* temp = head;
        while (temp->next) {
            temp = temp->next;
        }
        temp->next = token;
    }
}

// Convert token type to string
const char* tokenTypeToString(TokenType type) {
    switch (type) {
        case NUMBER: return "NUMBER";
        case ID: return "ID";
        case OPERATOR: return "OPERATOR";
        case ASSIGN: return "ASSIGN";
        case SEMICOLON: return "SEMICOLON";
        case LBRACE: return "LBRACE";
        case RBRACE: return "RBRACE";
        case LPAREN: return "LPAREN";
        case RPAREN: return "RPAREN";
        case IF: return "IF";
        case ELSE: return "ELSE";
        case WHILE: return "WHILE";
        case FOR: return "FOR";
        case DO: return "DO";
        case FUNC: return "FUNC";
        case CALL: return "CALL";
        case COMMA: return "COMMA";
        case AND: return "AND";
        case OR: return "OR";
        case RELOP: return "RELOP";
        case VAR: return "VAR";
        case END: return "END";
        default: return "UNKNOWN";
    }
}

// Function to tokenize source code
void tokenize(FILE *file) {
    if (!file) {
        perror("Failed to open source file");
        exit(1);
    }

    rewind(file); // Reset file pointer for lexing

    // Call the flex-generated tokenizer
    yyin = file;
    tokenizer();
    
    printf("\nTokenizing completed!\n");
}
