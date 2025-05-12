#include "symbol_table.h"
#include <string.h>
#include <stdio.h>

// Global symbol table
Symbol symTable[MAX_SYMBOLS];
int symCount = 0;

// Add a symbol to the symbol table
void addSymbol(const char *name, int value) {
    if (symCount >= MAX_SYMBOLS) {
        printf("Error: Symbol table full\n");
        exit(1);
    }
    
    strcpy(symTable[symCount].name, name);
    symTable[symCount].value = value;
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
