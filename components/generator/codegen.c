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
        printf("Generating code for binary op: %c\n", node->binaryOp.op);
        // Generate code for the right operand first (will be on stack)
        generateCode(node->binaryOp.right, asmFile);
        if (asmFile) {
            fprintf(asmFile, "    push rax\n");  // Save right operand on stack
        }
        
        // Generate code for the left operand (will be in rax)
        generateCode(node->binaryOp.left, asmFile);
        
        if (asmFile) {
            // Pop right operand into rbx
            fprintf(asmFile, "    pop rbx\n");
            
            // Perform the operation
            switch (node->binaryOp.op)
            {
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
        printf("Generating code for print\n");
        if (node->print.expr) {
            // Generate code for the expression to print
            generateCode(node->print.expr, asmFile);
            
            if (asmFile) {
                // Check the type of the expression
                if (node->print.expr->type == NODE_STRING_LITERAL || 
                    (node->print.expr->type == NODE_VAR_REF && 
                     getSymbolType(node->print.expr->varRef.name) == TYPE_STRING)) {
                    // For strings, call print_str
                    fprintf(asmFile, "    mov rdi, rax\n");
                    fprintf(asmFile, "    call print_str\n");
                } else if (node->print.expr->type == NODE_BOOLEAN_LITERAL || 
                          (node->print.expr->type == NODE_VAR_REF && 
                           getSymbolType(node->print.expr->varRef.name) == TYPE_BOOLEAN)) {
                    // For booleans, call print_log
                    fprintf(asmFile, "    mov rdi, rax\n");
                    fprintf(asmFile, "    call print_log\n");
                } else {
                    // For numbers, call print_num
                    fprintf(asmFile, "    mov rdi, rax\n");
                    fprintf(asmFile, "    call print_num\n");
                }
                
                // Print a newline
                fprintf(asmFile, "    mov rdi, 1\n");          // File descriptor: stdout
                fprintf(asmFile, "    mov rsi, 10\n");          // Newline character
                fprintf(asmFile, "    push rsi\n");             // Push newline onto stack
                fprintf(asmFile, "    mov rsi, rsp\n");         // Point to newline on stack
                fprintf(asmFile, "    mov rdx, 1\n");           // Number of bytes to write
                fprintf(asmFile, "    mov rax, 1\n");           // System call: sys_write
                fprintf(asmFile, "    syscall\n");              // Make the system call
                fprintf(asmFile, "    add rsp, 8\n");           // Clean up stack
            }
        }
        break;

    case NODE_IF:
        printf("Generating code for if statement\n");
        generateCode(node->ifNode.condition, asmFile);
        fprintf(asmFile, "    cmp rax, 0\n");
        fprintf(asmFile, "    je .else_%p\n", (void *)node);
        generateCode(node->ifNode.thenStmt, asmFile);
        fprintf(asmFile, "    jmp .endif_%p\n", (void *)node);
        fprintf(asmFile, ".else_%p:\n", (void *)node);
        generateCode(node->ifNode.elseStmt, asmFile);
        fprintf(asmFile, ".endif_%p:\n", (void *)node);
        break;

    case NODE_WHILE:
        printf("Generating code for while loop\n");
        fprintf(asmFile, ".loop_start_%p:\n", (void *)node);
        generateCode(node->whileNode.condition, asmFile);
        fprintf(asmFile, "    cmp rax, 0\n");
        fprintf(asmFile, "    je .loop_end_%p\n", (void *)node);
        generateCode(node->whileNode.body, asmFile);
        fprintf(asmFile, "    jmp .loop_start_%p\n", (void *)node);
        fprintf(asmFile, ".loop_end_%p:\n", (void *)node);
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
    
    // Add true/false strings for boolean printing
    fprintf(asmFile, "    true_str: db 'true', 0\n");
    fprintf(asmFile, "    false_str: db 'false', 0\n");
    
    fprintf(asmFile, "\n");

    // Text Section
    fprintf(asmFile, "section .text\n");
    fprintf(asmFile, "    global _start\n");

    // Add print_str function - simplified version
    fprintf(asmFile, "print_str:\n");
    fprintf(asmFile, "    push rbp\n");
    fprintf(asmFile, "    mov rbp, rsp\n");
    fprintf(asmFile, "    mov rsi, rdi\n");  // String address is in rdi

    // Print the string character by character
    fprintf(asmFile, ".print_loop:\n");
    fprintf(asmFile, "    movzx eax, byte [rsi]\n");  // Load character with zero extension
    fprintf(asmFile, "    test al, al\n");            // Check for null terminator
    fprintf(asmFile, "    jz .print_done\n");         // Exit if null
    
    // Print the character
    fprintf(asmFile, "    push rsi\n");               // Save string pointer
    fprintf(asmFile, "    mov rdi, 1\n");             // File descriptor: stdout
    fprintf(asmFile, "    mov rdx, 1\n");             // Number of bytes to write
    fprintf(asmFile, "    lea rsi, [rsi]\n");         // Address of character to print
    fprintf(asmFile, "    mov rax, 1\n");             // System call: sys_write
    fprintf(asmFile, "    syscall\n");                // Make the system call
    fprintf(asmFile, "    pop rsi\n");                // Restore string pointer
    
    // Move to next character
    fprintf(asmFile, "    inc rsi\n");                // Increment string pointer
    fprintf(asmFile, "    jmp .print_loop\n");        // Repeat for next character
    
    fprintf(asmFile, ".print_done:\n");
    fprintf(asmFile, "    mov rsp, rbp\n");
    fprintf(asmFile, "    pop rbp\n");
    fprintf(asmFile, "    ret\n\n");

    // Add print_num function (this is what we call, not print_int)
    fprintf(asmFile, "print_num:\n");
    fprintf(asmFile, "    push rbp\n");
    fprintf(asmFile, "    mov rbp, rsp\n");
    fprintf(asmFile, "    sub rsp, 32\n");            // Allocate buffer for string
    
    // Convert number to string
    fprintf(asmFile, "    mov rax, rdi\n");           // Number to print is in rdi
    fprintf(asmFile, "    lea rsi, [rsp]\n");         // Buffer for string
    fprintf(asmFile, "    add rsi, 31\n");            // Start at end of buffer
    fprintf(asmFile, "    mov byte [rsi], 0\n");      // Null terminator
    fprintf(asmFile, "    mov rcx, 10\n");            // Divisor
    
    fprintf(asmFile, ".convert_loop:\n");
    fprintf(asmFile, "    xor rdx, rdx\n");           // Clear rdx for division
    fprintf(asmFile, "    div rcx\n");                // rax = rax / 10, rdx = remainder
    fprintf(asmFile, "    add dl, '0'\n");            // Convert to ASCII
    fprintf(asmFile, "    dec rsi\n");                // Move buffer pointer
    fprintf(asmFile, "    mov [rsi], dl\n");          // Store digit
    fprintf(asmFile, "    test rax, rax\n");          // Check if number is zero
    fprintf(asmFile, "    jnz .convert_loop\n");      // If not, continue
    
    // Print the string
    fprintf(asmFile, "    mov rdi, rsi\n");           // String address
    fprintf(asmFile, "    call print_str\n");         // Call print_str
    
    fprintf(asmFile, "    mov rsp, rbp\n");
    fprintf(asmFile, "    pop rbp\n");
    fprintf(asmFile, "    ret\n\n");

    // Add print_log function for boolean values
    fprintf(asmFile, "print_log:\n");
    fprintf(asmFile, "    push rbp\n");
    fprintf(asmFile, "    mov rbp, rsp\n");
    
    // Check if the value is 0 (false) or non-zero (true)
    fprintf(asmFile, "    test rdi, rdi\n");
    fprintf(asmFile, "    jz .print_false\n");
    
    // Print "true"
    fprintf(asmFile, "    lea rdi, [rel true_str]\n");
    fprintf(asmFile, "    call print_str\n");
    fprintf(asmFile, "    jmp .print_log_done\n");
    
    // Print "false"
    fprintf(asmFile, ".print_false:\n");
    fprintf(asmFile, "    lea rdi, [rel false_str]\n");
    fprintf(asmFile, "    call print_str\n");
    
    fprintf(asmFile, ".print_log_done:\n");
    fprintf(asmFile, "    mov rsp, rbp\n");
    fprintf(asmFile, "    pop rbp\n");
    fprintf(asmFile, "    ret\n\n");

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
