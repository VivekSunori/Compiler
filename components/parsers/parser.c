#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header/parser.h"
#include "expressions.c"
#include "statements.c"
#include "conditionals.c"
#include "functions.c"
#include "loops.c"

// Define nextToken and match functions here (or move them to expression.c if necessary)
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

// Other parser code here if necessary
