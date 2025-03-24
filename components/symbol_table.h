// symbol_table.h
#include "tokens.h"
#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#define MAX_SYMBOLS 100

// Define the symbol structure
typedef struct {
    char name[50];
    int value;
} Symbol;

// Global symbol table and counter
extern Symbol symTable[MAX_SYMBOLS];
extern int symCount;

// Token pointer
extern Token *current;
// Functions related to symbol table (Optional)
void addSymbol(const char *name, int value);
int lookupSymbol(const char *name);

#endif // SYMBOL_TABLE_H
