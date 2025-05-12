#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils/help.c"
#include "components/symbol_table.h"
#include "parser.h"
#include "components/parsers/header/parser.h"
#include "components/ast_visualizer.h"
#include "components/ast_json_exporter.h"

struct Stack;

extern Token* current;
extern Token* head;
extern void tokenize(FILE *file);
extern ASTNode* statement();
extern void freeArena();
extern struct Stack bracesStack;
extern struct Stack parenStack;
extern int isEmpty(struct Stack *stack);

/**
 * @brief Compiles the given file.
 * 
 * @param filename The name of the file to compile.
 * @return void
 */
void compileFile(const char* filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Could not open file '%s'\n", filename);
        exit(1);
    }

    char source[1000] = {0};
    char line[100];
    while (fgets(line, sizeof(line), file)) {
        strcat(source, line);
    }

    tokenize(file);
    fclose(file);

    current = head;
    int statementCount = 0;
    
    #define MAX_STATEMENTS 100
    ASTNode* statements[MAX_STATEMENTS];
    
    while (current && current->type != END && statementCount < MAX_STATEMENTS) {
        ASTNode* node = statement();
        
        printf("\n--- AST for statement ---\n");
        visualizeASTRoot(node);
        printf("-------------------------\n\n");
    
        statements[statementCount++] = node;
        
        // nextToken();
    }

    // Check if all braces and parentheses are properly closed
    if (!isEmpty(&bracesStack)) {
        printf("Syntax Error: Unclosed braces '{' detected\n");
        exit(1);
    }
    if (!isEmpty(&parenStack)) {
        printf("Syntax Error: Unclosed parentheses '(' detected\n");
        exit(1);
    }
    
    // Export all ASTs to a single JSON file
    exportASTsToSingleJSON(statements, statementCount, "ast_all_statements.json");

    printf("\nCompilation completed successfully.\n");
    freeArena(); 
}


int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <filename.cx>\n", argv[0]);
        return 1;
    }
    if (strcmp(argv[1], "-help") == 0 || strcmp(argv[1], "--help") == 0 || 
        strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--h") == 0 || 
        strcmp(argv[1], "-H") == 0 || strcmp(argv[1], "--H") == 0) {
        help();
        exit(0);
    }
    
    char *filename = argv[1];
    char *dot = strrchr(filename, '.'); 

    if (dot == NULL || strcmp(dot, ".cx") != 0) {
        printf("Error: Invalid file extension\n");
        exit(1);
    }

    compileFile(argv[1]);

    return 0;
}
