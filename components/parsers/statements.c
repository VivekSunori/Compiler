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
    if (current->type == VAR) {  // Variable declaration
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
    else if (current->type == IF) {  // If statement
        nextToken();

        if (current->type != LPAREN) {
            printf("Error: Expected '(' after 'if'\n");
            exit(1);
        }
        nextToken();

        ASTNode *condition = parseExpression(0);

        if (current->type != RPAREN) {
            printf("Error: Expected ')' after condition\n");
            exit(1);
        }
        nextToken();

        if (current->type != LBRACE) {
            printf("Error: Expected '{' after 'if' condition\n");
            exit(1);
        }
        nextToken();

        ASTNode *thenStmt = parseBlock();
        ASTNode *elseStmt = NULL;

        if (current->type == ELSE) {
            nextToken();
            if (current->type != LBRACE) {
                printf("Error: Expected '{' after 'else'\n");
                exit(1);
            }
            nextToken();
            elseStmt = parseBlock();
        }

        ASTNode *ifNode = allocateNode(NODE_IF);
        ifNode->ifNode.condition = condition;
        ifNode->ifNode.thenStmt = thenStmt;
        ifNode->ifNode.elseStmt = elseStmt;

        return ifNode;
    }
    else if (current->type == WHILE) {  // While loop
        nextToken();

        if (current->type != LPAREN) {
            printf("Error: Expected '(' after 'while'\n");
            exit(1);
        }
        nextToken();

        ASTNode *condition = parseExpression(0);

        if (current->type != RPAREN) {
            printf("Error: Expected ')' after condition\n");
            exit(1);
        }
        nextToken();

        if (current->type != LBRACE) {
            printf("Error: Expected '{' after 'while' condition\n");
            exit(1);
        }
        nextToken();

        ASTNode *body = parseBlock();

        ASTNode *whileNode = allocateNode(NODE_WHILE);
        whileNode->whileNode.condition = condition;
        whileNode->whileNode.body = body;

        return whileNode;
    }
    else if (current->type == DO) {  // Do-While loop
        nextToken();

        if (current->type != LBRACE) {
            printf("Error: Expected '{' after 'do'\n");
            exit(1);
        }
        nextToken();

        ASTNode *body = parseBlock();

        if (current->type != WHILE) {
            printf("Error: Expected 'while' after 'do' block\n");
            exit(1);
        }
        nextToken();

        if (current->type != LPAREN) {
            printf("Error: Expected '(' after 'while'\n");
            exit(1);
        }
        nextToken();

        ASTNode *condition = parseExpression(0);

        if (current->type != RPAREN) {
            printf("Error: Expected ')' after condition\n");
            exit(1);
        }
        nextToken();

        if (current->type != SEMICOLON) {
            printf("Error: Expected ';' after 'do-while'\n");
            exit(1);
        }
        nextToken();

        ASTNode *doWhileNode = allocateNode(NODE_DO_WHILE);
        doWhileNode->doWhileNode.condition = condition;
        doWhileNode->doWhileNode.body = body;

        return doWhileNode;
    }
    else if (current->type == FOR) {  // For loop
        nextToken();

        if (current->type != LPAREN) {
            printf("Error: Expected '(' after 'for'\n");
            exit(1);
        }
        nextToken();

        ASTNode *initialization = statement();
        ASTNode *condition = parseExpression(0);

        if (current->type != SEMICOLON) {
            printf("Error: Expected ';' after for-loop condition\n");
            exit(1);
        }
        nextToken();

        ASTNode *increment = parseExpression(0);

        if (current->type != RPAREN) {
            printf("Error: Expected ')' after for-loop increment\n");
            exit(1);
        }
        nextToken();

        if (current->type != LBRACE) {
            printf("Error: Expected '{' after for-loop declaration\n");
            exit(1);
        }
        nextToken();

        ASTNode *body = parseBlock();

        ASTNode *forNode = allocateNode(NODE_FOR);
        forNode->forNode.initialization = initialization;
        forNode->forNode.condition = condition;
        forNode->forNode.increment = increment;
        forNode->forNode.body = body;

        return forNode;
    }
    else if (current->type == FUNC) {  // Function definition
        nextToken();

        if (current->type != ID) {
            printf("Error: Expected function name after 'func'\n");
            exit(1);
        }

        char funcName[MAX_VAR_NAME_LENGTH];
        strcpy(funcName, current->value);
        nextToken();

        if (current->type != LPAREN) {
            printf("Error: Expected '(' after function name\n");
            exit(1);
        }
        nextToken();

        ASTNode *params = parseParameterList();

        if (current->type != RPAREN) {
            printf("Error: Expected ')' after parameters\n");
            exit(1);
        }
        nextToken();

        if (current->type != LBRACE) {
            printf("Error: Expected '{' after function declaration\n");
            exit(1);
        }
        nextToken();

        ASTNode *body = parseBlock();

        ASTNode *funcNode = allocateNode(NODE_FUNC_DEF);
        strcpy(funcNode->funcDef.name, funcName);
        funcNode->funcDef.params = params;
        funcNode->funcDef.body = body;

        return funcNode;
    }
    else {
        printf("Error: Unexpected statement\n");
        exit(1);
    }
}
