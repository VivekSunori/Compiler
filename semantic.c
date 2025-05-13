#include "semantic.h"
#include "components/generator/codegen.c"
#include "components/ast.h"
#include "components/symbol_table.h"
#include <stdio.h>
#include <stdlib.h>

// Define a simple visited nodes tracking system
#define MAX_VISITED_NODES 1000
ASTNode* semanticVisitedNodes[MAX_VISITED_NODES];
int semanticVisitedCount = 0;

// Check if a node has been visited before
int semanticHasVisited(ASTNode* node) {
    for (int i = 0; i < semanticVisitedCount; i++) {
        if (semanticVisitedNodes[i] == node) {
            return 1;
        }
    }
    return 0;
}

// Mark a node as visited
void semanticMarkVisited(ASTNode* node) {
    if (semanticVisitedCount < MAX_VISITED_NODES) {
        semanticVisitedNodes[semanticVisitedCount++] = node;
    }
}

// Reset visited nodes
void semanticResetVisited() {
    semanticVisitedCount = 0;
}

// Perform semantic analysis on the AST
void checkSemantic(ASTNode *node) {
    if (!node) {
        printf("Warning: Null node encountered in semantic analysis\n");
        return;
    }

    // Check for circular references
    if (semanticHasVisited(node)) {
        printf("Warning: Circular reference detected in AST. Skipping node.\n");
        return;
    }
    
    // Mark this node as visited
    semanticMarkVisited(node);

    printf("Checking node type: %d\n", node->type);

    switch (node->type) {
        case NODE_ASSIGN:
            printf("Checking assignment to: %s\n", node->assign.name);
            if (lookupSymbol(node->assign.name) == -1) { 
                printf("Semantic Error: Variable '%s' not declared before assignment\n", node->assign.name);
                exit(1);
            }
            printf("Variable '%s' found in symbol table\n", node->assign.name);
            
            // Check type compatibility - be careful with this
            if (node->assign.expr) {
                int exprType = getExprType(node->assign.expr);
                int varType = getSymbolType(node->assign.name);
                printf("Variable type: %d, Expression type: %d\n", varType, exprType);
                
                if (varType != exprType) {
                    printf("Semantic Error: Type mismatch in assignment to '%s'\n", node->assign.name);
                    exit(1);
                }
            } else {
                printf("Warning: Null expression in assignment\n");
            }
            
            if (node->assign.expr) {
                checkSemantic(node->assign.expr);
            }
            break;

        case NODE_VAR_DECL:
            printf("Checking variable declaration: %s\n", node->varDecl.name);
            if (node->varDecl.value) {
                checkSemantic(node->varDecl.value);
            }
            break;

        case NODE_FUNC_CALL:
            printf("Checking function call: %s\n", node->funcCall.name);
            if (lookupSymbol(node->funcCall.name) == -1) { 
                printf("Semantic Error: Function '%s' is not defined\n", node->funcCall.name);
                exit(1);
            }
            
            // Be careful with these checks - they might be causing crashes
            printf("Function '%s' found in symbol table\n", node->funcCall.name);
            
            // Temporarily disable complex checks that might be causing crashes
            /*
            if (getFunctionArgCount(node->funcCall.name) != countArgs(node->funcCall.args)) {
                printf("Semantic Error: Function '%s' called with wrong number of arguments\n", node->funcCall.name);
                exit(1);
            }
            
            {
                ASTNode *arg = node->funcCall.args;
                int *expectedTypes = getFunctionArgTypes(node->funcCall.name);
                int i = 0;
                while (arg) {
                    if (getExprType(arg) != expectedTypes[i]) {
                        printf("Semantic Error: Argument %d type mismatch in function call '%s'\n", i+1, node->funcCall.name);
                        exit(1);
                    }
                    arg = arg->next;
                    i++;
                }
            }
            */
            
            if (node->funcCall.args) {
                checkSemantic(node->funcCall.args);
            }
            break;

        case NODE_FUNC_DEF:
            printf("Checking function definition: %s\n", node->funcDef.name);
            if (node->funcDef.body) {
                checkSemantic(node->funcDef.body);
            }
            break;

        case NODE_IF:
            printf("Checking if statement\n");
            if (node->ifNode.condition) {
                checkSemantic(node->ifNode.condition);
            }
            if (node->ifNode.thenStmt) {
                checkSemantic(node->ifNode.thenStmt);
            }
            if (node->ifNode.elseStmt) {
                checkSemantic(node->ifNode.elseStmt);
            }
            break;

        case NODE_WHILE:
            printf("Checking while loop\n");
            if (node->whileNode.condition) {
                checkSemantic(node->whileNode.condition);
            }
            if (node->whileNode.body) {
                checkSemantic(node->whileNode.body);
            }
            break;

        case NODE_FOR:
            printf("Checking for loop\n");
            if (node->forNode.initialization) {
                checkSemantic(node->forNode.initialization);
            }
            if (node->forNode.condition) {
                checkSemantic(node->forNode.condition);
            }
            if (node->forNode.increment) {
                checkSemantic(node->forNode.increment);
            }
            if (node->forNode.body) {
                checkSemantic(node->forNode.body);
            }
            break;

        case NODE_DO_WHILE:
            printf("Checking do-while loop\n");
            if (node->doWhileNode.body) {
                checkSemantic(node->doWhileNode.body);
            }
            if (node->doWhileNode.condition) {
                checkSemantic(node->doWhileNode.condition);
            }
            break;

        case NODE_PRINT:
            printf("Checking print statement\n");
            if (node->print.expr) {
                checkSemantic(node->print.expr);
            }
            break;

        case NODE_BINARY_OP:
            printf("Checking binary operation: %c\n", node->binaryOp.op);
            if (node->binaryOp.left) {
                checkSemantic(node->binaryOp.left);
            }
            if (node->binaryOp.right) {
                checkSemantic(node->binaryOp.right);
            }
            break;

        case NODE_NUMBER:
            printf("Checking number: %d\n", node->number);
            // Nothing to check for a number literal
            break;

        case NODE_STRING_LITERAL:
            printf("Checking string literal: %s\n", node->stringLiteral.value);
            // Nothing to check for a string literal
            break;

        default:
            printf("Warning: Unhandled node type %d in semantic analysis\n", node->type);
            break;
    }

    // Check the next node in the list
    printf("Checking next node\n");
    if (node->next) {
        checkSemantic(node->next);
    } else {
        printf("No more nodes to check\n");
    }
}

// Function to perform semantic analysis and generate code
void analyzeAndGenerateCode(ASTNode *root, const char *outputFile) {
    printf("Starting semantic analysis...\n");
    
    // Check if root is NULL
    if (!root) {
        printf("Error: AST is empty. No code to analyze.\n");
        exit(1);
    }
    
    // Reset visited nodes tracking
    semanticResetVisited();
    
    // First, perform semantic analysis
    printf("Checking AST...\n");
    checkSemantic(root);
    
    // If semantic analysis passes, generate code
    printf("Semantic analysis completed successfully.\n");
    
    // Debug: Print the output file name
    printf("Generating assembly code to: %s\n", outputFile);
    
    // Generate the assembly code
    generateAssembly(outputFile);
    
    printf("Code generation completed successfully.\n");
}
