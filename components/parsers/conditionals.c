#include "header/parser.h"

/**
 * @brief  Parses a conditional statement.
 * 
 * This function helps with parsing by parsing a conditional statement. It can be an IF statement.
 * 
 * @attention This function does not support nested conditions or else statements yet.
 * @return The parsed conditional statement
 */
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