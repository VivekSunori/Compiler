// parser.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "components/tokens.h"
#include "utils/lex.yy.c"
#include "components/ast.h"

#define MAX_SYMBOLS 100

// Define the symbol table
typedef struct {
    char name[50];
    int value;
} Symbol;

Symbol symTable[MAX_SYMBOLS];
int symCount = 0;

Token *current = NULL;

int lookupSymbol(const char *name) {
    for (int i = 0; i < symCount; i++) {
        if (strcmp(symTable[i].name, name) == 0) return i;
    }
    return -1;
}

void addSymbol(const char *name, int value) {
    if (symCount < MAX_SYMBOLS) {
        strcpy(symTable[symCount].name, name);
        symTable[symCount].value = value;
        symCount++;
    } else {
        printf("Symbol table full!");
        exit(1);
    }
}

void nextToken() {
    if (current) current = current->next;
}


void match(TokenType expected)
{
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

ASTNode* factor() {
    if (current && current->type == NUMBER) {
        ASTNode *node = malloc(sizeof(ASTNode));
        node->type = NODE_NUMBER;
        node->number = atoi(current->value);
        nextToken();
        return node;
    } else if (current && current->type == ID) {
        int index = lookupSymbol(current->value);
        if (index == -1) {
            printf("Semantic Error: Undefined variable '%s'\n", current->value);
            exit(1);
        }
        ASTNode *node = malloc(sizeof(ASTNode));
        node->type = NODE_NUMBER;
        node->number = symTable[index].value;
        nextToken();
        return node;
    } else {
        printf("Error: Expected a number or identifier\n");
        exit(1);
    }
}

ASTNode* term() {
    ASTNode *left = factor();
    while (current && current->type == OPERATOR && (current->value[0] == '*' || current->value[0] == '/')) {
        char op = current->value[0];
        nextToken();
        ASTNode *right = factor();
        ASTNode *node = malloc(sizeof(ASTNode));
        node->type = NODE_BINARY_OP;
        node->binaryOp.op = op;
        node->binaryOp.left = left;
        node->binaryOp.right = right;
        left = node;
    }
    return left;
}

ASTNode* expression() {
    ASTNode *left = term();
    while (current && current->type == OPERATOR && (current->value[0] == '+' || current->value[0] == '-')) {
        char op = current->value[0];
        nextToken();
        ASTNode *right = term();
        ASTNode *node = malloc(sizeof(ASTNode));
        node->type = NODE_BINARY_OP;
        node->binaryOp.op = op;
        node->binaryOp.left = left;
        node->binaryOp.right = right;
        left = node;
    }
    return left;
}

ASTNode* statement() {
    if (current && current->type == ID) {
        char varName[50];
        strcpy(varName, current->value);
        nextToken();
        if (current->type != ASSIGN) {
            printf("Error: Expected '=' after identifier\n");
            exit(1);
        }
        nextToken();
        ASTNode *exprNode = expression();
        int index = lookupSymbol(varName);
        if (index == -1) addSymbol(varName, 0);
        return exprNode;
    } else {
        printf("Error: Expected identifier at start of statement\n");
        exit(1);
    }
}

ASTNode* conditional() {
    nextToken();
    if (current->type != LPAREN) {
        printf("Error: Missing '(' after IF\n");
        exit(1);
    }
    nextToken();
    ASTNode *cond = expression();
    if (current->type != RPAREN) {
        printf("Error: Missing ')' after condition\n");
        exit(1);
    }
    nextToken();
    if (current->type != LBRACE) {
        printf("Error: Missing '{' after condition\n");
        exit(1);
    }
    nextToken();
    ASTNode *thenStmt = statement();
    if (current->type != RBRACE) {
        printf("Error: Missing '}' after IF body\n");
        exit(1);
    }
    nextToken();
    return thenStmt;
}

ASTNode* functionDef() {
    nextToken();
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = NODE_FUNC_DEF;
    strcpy(node->funcDef.name, current->value);
    nextToken();
    if (current->type != LPAREN) {
        printf("Error: Missing '(' after FUNC name\n");
        exit(1);
    }
    nextToken();
    if (current->type != RPAREN) {
        printf("Error: Missing ')' after FUNC parameters\n");
        exit(1);
    }
    nextToken();
    if (current->type != LBRACE) {
        printf("Error: Missing '{' after FUNC\n");
        exit(1);
    }
    nextToken();
    node->funcDef.body = statement();
    if (current->type != RBRACE) {
        printf("Error: Missing '}' after FUNC body\n");
        exit(1);
    }
    nextToken();
    return node;
}

ASTNode* functionCall() {
    nextToken();
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = NODE_FUNC_CALL;
    strcpy(node->funcCall.name, current->value);
    nextToken();
    if (current->type != LPAREN) {
        printf("Error: Missing '(' after CALL name\n");
        exit(1);
    }
    nextToken();
    node->funcCall.args = expression();
    if (current->type != RPAREN) {
        printf("Error: Missing ')' after CALL arguments\n");
        exit(1);
    }
    nextToken();
    return node;
}

void parseTokens() {
    current = head;
    while (current && current->type != END) {
        if (current->type == FUNC) functionDef();
        else if (current->type == CALL) functionCall();
        else statement();
    }
    printf("Parsing completed successfully.\n");
}

// ASTNode* loop() {
//     match(WHILE);
//     match(LPAREN);
//     ASTNode *cond = expression();
//     match(RPAREN);
//     match(LBRACE);
//     ASTNode *body = statement();
//     match(RBRACE);

//     ASTNode *node = malloc(sizeof(ASTNode));
//     node->type = NODE_WHILE;
//     node->whileNode.condition = cond;
//     node->whileNode.body = body;
//     return node;
// }

ASTNode* loop() {
    // Check for WHILE keyword
    if (!current || current->type != WHILE) {
        printf("Syntax Error: Expected 'while'\n");
        exit(1);
    }
    match(WHILE);

    // Check for '('
    if (!current || current->type != LPAREN) {
        printf("Syntax Error: Expected '(' after 'while'\n");
        exit(1);
    }
    match(LPAREN);

    // Parse the loop condition
    ASTNode *cond = expression();
    if (!cond) {
        printf("Syntax Error: Invalid condition in while loop\n");
        exit(1);
    }

    // Check for ')'
    if (!current || current->type != RPAREN) {
        printf("Syntax Error: Expected ')' after condition in while loop\n");
        exit(1);
    }
    match(RPAREN);

    // Check for '{'
    if (!current || current->type != LBRACE) {
        printf("Syntax Error: Expected '{' before loop body\n");
        exit(1);
    }
    match(LBRACE);

    // Parse the loop body
    ASTNode *body = statement();
    if (!body) {
        printf("Syntax Error: Invalid statement in while loop body\n");
        exit(1);
    }

    // Check for '}'
    if (!current || current->type != RBRACE) {
        printf("Syntax Error: Expected '}' after loop body\n");
        exit(1);
    }
    match(RBRACE);

    // Construct the loop AST node
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) {
        printf("Memory Error: Failed to allocate memory for while loop node\n");
        exit(1);
    }

    node->type = NODE_WHILE;
    node->whileNode.condition = cond;
    node->whileNode.body = body;

    return node;
}



void tokenize(FILE *file)
{
    if (!file)
    {
        perror("Failed to open source file");
        exit(1);
    }

    rewind(file); // Reset file pointer for lexing

    yyin = file;
    tokenizer();
    printf("\nTokenizing completed!\n");
    /*
    For debug purposes only
    // Print tokens
    Token *temp = head;
    if (!temp) {
        printf("No tokens generated!\n");
    }
    while (temp) {
        printf("[TOKEN: | VALUE: %s]\n",  temp->value);
        temp = temp->next;
    }
    */
}
