#include "../memory.h"
#include "./header/parser.h"
#include "./header/loops.h"
#include "./header/expressions.h"

#define MAX_VAR_NAME_LENGTH 50

extern int evaluateExpression(ASTNode *expr);

void insertSymbol(const char *name, int value) {
    int index = lookupSymbol(name);
    if (index != -1) {
        symTable[index].value = value;
    } else {
        addSymbol(name, value);
    }
}

/**
 * @brief Parses a statement.
 * 
 * This function parses variable declarations, assignments, loops, function calls, and conditionals.
 * 
 * @return The parsed statement AST node.
 */
ASTNode* statement() {
    switch (current->type) {
        case ID: {
            // In case of ID I'll check in symbol pool if already defined otherwise the VAR case will handle it or I'll throw an error
            int index = lookupSymbol(current->value);
            if (index != -1) {
                nextToken();
                if (current->type == ASSIGN) {
                    nextToken();
                    ASTNode *expr = parseExpression(0);
                    if (current->type != SEMICOLON) {
                        printf("Error: Missing ';' after assignment\n");
                        exit(1);
                    }
                    nextToken();
                    ASTNode *node = allocateNode(NODE_ASSIGN);
                    strcpy(node->assign.name, current->value);
                    node->assign.expr = expr;
                    insertSymbol(current->value, evaluateExpression(expr));
                    return node;
                } else {
                    printf("Error: Unexpected token after variable name\n");
                    exit(1);
                }
            } else {
                printf("Error: Variable '%s' not declared type: %s\n", current->value, tokenTypeToString(current->type));
                exit(1);
            }
        }

        case VAR: {
            nextToken();
            if (current->type != ID) {
                printf("Error: Expected variable name after 'var'\n");
                exit(1);
            }

            if (strcmp(current->value, "num") == 0 || 
                strcmp(current->value, "log") == 0 || 
                strcmp(current->value, "str") == 0) {
                printf("Error: '%s' is a reserved keyword and cannot be used as a variable name\n", current->value);
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

        case PRINT: {
            nextToken();
            ASTNode *expr = parseExpression(0);
            if (current->type != SEMICOLON) {
                printf("Error: Missing ';' after print statement\n");
                exit(1);
            }
            nextToken();
            ASTNode *node = allocateNode(NODE_PRINT);
            node->print.expr = expr;
            return node;
        }

        default: {
            printf("Error: Unexpected statement\n type: %s , value %s \n", tokenTypeToString(current->type) , current->value);
            exit(1);
        }
    }
}
