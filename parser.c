// parser.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils/lex.yy.c"
#include "components/parsers/header/parser.h"
#include "components/parsers/parser.c"
#include "components/symbol_table.c"

/**
 * @brief  Parses all tokens in the token list.
 * 
 * This function helps with parsing by parsing all tokens in the token list. It can be a function definition, function call, or a statement.
 * 
 * @attention This function should be used only for debugging or testing purposes.
 *  
 */
void parseTokens() {
    current = head;
    while (current && current->type != END) {
        if (current->type == FUNC) functionDef();
        else if (current->type == CALL) functionCall();
        else statement();
    }
    printf("Parsing completed successfully.\n");
}



/**
 * @brief  Tokenizes the source code file.
 * 
 * This function helps with tokenizing by tokenizing the source code file.
 */
void tokenize(FILE *file)
{
    if (!file)
    {
        perror("Failed to open source file");
        exit(1);
    }

    rewind(file); // Reset file pointer for lexing

    yyin = file;
    tokenizer();
    printf("\nTokenizing completed!\n");
    /*
    For debug purposes only
    // Print tokens
    Token *temp = head;
    if (!temp) {
        printf("No tokens generated!\n");
    }
    while (temp) {
        printf("[TOKEN: | VALUE: %s]\n",  temp->value);
        temp = temp->next;
    }
    */
}
