#include <stdio.h>
#include "./components/tokens.h"
#include "utils/lex.yy.c"

Token *current = NULL;

void nextToken()
{
    if (current)
        current = current->next;
}

void match(TokenType expected)
{
    if (current && current->type == expected)
    {
        nextToken();
    }
    else
    {
        printf("Syntax Error: Unexpected token '%s'\n", current ? current->value : "NULL");
        exit(1);
    }
}

void factor()
{
    if (current && (current->type == NUMBER || current->type == ID))
    {
        nextToken();
    }
    else
    {
        printf("Error: Expected factor but got '%s'\n", current ? current->value : "NULL");
        exit(1);
    }
}

void term()
{
    factor();
    while (current && current->type == OPERATOR && (current->value[0] == '*' || current->value[0] == '/'))
    {
        nextToken();
        factor();
    }
}

void expression()
{
    term();
    while (current && current->type == OPERATOR && (current->value[0] == '+' || current->value[0] == '-'))
    {
        nextToken();
        term();
    }
}

void statement()
{
    current = head;
    if (current && current->type == ID)
    {
        nextToken();
        match(ASSIGN);
        expression();
    }
    else
    {
        printf("Error: Expected identifier at start of statement\n");
        exit(1);
    }
}

void parseTokens()
{
    current = head;
    while (current && current->type != END)
    {
        statement();
    }
    printf("Parsing completed successfully.\n");
}

void tokenize(FILE *file)
{
    if (!file)
    {
        perror("Failed to open source file");
        exit(1);
    }

    rewind(file); // Reset file pointer for lexing

    yyin = file;
    yylex();
    printf("\nTokenizing completed!\n");
    /*
    FOr debug purposes only
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
