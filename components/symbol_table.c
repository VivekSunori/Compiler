#include "symbol_table.h"
#include "ast.h"  // Add this include for ASTNode definition
#include <string.h>
#include <stdio.h>
#include <stdlib.h>  // Add this for exit() function

// Global symbol table
Symbol symTable[MAX_SYMBOLS];
int symCount = 0;

// Add a symbol to the symbol table (legacy function for backward compatibility)
void addSymbol(const char *name, int value) {
    insertSymbol(name, value, TYPE_NUMBER);
}

// Add a symbol to the symbol table with type information
void insertSymbol(const char *name, int value, VariableType type) {
    if (symCount >= MAX_SYMBOLS) {
        printf("Error: Symbol table full\n");
        exit(1);
    }
    
    strcpy(symTable[symCount].name, name);
    symTable[symCount].value = value;
    symTable[symCount].type = type;
    symCount++;
}

// Look up a symbol in the symbol table
int lookupSymbol(const char *name) {
    for (int i = 0; i < symCount; i++) {
        if (strcmp(symTable[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

// Update the lookupSymbolType function
VariableType lookupSymbolType(const char *name) {
    for (int i = 0; i < symCount; i++) {
        if (strcmp(symTable[i].name, name) == 0) {
            return symTable[i].type;
        }
    }
    return TYPE_NUMBER; // Default to number if not found
}

// Get the type of an expression (for now, just return a dummy value)
int getExprType(ASTNode *expr) {
    // In a real implementation, you would analyze the expression
    // For now, just return 0 (assuming 0 is a valid type)
    return 0;
}

// Get the number of arguments a function expects
int getFunctionArgCount(const char *name) {
    // In a real implementation, you would store and retrieve this information
    // For now, just return 0
    return 0;
}

// Count the number of arguments in an argument list
int countArgs(ASTNode *args) {
    int count = 0;
    ASTNode *arg = args;
    while (arg) {
        count++;
        arg = arg->next;
    }
    return count;
}

// Get the types of arguments a function expects
int* getFunctionArgTypes(const char *name) {
    // In a real implementation, you would store and retrieve this information
    // For now, just return a static array with a single type
    static int types[1] = {0};
    return types;
}

// Get the return type of the current function
int getCurrentFunctionReturnType() {
    // In a real implementation, you would track the current function
    // For now, just return 0
    return 0;
}

// Get the name of the current function
const char* getCurrentFunctionName() {
    // In a real implementation, you would track the current function
    // For now, just return a dummy name
    return "unknown";
}

// Add a function symbol to the symbol table
void addFunctionSymbol(const char *name, int returnType, int *paramTypes, int paramCount) {
    // In a real implementation, you would store all this information
    // For now, just add the function name to the symbol table
    addSymbol(name, 0);
}

// Get the type of a symbol
int getSymbolType(const char *name) {
    for (int i = 0; i < symCount; i++) {
        if (strcmp(symTable[i].name, name) == 0) {
            return symTable[i].type;
        }
    }
    printf("Error: Symbol '%s' not found\n", name);
    exit(1);
}
