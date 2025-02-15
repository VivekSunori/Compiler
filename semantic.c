// semantic.c
#include "parser.c"

typedef struct {
    char name[50];
    int value;
} Symbol;

Symbol symTable[100];
int symCount = 0;

void addSymbol(char *name, int value) {
    strcpy(symTable[symCount].name, name);
    symTable[symCount].value = value;
    symCount++;
}

int lookupSymbol(char *name) {
    for (int i = 0; i < symCount; i++) {
        if (strcmp(symTable[i].name, name) == 0) return i;
    }
    return -1;
}

