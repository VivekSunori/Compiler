#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#define MAX_SYMBOLS 100
#define MAX_VAR_NAME_LENGTH 50

// Define variable types
typedef enum {
    TYPE_UNKNOWN = -1,
    TYPE_NUMBER,
    TYPE_STRING,
    TYPE_BOOLEAN
} VariableType;

typedef struct {
    char name[MAX_VAR_NAME_LENGTH];
    int value;
    VariableType type;  // Add type field to store variable type
} Symbol;

extern Symbol symTable[MAX_SYMBOLS];
extern int symCount;

void initSymbolTable();
int insertSymbol(const char* name, int value, VariableType type);
int lookupSymbol(const char* name);
int getSymbolValue(const char* name);
VariableType getSymbolType(const char* name);  // Add function to get symbol type

#endif // SYMBOL_TABLE_H
