#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "components/ast.h"

// Function to check the semantics of an AST node
void checkSemantic(ASTNode *node);

// Function to perform semantic analysis and generate code
void analyzeAndGenerateCode(ASTNode *root, const char *outputFile);

#endif // SEMANTIC_H