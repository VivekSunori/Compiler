#include "header/parser.h"

/**
 * @brief  Parses a function definition.
 * 
 * This function helps with parsing by parsing a function definition. It can be a function definition.
 * 
 * @return The parsed function definition
 */
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

/**
 * @brief  Parses a function call.
 * 
 * This function helps with parsing by parsing a function call. It can be a function call.
 * 
 * @return The parsed function call
 */
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