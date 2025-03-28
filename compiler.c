// compiler.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "semantic.c"
#include "utils/help.c"

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
    while (current && current->type != END) {
        statement();
    }

    printf("\nCompilation completed successfully.\n");
    freeArena(); // Free memory allocated by the arena allocator (Testing purposes only, use after compilation is complete and assembly is generated) 
}


int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <filename.cx>\n", argv[0]);
        return 1;
    }
    if (strcmp(argv[1], "-help") == 0 || strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--h") == 0 || strcmp(argv[1], "-H") == 0 || strcmp(argv[1], "--H") == 0) {
        help();
        exit(0);
    }
    
    // Extract the file extension
    char *filename = argv[1];
    char *dot = strrchr(filename, '.'); 

    if (dot == NULL || strcmp(dot, ".cx") != 0) {
        printf("Error: Invalid file extension\n");
        exit(1);
    }

    compileFile(argv[1]);

    return 0;
}
