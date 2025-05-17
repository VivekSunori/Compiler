#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils/help.c"
#include "components/symbol_table.h"
#include "parser.h"
#include "components/parsers/header/parser.h"
#include "components/ast_visualizer.h"
#include "components/ast_json_exporter.h"
#include "semantic.h"

extern ASTNode *astHead;

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
    // Initialize the symbol table at the start of compilation
    initSymbolTable();
    
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
    ASTNode* prevNode = NULL;
    astHead = NULL;  // Reset AST head
    
    while (current && current->type != END_OF_FILE && statementCount < MAX_STATEMENTS) {
        ASTNode* node = statement();
        if (!node) {
            printf("Warning: statement() returned NULL\n");
            continue;
        }
        
        // Store the node in our array
        statements[statementCount++] = node;
        
        // If this is the first statement, set it as the AST head
        if (statementCount == 1) {
            astHead = node;
            printf("AST head set to node type: %d\n", node->type);
        } else if (prevNode) {
            // Link the previous node to this one
            prevNode->next = node;
            printf("Linked node %d to node %d\n", statementCount-2, statementCount-1);
        }
        
        // Update prevNode for the next iteration
        prevNode = node;
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
    
    // Generate output filename (replace .cx with .asm)
    char outputFile[256];
    strcpy(outputFile, filename);
    char *ext = strrchr(outputFile, '.');
    if (ext) {
        strcpy(ext, ".asm");
    } else {
        strcat(outputFile, ".asm");
    }
    
    // Print debug information
    printf("\nParsing completed. Starting semantic analysis...\n");
    
    // Check if astHead is NULL
    if (!astHead) {
        printf("Error: AST is empty. No code to analyze.\n");
        exit(1);
    } else {
        printf("AST head is not NULL, type: %d\n", astHead->type);
        
        // Debug: Print the first few nodes to verify the AST structure
        ASTNode* temp = astHead;
        int count = 0;
        while (temp && count < 5) {
            printf("Node %d: type %d\n", count, temp->type);
            temp = temp->next;
            count++;
        }
    }
    
    // Perform semantic analysis and code generation
    analyzeAndGenerateCode(astHead, outputFile);
    
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
