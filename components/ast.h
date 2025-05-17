#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbol_table.h"  // Include this to get VariableType definition

#define ARENA_BLOCK_SIZE 8192 // 8KB blocks for memory pooling in order to remove memory leaks

// Forward declaration
typedef struct ASTNode ASTNode;

typedef enum
{
    NODE_UNKNOWN,
    NODE_NUMBER,
    NODE_STRING_LITERAL,
    NODE_BOOLEAN_LITERAL,
    NODE_VAR_REF,
    NODE_BINARY_OP,
    NODE_LOGICAL_OP,     // For logical operators like && and ||
    NODE_RELATIONAL_OP,  // For relational operators like ==, !=, <, >, <=, >=
    NODE_COMPARISON_OP,  // For comparison operations
    NODE_ASSIGN,
    NODE_VAR_DECL,
    NODE_PRINT,
    NODE_IF,
    NODE_WHILE,
    NODE_DO_WHILE,
    NODE_FOR,
    NODE_FUNC_DEF,
    NODE_FUNC_CALL,
    NODE_RETURN
} NodeType;

// Arena allocator structure
typedef struct Arena
{
    char *block;
    size_t offset;
    size_t size;
    struct Arena *next;
} Arena;

// Function parameter structure
typedef struct {
    char name[50];
    int type;  // TYPE_INT, TYPE_FLOAT, etc.
} FunctionParam;

// Function definition node
struct FuncDefNode {
    char name[50];
    FunctionParam *params;  // Array of parameters
    int paramCount;         // Number of parameters
    struct ASTNode *body;
    int returnType;         // Return type (TYPE_INT, TYPE_VOID, etc.)
};

// Function call node
struct FuncCallNode {
    char name[50];
    struct ASTNode *args;   // Linked list of argument expressions
    int argCount;           // Number of arguments
};

// Return statement node
struct ReturnNode {
    struct ASTNode *expr;   // Return expression (can be NULL for void functions)
};

// Define the AST node structure
typedef struct ASTNode {
    NodeType type;
    
    union {
        // For number literals
        int number;
        
        // For string literals
        struct {
            char value[100];
        } stringLiteral;
        
        // For boolean literals
        struct {
            char value[32];
        } booleanLiteral;
        
        // For variable references
        struct {
            char name[MAX_VAR_NAME_LENGTH];
        } varRef;
        
        // For binary operations (+, -, *, /)
        struct {
            char op;
            struct ASTNode *left;
            struct ASTNode *right;
        } binaryOp;
        
        // For logical operations (&&, ||)
        struct {
            char op[3];
            struct ASTNode *left;
            struct ASTNode *right;
        } logicalOp;
        
        // For relational operations (==, !=, <, >, <=, >=)
        struct {
            char op[3];
            struct ASTNode *left;
            struct ASTNode *right;
        } relOp;
        
        // For comparison operations
        struct {
            char op[3];
            struct ASTNode *left;
            struct ASTNode *right;
        } compOp;
        
        // For assignment operations
        struct {
            char name[MAX_VAR_NAME_LENGTH];
            struct ASTNode *expr;
        } assign;
        
        // For variable declarations
        struct {
            char name[MAX_VAR_NAME_LENGTH];
            struct ASTNode *value;
            int type;  // Variable type (int, string, etc.)
        } varDecl;
        
        // For print statements
        struct {
            struct ASTNode *expr;
        } print;
        
        // For if statements
        struct {
            struct ASTNode *condition;
            struct ASTNode *thenStmt;
            struct ASTNode *elseStmt;
        } ifNode;
        
        // For while loops
        struct {
            struct ASTNode *condition;
            struct ASTNode *body;
        } whileNode;
        
        // For do-while loops
        struct {
            struct ASTNode *body;
            struct ASTNode *condition;
        } doWhileNode;
        
        // For for loops
        struct {
            struct ASTNode *initialization;
            struct ASTNode *condition;
            struct ASTNode *increment;
            struct ASTNode *body;
        } forNode;
        
        // For function definitions
        struct {
            char name[MAX_VAR_NAME_LENGTH];
            struct ASTNode *params;
            struct ASTNode *body;
        } funcDef;
        
        // For function calls
        struct {
            char name[MAX_VAR_NAME_LENGTH];
            struct ASTNode *args;
        } funcCall;
    };
    
    struct ASTNode *next;
} ASTNode;

// Function declarations
Arena *createArena();
void *allocateFromArena(Arena *arena, size_t size);
void freeArena();
ASTNode *allocateNode(NodeType type);

#endif // AST_H

