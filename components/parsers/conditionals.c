#include "header/parser.h"

/**
 * @brief  Parses a conditional statement with support for else and else-if.
 * 
 * This function helps with parsing a conditional statement. It can handle:
 * - if statement
 * - else statement
 * - nested else-if conditions
 * 
 * @return The parsed conditional statement
 */
ASTNode* conditional() {
    nextToken();
    
    if (current->type != LPAREN) {
        printf("Error: Missing '(' after IF\n");
        exit(1);
    }
    nextToken();
    
    // Parse the condition expression
    ASTNode *cond = parseCondition(1);
    if (current->type != RPAREN) {
        printf("Error: Missing ')' after condition\n");
        exit(1);
    }
    nextToken();

    // Check for '{' before the 'if' body
    if (current->type != LBRACE) {
        printf("Error: Missing '{' after condition\n");
        exit(1);
    }
    nextToken();
    
    // Parse the 'if' body statement
    ASTNode *thenStmt = statement();
    if (current->type != RBRACE) {
        printf("Error: Missing '}' after IF body\n");
        exit(1);
    }
    nextToken();

    // Handle 'else' or 'else if'
    if (current->type == ELSE) {
        nextToken();
        
        // Check for 'if' after 'else' (this handles the 'else if' case)
        if (current->type == IF) {
            ASTNode *elseIfStmt = conditional();  // Recursively handle the else-if case
            ASTNode *node = allocateNode(NODE_IF);
            if (!node) {
                printf("Memory Error: Failed to allocate memory for else-if node\n");
                exit(1);
            }
            node->ifNode.condition = cond;
            node->ifNode.thenStmt = thenStmt;
            node->ifNode.elseStmt = elseIfStmt;  // Assign the else-if statement as the else
            return node;
        }
        
        // Otherwise, handle a plain 'else'
        if (current->type == LBRACE) {
            ASTNode *elseStmt = statement();  // Parse the 'else' body
            if (current->type != RBRACE) {
                printf("Error: Missing '}' after ELSE body\n");
                exit(1);
            }
            nextToken();
            
            ASTNode *node = allocateNode(NODE_IF);
            if (!node) {
                printf("Memory Error: Failed to allocate memory for else node\n");
                exit(1);
            }
            node->ifNode.condition = cond;
            node->ifNode.thenStmt = thenStmt;
            node->ifNode.elseStmt = elseStmt;  // Assign the 'else' statement
            return node;
        }
    }

    // No else, return the current node
    ASTNode *node = allocateNode(NODE_IF);
    if (!node) {
        printf("Memory Error: Failed to allocate memory for if node\n");
        exit(1);
    }
    node->ifNode.condition = cond;
    node->ifNode.thenStmt = thenStmt;
    node->ifNode.elseStmt = NULL;  // No 'else' or 'else if'
    return node;
}
