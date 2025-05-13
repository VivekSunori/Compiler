#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stddef.h>

#define MAX_SYMBOLS 100
#define MAX_VAR_NAME_LENGTH 50

// Forward declaration of ASTNode
typedef struct ASTNode ASTNode;

// Define variable types
typedef enum {
    TYPE_NUMBER,
    TYPE_STRING,
    TYPE_BOOLEAN
} VariableType;

// Define symbol structure
typedef struct {
    char name[MAX_VAR_NAME_LENGTH];
    int value;
    VariableType type;
} Symbol;

// Function declarations
void addSymbol(const char *name, int value);
void insertSymbol(const char *name, int value, VariableType type);
int lookupSymbol(const char *name);
VariableType lookupSymbolType(const char *name);
int getSymbolType(const char *name);
int getExprType(ASTNode *expr);
int getFunctionArgCount(const char *name);
int countArgs(ASTNode *args);
int* getFunctionArgTypes(const char *name);
int getCurrentFunctionReturnType();
const char* getCurrentFunctionName();
void addFunctionSymbol(const char *name, int returnType, int *paramTypes, int paramCount);

// Global symbol table
extern Symbol symTable[MAX_SYMBOLS];
extern int symCount;

#endif // SYMBOL_TABLE_H
