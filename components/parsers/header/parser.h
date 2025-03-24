#ifndef PARSER_H
#define PARSER_H

#include "../../ast.h"
#include "../../symbol_table.h"
#include "../../tokens.h"

// Function declarations
void nextToken();
void match(TokenType expected);
ASTNode* factor();
ASTNode* term();
ASTNode* parseExpression(1);
ASTNode* statement();
ASTNode* conditional();
ASTNode* functionDef();
ASTNode* functionCall();
void parseTokens();
ASTNode* loop();

#endif // PARSER_H
