#ifndef CODEGEN_H
#define CODEGEN_H

#include "../ast.h"

// Function to generate assembly code
void generateAssembly(const char *filename);

// Function to generate code for a node
void generateCode(ASTNode *node, FILE *asmFile);

// Reset visited nodes for code generation
void codegenResetVisited();

#endif // CODEGEN_H