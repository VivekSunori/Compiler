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
    ASTNode *cond = parseExpression(1);
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

/**
 * @brief  Parses a for loop.
 * 
 * This function helps with parsing by parsing a for loop.
 * 
 * @return The parsed for loop
 */
ASTNode* forLoop() {
    // Check for FOR keyword
    if (!current || current->type != FOR) {
        printf("Syntax Error: Expected 'for'\n");
        exit(1);
    }
    match(FOR);

    // Check for '('
    if (!current || current->type != LPAREN) {
        printf("Syntax Error: Expected '(' after 'for'\n");
        exit(1);
    }
    match(LPAREN);

    // Parse initialization
    ASTNode *initialization = statement();  // This should handle initialization (e.g., int i = 0;)
    if (!initialization) {
        printf("Syntax Error: Invalid initialization in for loop\n");
        exit(1);
    }

    // Check for ';' (end of initialization)
    if (!current || current->type != SEMICOLON) {
        printf("Syntax Error: Expected ';' after initialization in for loop\n");
        exit(1);
    }
    match(SEMICOLON);

    // Parse condition
    ASTNode *condition = parseExpression(1);  // This should handle the loop condition (e.g., i < 10)
    if (!condition) {
        printf("Syntax Error: Invalid condition in for loop\n");
        exit(1);
    }

    // Check for ';' (end of condition)
    if (!current || current->type != SEMICOLON) {
        printf("Syntax Error: Expected ';' after condition in for loop\n");
        exit(1);
    }
    match(SEMICOLON);

    // Parse increment
    ASTNode *increment = parseExpression(1);  // This should handle the increment statement (e.g., i++)
    if (!increment) {
        printf("Syntax Error: Invalid increment in for loop\n");
        exit(1);
    }

    // Check for ')'
    if (!current || current->type != RPAREN) {
        printf("Syntax Error: Expected ')' after loop components in for loop\n");
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
    ASTNode *body = statement();  // The body of the loop
    if (!body) {
        printf("Syntax Error: Invalid statement in for loop body\n");
        exit(1);
    }

    // Check for '}'
    if (!current || current->type != RBRACE) {
        printf("Syntax Error: Expected '}' after loop body\n");
        exit(1);
    }
    match(RBRACE);

    // Construct the for loop AST node
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) {
        printf("Memory Error: Failed to allocate memory for for loop node\n");
        exit(1);
    }

    node->type = NODE_FOR;
    node->forNode.initialization = initialization;
    node->forNode.condition = condition;
    node->forNode.increment = increment;
    node->forNode.body = body;

    return node;
}

/**
 * @brief  Parses a do-while loop.
 * 
 * This function helps with parsing by parsing a do-while loop.
 * 
 * @return The parsed do-while loop
 */
ASTNode* doWhileLoop() {
    // Check for DO keyword
    if (!current || current->type != DO) {
        printf("Syntax Error: Expected 'do'\n");
        exit(1);
    }
    match(DO);

    // Check for '{'
    if (!current || current->type != LBRACE) {
        printf("Syntax Error: Expected '{' before loop body\n");
        exit(1);
    }
    match(LBRACE);

    // Parse the loop body
    ASTNode *body = statement();
    if (!body) {
        printf("Syntax Error: Invalid statement in do-while loop body\n");
        exit(1);
    }

    // Check for '}'
    if (!current || current->type != RBRACE) {
        printf("Syntax Error: Expected '}' after loop body\n");
        exit(1);
    }
    match(RBRACE);

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
    ASTNode *condition = parseExpression(1);
    if (!condition) {
        printf("Syntax Error: Invalid condition in do-while loop\n");
        exit(1);
    }

    // Check for ')'
    if (!current || current->type != RPAREN) {
        printf("Syntax Error: Expected ')' after condition in do-while loop\n");
        exit(1);
    }
    match(RPAREN);

    // Construct the do-while AST node
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) {
        printf("Memory Error: Failed to allocate memory for do-while loop node\n");
        exit(1);
    }

    node->type = NODE_DO_WHILE;
    node->doWhileNode.condition = condition;
    node->doWhileNode.body = body;

    return node;
}
