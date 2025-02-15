#include "tokens.h"

Token *head = NULL, *tail = NULL;

void addToken(TokenType type, const char *value) {
    Token* newToken = (Token*)malloc(sizeof(Token));
    newToken->type = type;
    strcpy(newToken->value, value);
    newToken->next = NULL;

    if (!head) head = tail = newToken;
    else {
        tail->next = newToken;
        tail = newToken;
    }
}
