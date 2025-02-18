#include "header/parser.h"

/**
 * @brief  Parses a while loop.
 * 
 * This function helps with parsing by parsing a while loop. It can be a while loop.
 * 
 * @return The parsed while loop
 */
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