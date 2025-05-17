#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "components/ast.h"

// Function to check the semantics of an AST node
void checkSemantic(ASTNode *node);

// Function to perform semantic analysis and generate code
void analyzeAndGenerateCode(ASTNode *root, const char *outputFile);

// Function to get the type of an expression
int getExprType(ASTNode *expr);

#endif // SEMANTIC_H
