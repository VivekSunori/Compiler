#include "codegen.h"
#include "../ast.h"
#include "../symbol_table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// External reference to the AST head
extern ASTNode* astHead;

// Define a simple visited nodes tracking system for code generation
#define MAX_CODEGEN_VISITED_NODES 1000
static ASTNode* codegenVisitedNodes[MAX_CODEGEN_VISITED_NODES];
static int codegenVisitedCount = 0;

// Check if a node has been visited before during code generation
static int codegenHasVisited(ASTNode* node) {
    for (int i = 0; i < codegenVisitedCount; i++) {
        if (codegenVisitedNodes[i] == node) {
            return 1;
        }
    }
    return 0;
}

// Mark a node as visited during code generation
static void codegenMarkVisited(ASTNode* node) {
    if (codegenVisitedCount < MAX_CODEGEN_VISITED_NODES) {
        codegenVisitedNodes[codegenVisitedCount++] = node;
    }
}

// Reset visited nodes for code generation
void codegenResetVisited() {
    codegenVisitedCount = 0;
}

// String literal handling
#define MAX_STRING_LITERALS 100
static char stringLiterals[MAX_STRING_LITERALS][256];
static int stringLiteralCount = 0;

// Reset string literals
static void resetStringLiterals() {
    stringLiteralCount = 0;
}

// Add a string literal and return its index
static int addStringLiteral(const char* str) {
    if (stringLiteralCount < MAX_STRING_LITERALS) {
        strcpy(stringLiterals[stringLiteralCount], str);
        return stringLiteralCount++;
    }
    return -1;
}

void generateCode(ASTNode *node, FILE *asmFile)
{
    if (!node) {
        printf("Warning: Null node in generateCode\n");
        return;
    }

    // Check for circular references
    if (codegenHasVisited(node)) {
        printf("Warning: Circular reference detected in AST during code generation. Skipping node %p\n", (void*)node);
        return;
    }
    
    // Mark this node as visited
    codegenMarkVisited(node);

    printf("Generating code for node type: %d\n", node->type);

    switch (node->type)
    {
    case NODE_NUMBER:
        printf("Generating code for number: %d\n", node->number);
        if (asmFile) {
            fprintf(asmFile, "    mov rax, %d\n", node->number);
        }
        break;

    case NODE_VAR_REF:
        printf("Generating code for variable reference: %s\n", node->varRef.name);
        if (asmFile) {
            fprintf(asmFile, "    mov rax, [%s]\n", node->varRef.name);
        }
        break;

    case NODE_BINARY_OP:
        printf("Generating code for binary operation: %c\n", node->binaryOp.op);
        
        // Special case for decrement (x = x - 1)
        if (node->binaryOp.op == '-' && 
            node->binaryOp.right->type == NODE_NUMBER && 
            node->binaryOp.right->number == 1 &&
            node->binaryOp.left->type == NODE_VAR_REF) {
            
            // Generate more efficient code for x = x - 1
            fprintf(asmFile, "    ; Optimized decrement\n");
            fprintf(asmFile, "    mov rax, [%s]\n", node->binaryOp.left->varRef.name);
            fprintf(asmFile, "    dec rax\n");
            fprintf(asmFile, "    mov [%s], rax\n", node->binaryOp.left->varRef.name);
            return; // Important: return to prevent further processing
        } 
        
        // Regular binary operation
        fprintf(asmFile, "    ; Regular binary operation\n");
        generateCode(node->binaryOp.right, asmFile);
        fprintf(asmFile, "    push rax\n");
        generateCode(node->binaryOp.left, asmFile);
        fprintf(asmFile, "    pop rbx\n");
        
        switch (node->binaryOp.op) {
            case '+':
                fprintf(asmFile, "    add rax, rbx\n");
                break;
            case '-':
                fprintf(asmFile, "    sub rax, rbx\n");
                break;
            case '*':
                fprintf(asmFile, "    imul rax, rbx\n");
                break;
            case '/':
                fprintf(asmFile, "    mov rcx, rbx\n");  // Save divisor in rcx
                fprintf(asmFile, "    mov rax, rdx\n");  // Move dividend to rax
                fprintf(asmFile, "    xor rdx, rdx\n");  // Clear rdx for division
                fprintf(asmFile, "    idiv rcx\n");      // Divide rax by rcx
                break;
            default:
                printf("Error: Unknown binary operator %c\n", node->binaryOp.op);
                break;
        }
        break;

    case NODE_VAR_DECL:
        printf("Generating code for variable declaration: %s\n", node->varDecl.name);
        
        // If the value is a string literal, handle it specially
        if (node->varDecl.value->type == NODE_STRING_LITERAL) {
            printf("Variable %s is a string literal\n", node->varDecl.name);
            // Generate code for the string literal (this will put its address in rax)
            generateCode(node->varDecl.value, asmFile);
            // Store the string address in the variable
            if (asmFile) {
                fprintf(asmFile, "    mov [%s], rax\n", node->varDecl.name);
            }
        } else {
            // For numeric values, generate code as before
            generateCode(node->varDecl.value, asmFile);
            if (asmFile) {
                fprintf(asmFile, "    mov [%s], rax\n", node->varDecl.name);
            }
        }
        break;

    case NODE_ASSIGN:
        printf("Generating code for assignment: %s\n", node->assign.name);
        generateCode(node->assign.expr, asmFile);
        if (asmFile) {
            fprintf(asmFile, "    mov [%s], rax\n", node->assign.name);
        }
        break;

    case NODE_PRINT:
        printf("Generating code for print statement\n");
        generateCode(node->print.expr, asmFile);
        
        // Determine the type of expression to print
        if (node->print.expr->type == NODE_NUMBER || 
            (node->print.expr->type == NODE_VAR_REF && 
             getSymbolType(node->print.expr->varRef.name) == TYPE_NUMBER)) {
            fprintf(asmFile, "    mov rdi, rax\n");
            fprintf(asmFile, "    call print_num\n");
        } else if (node->print.expr->type == NODE_BOOLEAN_LITERAL || 
                  (node->print.expr->type == NODE_VAR_REF && 
                   getSymbolType(node->print.expr->varRef.name) == TYPE_BOOLEAN)) {
            fprintf(asmFile, "    mov rdi, rax\n");
            fprintf(asmFile, "    call print_log\n");
        } else if (node->print.expr->type == NODE_STRING_LITERAL || 
                  (node->print.expr->type == NODE_VAR_REF && 
                   getSymbolType(node->print.expr->varRef.name) == TYPE_STRING)) {
            fprintf(asmFile, "    mov rdi, rax\n");
            fprintf(asmFile, "    call print_str\n");
        }
        
        // Always add a newline after printing
        fprintf(asmFile, "    mov rdi, 1\n");
        fprintf(asmFile, "    mov rsi, 10\n");
        fprintf(asmFile, "    push rsi\n");
        fprintf(asmFile, "    mov rsi, rsp\n");
        fprintf(asmFile, "    mov rdx, 1\n");
        fprintf(asmFile, "    mov rax, 1\n");
        fprintf(asmFile, "    syscall\n");
        fprintf(asmFile, "    add rsp, 8\n");
        break;

    case NODE_IF:
        printf("Generating code for if statement\n");
        generateCode(node->ifNode.condition, asmFile);
        
        // If there's no else branch, jump to the end if condition is false
        if (node->ifNode.elseStmt == NULL) {
            fprintf(asmFile, "    cmp rax, 0\n");
            fprintf(asmFile, "    je .endif_%p\n", (void *)node);
            generateCode(node->ifNode.thenStmt, asmFile);
            fprintf(asmFile, ".endif_%p:\n", (void *)node);
        } else {
            // If there is an else branch, handle both paths
            fprintf(asmFile, "    cmp rax, 0\n");
            fprintf(asmFile, "    je .else_%p\n", (void *)node);
            generateCode(node->ifNode.thenStmt, asmFile);
            fprintf(asmFile, "    jmp .endif_%p\n", (void *)node);
            fprintf(asmFile, ".else_%p:\n", (void *)node);
            generateCode(node->ifNode.elseStmt, asmFile);
            fprintf(asmFile, ".endif_%p:\n", (void *)node);
        }
        break;

    case NODE_WHILE:
        printf("Generating code for while loop\n");
        
        // Generate a unique label for this loop
        char startLabel[32], endLabel[32];
        sprintf(startLabel, ".loop_start_%p", (void *)node);
        sprintf(endLabel, ".loop_end_%p", (void *)node);
        
        // Loop start label
        fprintf(asmFile, "%s:\n", startLabel);
        
        // Optimize simple relational conditions
        if (node->whileNode.condition->type == NODE_RELATIONAL_OP) {
            ASTNode *left = node->whileNode.condition->relOp.left;
            ASTNode *right = node->whileNode.condition->relOp.right;
            const char *op = node->whileNode.condition->relOp.op;
            
            // Handle x > 0, x < 10, etc.
            if (left->type == NODE_VAR_REF && right->type == NODE_NUMBER) {
                fprintf(asmFile, "    mov rax, [%s]\n", left->varRef.name);
                fprintf(asmFile, "    cmp rax, %d\n", right->number);
                
                // Generate appropriate jump based on the operator
                if (strcmp(op, ">") == 0) {
                    fprintf(asmFile, "    jle %s\n", endLabel);
                } else if (strcmp(op, "<") == 0) {
                    fprintf(asmFile, "    jge %s\n", endLabel);
                } else if (strcmp(op, ">=") == 0) {
                    fprintf(asmFile, "    jl %s\n", endLabel);
                } else if (strcmp(op, "<=") == 0) {
                    fprintf(asmFile, "    jg %s\n", endLabel);
                } else if (strcmp(op, "==") == 0) {
                    fprintf(asmFile, "    jne %s\n", endLabel);
                } else if (strcmp(op, "!=") == 0) {
                    fprintf(asmFile, "    je %s\n", endLabel);
                }
                
                // Generate body code
                generateCode(node->whileNode.body, asmFile);
                
                // Jump back to start
                fprintf(asmFile, "    jmp %s\n", startLabel);
                fprintf(asmFile, "%s:\n", endLabel);
                
                break;  // Exit the switch case
            }
        }
        
        // If not a simple relational condition, use the general approach
        // but still with simplified logic
        fprintf(asmFile, "    ; Evaluating condition\n");
        generateCode(node->whileNode.condition, asmFile);
        fprintf(asmFile, "    cmp rax, 0\n");
        fprintf(asmFile, "    je %s\n", endLabel);
        
        fprintf(asmFile, "    ; Loop body\n");
        generateCode(node->whileNode.body, asmFile);
        
        fprintf(asmFile, "    jmp %s\n", startLabel);
        fprintf(asmFile, "%s:\n", endLabel);
        break;

    case NODE_FOR:
        printf("Generating code for for loop\n");
        
        // Generate initialization code
        if (node->forNode.initialization) {
            generateCode(node->forNode.initialization, asmFile);
        }
        
        // Loop start label
        fprintf(asmFile, ".for_loop_start_%p:\n", (void *)node);
        
        // Generate condition code
        if (node->forNode.condition) {
            generateCode(node->forNode.condition, asmFile);
            fprintf(asmFile, "    cmp rax, 0\n");
            fprintf(asmFile, "    je .for_loop_end_%p\n", (void *)node);
        }
        
        // Generate body code
        if (node->forNode.body) {
            generateCode(node->forNode.body, asmFile);
        }
        
        // Generate increment code
        if (node->forNode.increment) {
            generateCode(node->forNode.increment, asmFile);
        }
        
        // Jump back to condition
        fprintf(asmFile, "    jmp .for_loop_start_%p\n", (void *)node);
        
        // Loop end label
        fprintf(asmFile, ".for_loop_end_%p:\n", (void *)node);
        break;

    case NODE_DO_WHILE:
        printf("Generating code for do-while loop\n");
        
        // Loop start label
        fprintf(asmFile, ".do_while_start_%p:\n", (void *)node);
        
        // Generate body code
        if (node->doWhileNode.body) {
            generateCode(node->doWhileNode.body, asmFile);
        }
        
        // Generate condition code
        if (node->doWhileNode.condition) {
            generateCode(node->doWhileNode.condition, asmFile);
            fprintf(asmFile, "    cmp rax, 0\n");
            fprintf(asmFile, "    jne .do_while_start_%p\n", (void *)node);
        }
        break;

    case NODE_LOGICAL_OP:
        printf("Generating code for logical op: %s\n", node->logicalOp.op);
        // Generate code for the right operand first (will be on stack)
        generateCode(node->logicalOp.right, asmFile);
        fprintf(asmFile, "    push rax\n");  // Save right operand on stack
        
        // Generate code for the left operand (will be in rax)
        generateCode(node->logicalOp.left, asmFile);
        
        // Pop right operand into rbx
        fprintf(asmFile, "    pop rbx\n");
        
        // Perform the operation
        if (strcmp(node->logicalOp.op, "&&") == 0) {
            fprintf(asmFile, "    and rax, rbx\n");
        } else if (strcmp(node->logicalOp.op, "||") == 0) {
            fprintf(asmFile, "    or rax, rbx\n");
        } else {
            printf("Error: Unknown logical operator %s\n", node->logicalOp.op);
        }
        break;

    case NODE_RELATIONAL_OP:
        printf("Generating code for relational op: %s\n", node->relOp.op);
        // Generate code for the right operand first (will be on stack)
        generateCode(node->relOp.right, asmFile);
        fprintf(asmFile, "    push rax\n");  // Save right operand on stack
        
        // Generate code for the left operand (will be in rax)
        generateCode(node->relOp.left, asmFile);
        
        // Pop right operand into rbx
        fprintf(asmFile, "    pop rbx\n");
        
        // Compare left (rax) with right (rbx)
        fprintf(asmFile, "    cmp rax, rbx\n");
        
        // Set result based on comparison
        if (strcmp(node->relOp.op, "==") == 0) {
            fprintf(asmFile, "    sete al\n");
        } else if (strcmp(node->relOp.op, "!=") == 0) {
            fprintf(asmFile, "    setne al\n");
        } else if (strcmp(node->relOp.op, "<") == 0) {
            fprintf(asmFile, "    setl al\n");
        } else if (strcmp(node->relOp.op, ">") == 0) {
            fprintf(asmFile, "    setg al\n");
        } else if (strcmp(node->relOp.op, "<=") == 0) {
            fprintf(asmFile, "    setle al\n");
        } else if (strcmp(node->relOp.op, ">=") == 0) {
            fprintf(asmFile, "    setge al\n");
        } else {
            printf("Error: Unknown relational operator %s\n", node->relOp.op);
        }
        
        // Zero-extend the result to 64 bits
        fprintf(asmFile, "    movzx rax, al\n");
        break;

    case NODE_STRING_LITERAL:
        printf("Generating code for string literal: %s\n", node->stringLiteral.value);
        
        // Add the string to our string literals array if it's not already there
        int strIndex = -1;
        for (int i = 0; i < stringLiteralCount; i++) {
            if (strcmp(stringLiterals[i], node->stringLiteral.value) == 0) {
                strIndex = i;
                printf("Found existing string literal at index %d\n", strIndex);
                break;
            }
        }
        if (strIndex == -1) {
            // Add new string literal
            strIndex = stringLiteralCount;
            strcpy(stringLiterals[stringLiteralCount], node->stringLiteral.value);
            stringLiteralCount++;
            printf("Added string literal: '%s' at index %d\n", node->stringLiteral.value, strIndex);
        }
        
        // Load the address of the string - don't add the string here
        if (asmFile) {
            fprintf(asmFile, "    lea rax, [rel str_%d]\n", strIndex);
        }
        break;

    case NODE_BOOLEAN_LITERAL:
        if (asmFile) {
            if (strcmp(node->booleanLiteral.value, "true") == 0) {
                fprintf(asmFile, "    mov rax, 1\n");
            } else {
                fprintf(asmFile, "    mov rax, 0\n");
            }
        }
        break;

    default:
        printf("Warning: Unhandled node type %d in code generation\n", node->type);
        break;
    }

    // Process the next node in the list
    if (node->next) {
        printf("Processing next node\n");
        generateCode(node->next, asmFile);
    } else {
        printf("No more nodes to process\n");
    }
}

void generateAssembly(const char *filename)
{
    resetStringLiterals();
    printf("Opening file for writing: %s\n", filename);
    
    // Try to open the file with full path
    FILE *asmFile = fopen(filename, "w");
    if (!asmFile)
    {
        printf("Error: Cannot open ASM file '%s'\n", filename);
        perror("fopen");
        exit(1);
    }
    
    printf("File opened successfully\n");

    // Debug information
    printf("Symbol count: %d\n", symCount);
    printf("AST head address: %p\n", (void*)astHead);

    // === FIRST PASS: COLLECT string literals and symbols ===
    if (astHead != NULL) {
        printf("Collecting data by generating code once...\n");
        codegenResetVisited();
        generateCode(astHead, NULL); // null FILE pointer, just collect
        printf("First pass completed, collected %d string literals\n", stringLiteralCount);
    }

    // === GENERATE ASSEMBLY FILE ===
    // Data Section
    fprintf(asmFile, "section .data\n");
    
    // Add variables from symbol table
    for (int i = 0; i < symCount; i++) {
        fprintf(asmFile, "    %s: dq 0\n", symTable[i].name);
    }
    
    // Add string literals
    for (int i = 0; i < stringLiteralCount; i++) {
        fprintf(asmFile, "    str_%d: db '%s', 0\n", i, stringLiterals[i]);
    }
    
    fprintf(asmFile, "\n");

    // Text Section
    fprintf(asmFile, "section .text\n");
    fprintf(asmFile, "    global _start\n");

    // Add utility functions for printing
    fprintf(asmFile, "\n; Utility function to print a number\n");
    fprintf(asmFile, "print_num:\n");
    fprintf(asmFile, "    push rbp\n");
    fprintf(asmFile, "    mov rbp, rsp\n");
    fprintf(asmFile, "    sub rsp, 32         ; Reserve space for buffer\n");
    fprintf(asmFile, "    mov rcx, 0          ; Counter for digits\n");
    fprintf(asmFile, "    mov rax, rdi        ; Number to print\n");
    fprintf(asmFile, "    mov r8, 10          ; Base 10\n");
    fprintf(asmFile, "    mov r9, rsp         ; Buffer pointer\n");
    fprintf(asmFile, "    ; Handle negative numbers\n");
    fprintf(asmFile, "    cmp rax, 0\n");
    fprintf(asmFile, "    jge .positive\n");
    fprintf(asmFile, "    neg rax\n");
    fprintf(asmFile, "    push rax\n");
    fprintf(asmFile, "    mov rax, '-'\n");
    fprintf(asmFile, "    push rax\n");
    fprintf(asmFile, "    mov rax, 1          ; sys_write\n");
    fprintf(asmFile, "    mov rdi, 1          ; stdout\n");
    fprintf(asmFile, "    mov rsi, rsp        ; pointer to '-'\n");
    fprintf(asmFile, "    mov rdx, 1          ; length\n");
    fprintf(asmFile, "    syscall\n");
    fprintf(asmFile, "    add rsp, 8          ; Clean up stack\n");
    fprintf(asmFile, "    pop rax\n");
    fprintf(asmFile, ".positive:\n");
    fprintf(asmFile, "    ; Convert number to string (reversed)\n");
    fprintf(asmFile, "    cmp rax, 0\n");
    fprintf(asmFile, "    jne .convert\n");
    fprintf(asmFile, "    mov byte [r9], '0'  ; Special case for 0\n");
    fprintf(asmFile, "    inc r9\n");
    fprintf(asmFile, "    inc rcx\n");
    fprintf(asmFile, "    jmp .print\n");
    fprintf(asmFile, ".convert:\n");
    fprintf(asmFile, "    cmp rax, 0\n");
    fprintf(asmFile, "    je .print\n");
    fprintf(asmFile, "    xor rdx, rdx\n");
    fprintf(asmFile, "    div r8              ; rax = rax / 10, rdx = remainder\n");
    fprintf(asmFile, "    add rdx, '0'        ; Convert to ASCII\n");
    fprintf(asmFile, "    mov byte [r9], dl   ; Store in buffer\n");
    fprintf(asmFile, "    inc r9\n");
    fprintf(asmFile, "    inc rcx\n");
    fprintf(asmFile, "    jmp .convert\n");
    fprintf(asmFile, ".print:\n");
    fprintf(asmFile, "    ; Print the number (reversed buffer)\n");
    fprintf(asmFile, "    mov rax, 1          ; sys_write\n");
    fprintf(asmFile, "    mov rdi, 1          ; stdout\n");
    fprintf(asmFile, "    lea rsi, [r9-1]     ; End of buffer\n");
    fprintf(asmFile, ".print_loop:\n");
    fprintf(asmFile, "    mov rdx, 1          ; length\n");
    fprintf(asmFile, "    syscall\n");
    fprintf(asmFile, "    dec rsi             ; Move to previous character\n");
    fprintf(asmFile, "    dec rcx             ; Decrement counter\n");
    fprintf(asmFile, "    cmp rcx, 0\n");
    fprintf(asmFile, "    jg .print_loop\n");
    fprintf(asmFile, "    mov rsp, rbp\n");
    fprintf(asmFile, "    pop rbp\n");
    fprintf(asmFile, "    ret\n\n");

    fprintf(asmFile, "; Utility function to print a boolean\n");
    fprintf(asmFile, "print_log:\n");
    fprintf(asmFile, "    push rbp\n");
    fprintf(asmFile, "    mov rbp, rsp\n");
    fprintf(asmFile, "    cmp rdi, 0\n");
    fprintf(asmFile, "    je .print_false\n");
    fprintf(asmFile, "    ; Print 'true'\n");
    fprintf(asmFile, "    mov rax, 1          ; sys_write\n");
    fprintf(asmFile, "    mov rdi, 1          ; stdout\n");
    fprintf(asmFile, "    lea rsi, [rel true_str]\n");
    fprintf(asmFile, "    mov rdx, 4          ; length of 'true'\n");
    fprintf(asmFile, "    syscall\n");
    fprintf(asmFile, "    jmp .done\n");
    fprintf(asmFile, ".print_false:\n");
    fprintf(asmFile, "    ; Print 'false'\n");
    fprintf(asmFile, "    mov rax, 1          ; sys_write\n");
    fprintf(asmFile, "    mov rdi, 1          ; stdout\n");
    fprintf(asmFile, "    lea rsi, [rel false_str]\n");
    fprintf(asmFile, "    mov rdx, 5          ; length of 'false'\n");
    fprintf(asmFile, "    syscall\n");
    fprintf(asmFile, ".done:\n");
    fprintf(asmFile, "    mov rsp, rbp\n");
    fprintf(asmFile, "    pop rbp\n");
    fprintf(asmFile, "    ret\n\n");

    fprintf(asmFile, "; Utility function to print a string\n");
    fprintf(asmFile, "print_str:\n");
    fprintf(asmFile, "    push rbp\n");
    fprintf(asmFile, "    mov rbp, rsp\n");
    fprintf(asmFile, "    mov r12, rdi        ; Save string pointer\n");
    fprintf(asmFile, "    ; Calculate string length\n");
    fprintf(asmFile, "    mov rcx, -1         ; Start counter at -1\n");
    fprintf(asmFile, ".strlen_loop:\n");
    fprintf(asmFile, "    inc rcx\n");
    fprintf(asmFile, "    mov al, [r12+rcx]   ; Load character\n");
    fprintf(asmFile, "    test al, al         ; Check for null terminator\n");
    fprintf(asmFile, "    jnz .strlen_loop\n");
    fprintf(asmFile, "    ; Print the string\n");
    fprintf(asmFile, "    mov rax, 1          ; sys_write\n");
    fprintf(asmFile, "    mov rdi, 1          ; stdout\n");
    fprintf(asmFile, "    mov rsi, r12        ; string pointer\n");
    fprintf(asmFile, "    mov rdx, rcx        ; string length\n");
    fprintf(asmFile, "    syscall\n");
    fprintf(asmFile, "    mov rsp, rbp\n");
    fprintf(asmFile, "    pop rbp\n");
    fprintf(asmFile, "    ret\n\n");

    // Add string constants for boolean values
    fprintf(asmFile, "section .data\n");
    fprintf(asmFile, "    true_str: db 'true', 0\n");
    fprintf(asmFile, "    false_str: db 'false', 0\n");
    fprintf(asmFile, "section .text\n");

    // Generate function definitions first
    if (astHead != NULL) {
        ASTNode *node = astHead;
        while (node) {
            if (node->type == NODE_FUNC_DEF) {
                fprintf(asmFile, "%s:\n", node->funcDef.name);
                fprintf(asmFile, "    push rbp\n");
                fprintf(asmFile, "    mov rbp, rsp\n");
                
                // Generate code for function body
                if (node->funcDef.body) {
                    generateCode(node->funcDef.body, asmFile);
                }
                
                // Return from function
                fprintf(asmFile, "    mov rsp, rbp\n");
                fprintf(asmFile, "    pop rbp\n");
                fprintf(asmFile, "    ret\n\n");
            }
            node = node->next;
        }
    }

    // Start of program
    fprintf(asmFile, "_start:\n");

    // === SECOND PASS: Generate actual code ===
    if (astHead != NULL) {
        printf("Generating code from AST (second pass)...\n");
        codegenResetVisited(); // reset again
        generateCode(astHead, asmFile);
        printf("Code generation from AST completed\n");
    } else {
        printf("Warning: AST head is NULL, no code generated\n");
    }

    // Exit system call
    fprintf(asmFile, "    mov rax, 60\n");   // sys_exit
    fprintf(asmFile, "    xor rdi, rdi\n");  // Exit code 0
    fprintf(asmFile, "    syscall\n");

    // Flush the file buffer to ensure all data is written
    fflush(asmFile);
    
    printf("Closing file...\n");
    if (fclose(asmFile) != 0) {
        printf("Error: Failed to close file\n");
        perror("fclose");
        exit(1);
    }
    
    printf("File closed successfully\n");
    printf("Assembly file generated at: %s\n", filename);

    // Debug output
    printf("String literal count: %d\n", stringLiteralCount);
    for (int i = 0; i < stringLiteralCount; i++) {
        printf("String literal %d: '%s'\n", i, stringLiterals[i]);
    }

    if (stringLiteralCount > 0) {
        printf("String literals were added to the data section.\n");
    } else {
        printf("Warning: No string literals were added to the data section.\n");
    }

    // Check if the file exists and is not empty
    FILE *checkFile = fopen(filename, "r");
    if (checkFile) {
        fseek(checkFile, 0, SEEK_END);
        long size = ftell(checkFile);
        fclose(checkFile);
        printf("Assembly file size: %ld bytes\n", size);
        
        if (size < 10) {
            printf("Warning: Assembly file is very small or empty!\n");
        }
    } else {
        printf("Warning: Could not open assembly file for checking!\n");
    }
}
