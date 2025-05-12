#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdlib.h>

#define MAX_SYMBOLS 100
#define MAX_SYMBOL_NAME 50

// Symbol table entry
typedef struct {
    char name[MAX_SYMBOL_NAME];
    int value;
} Symbol;

// Global symbol table
extern Symbol symTable[MAX_SYMBOLS];
extern int symCount;

// Symbol table functions
void addSymbol(const char *name, int value);
int lookupSymbol(const char *name);

#endif // SYMBOL_TABLE_H
