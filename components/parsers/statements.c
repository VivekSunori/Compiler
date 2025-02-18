#include "header/parser.h"


/**
 * @brief  Parses a statement.
 * 
 * This function helps with parsing by parsing a statement. It can be a variable assignment or a conditional statement.
 * 
 * @return The parsed statement
 */
ASTNode* statement() {
    if (current && current->type == KEYWORD) {
        char varName[50];
        strcpy(varName, current->value);
        nextToken();
        if (current->type != ID) {
            printf("Error: Expected variable after identifier\n");
            exit(1);
        }
        nextToken();
        if (current->type != ASSIGN) {
            printf("Error: Expected '=' after identifier\n");
        }
        nextToken();
        ASTNode *exprNode = expression();
        int index = lookupSymbol(varName);
        if (index == -1) addSymbol(varName, 0);
        return exprNode;
    } else {
        printf("Error: Expected keyword at start of statement\n");
        exit(1);
    }
}