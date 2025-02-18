// symbol_table.c
#include "symbol_table.h"

// Initialize the symbol table and counter
Symbol symTable[MAX_SYMBOLS];
int symCount = 0;

// Token pointer
Token *current = NULL;


/**
 * @brief  Adds a symbol to the symbol table.
 * This function helps to add a symbol to the symbol table and to define its value.

 * @param name The name of the symbol to add
 * @param value The value of the symbol
 * @return void 
 */
void addSymbol(const char *name, int value) {
    if (symCount < MAX_SYMBOLS) {
        strncpy(symTable[symCount].name, name, 50);
        symTable[symCount].value = value;
        symCount++;
    }
}

/**
 * @brief  Looks up a symbol in the symbol table.
 * 
 * This function helps with semantic analysis by checking if a variable is 
 * defined in the symbol table. It returns the index of the symbol if found, 
 * otherwise, it returns -1.
 * 
 * @param  name The name of the symbol to look up.
 * @return The index of the symbol in the symbol table, or -1 if not found.
 */
int lookupSymbol(const char *name) {
    for (int i = 0; i < symCount; i++) {
        if (strcmp(symTable[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}
