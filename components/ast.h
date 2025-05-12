#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARENA_BLOCK_SIZE 8192 // 8KB blocks for memory pooling in order to remove memory leaks

typedef enum
{
    NODE_NUMBER,
    NODE_BINARY_OP,
    NODE_ASSIGN,
    NODE_IF,
    NODE_WHILE,
    NODE_DO_WHILE,
    NODE_FUNC_DEF,
    NODE_FUNC_CALL,
    NODE_FOR,
    NODE_VAR_DECL,
    NODE_LOGICAL_OP,
    NODE_RELATIONAL_OP,
    NODE_COMPARISON_OP,
    NODE_PRINT
} NodeType;

// Arena allocator structure
typedef struct Arena
{
    char *block;
    size_t offset;
    size_t size;
    struct Arena *next;
} Arena;

typedef struct ASTNode ASTNode;
struct ASTNode
{
    NodeType type;
    union 
    {
        int number;
        struct
        {
            char op;
            struct ASTNode *left, *right;
        } binaryOp;
        struct
        {
            char name[50];
            struct ASTNode *expr;
        } assign;
        struct
        {
            char name[50];
            ASTNode *value;
        } varDecl;
        struct
        {
            struct ASTNode *condition;
            struct ASTNode *thenStmt;
            struct ASTNode *elseStmt;
        } ifNode;
        struct
        {
            struct ASTNode *condition;
            struct ASTNode *body;
        } whileNode;
        struct
        {
            struct ASTNode *condition;
            struct ASTNode *body;
        } doWhileNode;
        struct
        {
            char name[50];
            struct ASTNode *params;
            struct ASTNode *body;
        } funcDef;
        struct
        {
            char name[50];
            struct ASTNode *args;
        } funcCall;
        struct
        {
            struct ASTNode *initialization;
            struct ASTNode *condition;
            struct ASTNode *increment;
            struct ASTNode *body;
        } forNode;
        struct
        {
            char op[3];
            struct ASTNode *left, *right;
        } relOp;

        struct
        {
            char op[3];
            struct ASTNode *left, *right;
        } logicalOp;

        int boolean;
        struct {
            struct ASTNode *expr;
        } print;
    };
    struct ASTNode *next;
};

// Global AST head and tail
extern ASTNode *astHead;
extern ASTNode *astTail;

// Arena global instance
extern Arena *arenaHead;

// Function to allocate a new memory block in the arena
void *arenaAlloc(size_t size);

// Function to allocate an ASTNode using the arena
ASTNode *allocateNode(NodeType type);

void freeArena();

#endif

