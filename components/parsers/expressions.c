#include "header/parser.h"

/**
 * @brief  Parses a factor expression.
 *
 *  This function helps with parsing by parsing a factor expression, which can be a number or an identifier. Other token types will be added later.
 * 
 * @return The parsed factor expression.
 */
ASTNode* factor() {
    printf("%s",current->next->value);
    if (current && current->type == NUMBER) {
        ASTNode *node = malloc(sizeof(ASTNode));
        node->type = NODE_NUMBER;
        node->number = atoi(current->value);
        nextToken();
        return node;
    } else if (current && current->type == ID) {
        int index = lookupSymbol(current->value);
        if (index == -1) {
            printf("Semantic Error: Undefined variable '%s'\n", current->value);
            exit(1);
        }
        ASTNode *node = malloc(sizeof(ASTNode));
        node->type = NODE_NUMBER;
        node->number = symTable[index].value;
        nextToken();
        return node;
    } else {
        printf("Error: Expected a number or identifier\n");
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
        ASTNode *node = malloc(sizeof(ASTNode));
        node->type = NODE_BINARY_OP;
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
        ASTNode *node = malloc(sizeof(ASTNode));
        node->type = NODE_BINARY_OP;
        node->binaryOp.op = op;
        node->binaryOp.left = left;
        node->binaryOp.right = right;
        left = node;
    }
    return left;
}

ASTNode* parseExpression(int minPrecedence) {
    ASTNode *left = factor();  // Get the first operand (number, variable, function call)

    while (current && current->type == OPERATOR && getPrecedence(current->value[0]) >= minPrecedence) {
        char op = current->value[0];  // Store the operator
        int precedence = getPrecedence(op);
        nextToken();

        // Right-associative operators (e.g., power ^) need +1 precedence
        if (isRightAssociative(op)) {
            precedence++;
        }

        ASTNode *right = parseExpression(precedence);  // Recursively parse right operand

        // Create a new binary operation node
        ASTNode *node = malloc(sizeof(ASTNode));
        node->type = NODE_BINARY_OP;
        node->binaryOp.op = op;
        node->binaryOp.left = left;
        node->binaryOp.right = right;
        left = node;  // Update left operand for next iteration
    }

    return left;
}


int getPrecedence(char op) {
    switch (op) {
        case '+': case '-': return 1;
        case '*': case '/': return 2;
        case '^': return 3;  // Highest precedence (right-associative)
        default: return 0;
    }
}

// Determines if an operator is right-associative (like exponentiation)

int isRightAssociative(char op) {
    return (op == '^');  // Exponentiation is right-associative
}

