#include "../memory.h"
#include "header/parser.h"
#include "header/expressions.h"

// Keep the definition here
int evaluateExpression(ASTNode *expr) {
    if (!expr) return 0;
    
    switch (expr->type) {
        case NODE_NUMBER:
            return expr->number;
        case NODE_BINARY_OP:
            switch (expr->binaryOp.op) {
                case '+': return evaluateExpression(expr->binaryOp.left) + evaluateExpression(expr->binaryOp.right);
                case '-': return evaluateExpression(expr->binaryOp.left) - evaluateExpression(expr->binaryOp.right);
                case '*': return evaluateExpression(expr->binaryOp.left) * evaluateExpression(expr->binaryOp.right);
                case '/': {
                    int divisor = evaluateExpression(expr->binaryOp.right);
                    if (divisor == 0) {
                        printf("Error: Division by zero\n");
                        exit(1);
                    }
                    return evaluateExpression(expr->binaryOp.left) / divisor;
                }
                default: return 0;
            }
        default: return 0;
    }
}
 

/**
 * @brief  Parses a factor expression.
 *
 *  This function helps with parsing by parsing a factor expression, which can be a number or an identifier. Other token types will be added later.
 * 
 * @return The parsed factor expression.
 */
ASTNode* factor() {
    printf("%s", current->next->value);
    if (current && current->type == NUMBER) {
        ASTNode *node = allocateNode(NODE_NUMBER);
        node->number = atoi(current->value);
        nextToken();
        return node;
    } else if (current && current->type == STRING_LITERAL) {
        ASTNode *node = allocateNode(NODE_STRING_LITERAL);
        strcpy(node->stringLiteral.value, current->value);
        nextToken();
        return node;
    } else if (current && current->type == ID) {
        int index = lookupSymbol(current->value);
        if (index == -1) {
            printf("Semantic Error: Undefined variable '%s'\n", current->value);
            exit(1);
        }
        
        // Create a node for the variable reference
        ASTNode *node = allocateNode(NODE_VAR_REF);
        strcpy(node->varRef.name, current->value);
        
        printf("Variable reference: %s\n", current->value);
        
        nextToken();
        return node;
    } else if (current && current->type == LPAREN) {
        nextToken();
        ASTNode *node = parseExpression(0);
        if (current->type != RPAREN) {
            printf("Syntax Error: Expected ')'\n");
            exit(1);
        }
        nextToken();
        return node;
    } else {
        printf("Syntax Error: Expected number, string, variable, or '('\n");
        exit(1);
    }
}

/**
 * @brief  Parses a term expression.
 * 
 * This function helps with parsing by parsing a term expression. (* and /) 
 *
 * @return The parsed term expression
 */
ASTNode* term() {
    ASTNode *left = factor();
    while (current && current->type == OPERATOR && (current->value[0] == '*' || current->value[0] == '/')) {
        char op = current->value[0];
        nextToken();
        ASTNode *right = factor();
        ASTNode *node = allocateNode(NODE_BINARY_OP);
        node->binaryOp.op = op;
        node->binaryOp.left = left;
        node->binaryOp.right = right;
        left = node;
    }
    return left;
}


/**
 * @deprecated Use parseExpression instead, which is more general to handle cases like 4 + 2 * 3 --> 4 + (2 * 3)
 * @brief  Parses an expression.
 * 
 * This function helps with parsing by parsing an expression. (+ and -)
 * 
 * @return The parsed expression
 */
ASTNode* expression() {
    ASTNode *left = term();
    while (current && current->type == OPERATOR && (current->value[0] == '+' || current->value[0] == '-')) {
        char op = current->value[0];
        nextToken();
        ASTNode *right = term();
        ASTNode *node = allocateNode(NODE_BINARY_OP);
        node->binaryOp.op = op;
        node->binaryOp.left = left;
        node->binaryOp.right = right;
        left = node;
    }
    return left;
}

/**
 * @brief  Parses an expression.
 * 
 * This function helps with parsing by parsing an expression. (+, -, *, /, ^)
 * The precedence of each operator is determined by the getPrecedence function which helps with parsing to determine the order of operations.
 * 
 * @return The parsed expression ASTNode pointer*
 */
ASTNode* parseExpression(int minPrecedence) {
    ASTNode *left = factor();

    while (current && current->type == OPERATOR && getPrecedence(current->value[0]) >= minPrecedence) {
        char op = current->value[0];
        int precedence = getPrecedence(op);
        nextToken();

        if (isRightAssociative(op)) {
            precedence++;
        }

        ASTNode *right = parseExpression(precedence);

        ASTNode *node = allocateNode(NODE_BINARY_OP);
        node->binaryOp.op = op;
        node->binaryOp.left = left;
        node->binaryOp.right = right;
        left = node; 
    }

    return left;
}

ASTNode* parseCondition(int minPrecedence) {
    ASTNode *left = parseExpression(0); 

    while (current && (isLogicalOp(current->value) || isRelationalOp(current->value)) && 
           getPrecedence(current->value[0]) >= minPrecedence) {
        char op[3];
        strcpy(op, current->value);
        int precedence = getPrecedence(op[0]);
        nextToken();

        ASTNode *right = parseCondition(precedence);

        ASTNode *node = NULL;

        if (isLogicalOp(op)) {
            node = allocateNode(NODE_LOGICAL_OP);
            strcpy(node->logicalOp.op, op);
        } else if (isRelationalOp(op)) {
            node = allocateNode(NODE_RELATIONAL_OP);
            strcpy(node->relOp.op, op);
        }
        node->logicalOp.left = left;
        node->logicalOp.right = right;
        left = node;
    }

    return left;
}



int getPrecedence(char op) {
    if (op == '+' || op == '-') return 5;
    if (op == '*' || op == '/') return 6;
    if (op == '^') return 7;
    return 0;
}

// Determines if an operator is right-associative (like exponentiation)

int isRightAssociative(char op) {
    return (op == '^');  // Exponentiation is right-associative
}


ASTNode* logical() {
    ASTNode* left = comparison();
    while (current && current->type == OPERATOR && (strcmp(current->value, "&&") == 0 || strcmp(current->value, "||") == 0)) {
        char op[3];
        strcpy(op, current->value);
        nextToken();
        ASTNode* right = comparison();
        ASTNode* node = allocateNode(NODE_LOGICAL_OP);
        strcpy(node->logicalOp.op, op);  // Use strcpy instead of strdup
        node->logicalOp.left = left;
        node->logicalOp.right = right;
        left = node;
    }
    return left;
}

ASTNode* comparison() {
    ASTNode* left = parseExpression(1); 
    if (current && current->type == OPERATOR && isComparisonOperator(current->value)) {
        char op[3];
        strcpy(op, current->value);
        nextToken();
        ASTNode* right = parseExpression(1);
        
        // Use NODE_RELATIONAL_OP instead of NODE_COMPARISON_OP
        ASTNode* node = allocateNode(NODE_RELATIONAL_OP);
        
        // Use relOp field instead of comparisonOp
        strcpy(node->relOp.op, op);  // Copy the operator string
        node->relOp.left = left;
        node->relOp.right = right;
        return node;
    }
    return left;
}


int isLogicalOp(const char *op) {
    return strcmp(op, "&&") == 0 || strcmp(op, "||") == 0;
}

int isRelationalOp(const char *op) {
    return strcmp(op, "<") == 0 || strcmp(op, ">") == 0 ||
           strcmp(op, "<=") == 0 || strcmp(op, ">=") == 0 ||
           strcmp(op, "==") == 0 || strcmp(op, "!=") == 0;
}

int isComparisonOperator(const char *value) {
    return strcmp(value, "==") == 0 || 
           strcmp(value, "!=") == 0 || 
           strcmp(value, "<") == 0 || 
           strcmp(value, ">") == 0 || 
           strcmp(value, "<=") == 0 || 
           strcmp(value, ">=") == 0;
}
