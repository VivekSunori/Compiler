#ifndef MEMORY_H
#define MEMORY_H

#include "ast.h"

// Memory management functions
void* arenaAlloc(size_t size);
ASTNode* allocateNode(NodeType type);
void freeArena();

// Global variables
extern ASTNode* astHead;
extern ASTNode* astTail;
extern Arena* arenaHead;

#endif // MEMORY_H
