#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header/parser.h"
#include "expressions.c"
#include "statements.c"
#include "conditionals.c"
#include "functions.c"
#include "loops.c"

void nextToken() {
    if (current) current = current->next;
}

void match(TokenType expected) {
    if (current && current->type == expected)
    {
        nextToken();
    }
    else
    {
        printf("Syntax Error: Unexpected token '%s'\n", current ? current->value : "NULL");
        exit(1);
    }
}

void expect(TokenType expected) {
    if (current && current->type != expected)
    {
        printf("Syntax Error: Expected '%s' but found '%s'\n", tokenTypeToString(expected), tokenTypeToString(current->type));
        exit(1);
    }else{
    nextToken();
    }
}