// symbol_table.h
#include "tokens.h"
#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#define MAX_SYMBOLS 100
 
typedef struct {
    char name[50];
    int value;
} Symbol;
 
extern Symbol symTable[MAX_SYMBOLS];
extern int symCount;
 
extern Token *current; 
void addSymbol(const char *name, int value);
int lookupSymbol(const char *name);

#endif  
