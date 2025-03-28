#include "header/parser.h"

#define MAX_VAR_NAME_LENGTH 50

/**
 * @brief Parses a statement.
 * 
 * This function parses variable declarations, assignments, loops, function calls, and conditionals.
 * 
 * @return The parsed statement AST node.
 */
ASTNode* statement() {
    switch (current->type) {

        case VAR: {
            nextToken();
            if (current->type != ID) {
                printf("Error: Expected variable name after 'var'\n");
                exit(1);
            }
            char varName[MAX_VAR_NAME_LENGTH];
            strcpy(varName, current->value);
            nextToken();
            if (current->type != ASSIGN) {
                printf("Error: Expected '=' after variable name\n");
                exit(1);
            }
            nextToken();
            ASTNode *expr = parseExpression(0);
            if (current->type != SEMICOLON) {
                printf("Error: Missing ';' after variable declaration\n");
                exit(1);
            }
            nextToken();
            ASTNode *node = allocateNode(NODE_VAR_DECL);
            strcpy(node->varDecl.name, varName);
            node->varDecl.value = expr;
            insertSymbol(varName, evaluateExpression(expr));
            return node;
        }

        case IF: {
            return conditional();
        }

        case WHILE: {
            return loop();
        }

        case DO: {
            return doWhileLoop();
        }

        case FOR: {
            return forLoop();
        }

        case FUNC: {
            nextToken();
            if (current->type != ID) {
                printf("Error: Expected function name after 'func'\n");
                exit(1);
            }
           return functionDef();
        }

        default: {
            printf("Error: Unexpected statement\n");
            exit(1);
        }
    }
}
