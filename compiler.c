// compiler.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "semantic.c"

void compileFile(const char* filename) {
    // Open the source file
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Could not open file '%s'\n", filename);
        exit(1);
    }

    // Read the file content
    char source[1000] = {0};
    char line[100];
    while (fgets(line, sizeof(line), file)) {
        strcat(source, line);
    }

    // Lexical Analysis
    tokenize(file);
    fclose(file);

    // Parsing
    current = head;
    while (current && current->type != END) {
        statement();
    }

    printf("\nCompilation completed successfully.\n");
}

void help(){
    printf("cmpx <filename.cx> - Compiles the given file.\n");
    printf("cmpx -help - Displays this help message.\n");
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <filename.cx>\n", argv[0]);
        return 1;
    }
    if(argv[1] == "-help"){
        help();
        exit(0);
    }

    compileFile(argv[1]);

    return 0;
}
