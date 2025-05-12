#ifndef EXPRESSIONS_H
#define EXPRESSIONS_H

#include "../../ast.h"

ASTNode* factor();
ASTNode* term();
ASTNode* expression();
ASTNode* parseExpression(int minPrecedence);
ASTNode* parseCondition(int minPrecedence);
int getPrecedence(char op); 
int isRightAssociative(char op);
ASTNode* logical();
ASTNode* comparison();
int isLogicalOp(const char *op);
int isRelationalOp(const char *op);
int isComparisonOperator(const char *value);
int evaluateExpression(ASTNode *expr);

#endif // EXPRESSIONS_H

