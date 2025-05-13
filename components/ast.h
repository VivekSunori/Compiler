#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARENA_BLOCK_SIZE 8192 // 8KB blocks for memory pooling in order to remove memory leaks

// Forward declaration
typedef struct ASTNode ASTNode;

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
    NODE_PRINT,
    NODE_VAR_REF,
    NODE_STRING_LITERAL  // New node type for string literals
} NodeType;

// Arena allocator structure
typedef struct Arena
{
    char *block;
    size_t offset;
    size_t size;
    struct Arena *next;
} Arena;

// Define the ASTNode structure
struct ASTNode
{
    NodeType type;
    union 
    {
        int number;
        struct
        {
            char op;
            ASTNode *left, *right;
        } binaryOp;
        struct
        {
            char name[50];
            ASTNode *expr;
        } assign;
        struct
        {
            char name[50];
            ASTNode *value;
        } varDecl;
        struct
        {
            ASTNode *condition;
            ASTNode *thenStmt;
            ASTNode *elseStmt;
        } ifNode;
        struct
        {
            ASTNode *condition;
            ASTNode *body;
        } whileNode;
        struct
        {
            ASTNode *condition;
            ASTNode *body;
        } doWhileNode;
        struct
        {
            char name[50];
            ASTNode *params;
            ASTNode *body;
        } funcDef;
        struct
        {
            char name[50];
            ASTNode *args;
        } funcCall;
        struct
        {
            ASTNode *initialization;
            ASTNode *condition;
            ASTNode *increment;
            ASTNode *body;
        } forNode;
        struct
        {
            char op[3];
            ASTNode *left, *right;
        } relOp;
        struct
        {
            char op[3];
            ASTNode *left, *right;
        } logicalOp;
        int boolean;
        struct {
            ASTNode *expr;
        } print;
        struct {
            char name[50];  // Variable name for reference
        } varRef;
        struct {
            char value[256];  // String literal value
        } stringLiteral;
    };
    ASTNode *next;
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

