// codegen.c
#include "../parsers/header/parser.h"

// Define a simple visited nodes tracking system for code generation
#define MAX_CODEGEN_VISITED 1000
ASTNode* codegenVisited[MAX_CODEGEN_VISITED];
int codegenVisitedCount = 0;

// Check if a node has been visited during code generation
int codegenHasVisited(ASTNode* node) {
    for (int i = 0; i < codegenVisitedCount; i++) {
        if (codegenVisited[i] == node) {
            return 1;
        }
    }
    return 0;
}

// Mark a node as visited during code generation
void codegenMarkVisited(ASTNode* node) {
    if (codegenVisitedCount < MAX_CODEGEN_VISITED) {
        codegenVisited[codegenVisitedCount++] = node;
    }
}

// Reset visited nodes for code generation
void codegenResetVisited() {
    codegenVisitedCount = 0;
}

#define MAX_STRING_LITERALS 100
#define MAX_STRING_LENGTH 256

char stringLiterals[MAX_STRING_LITERALS][MAX_STRING_LENGTH];
int stringLiteralCount = 0;

// Add a function to reset string literals
void resetStringLiterals() {
    stringLiteralCount = 0;
    memset(stringLiterals, 0, sizeof(stringLiterals));
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
                fprintf(asmFile, "    xor rdx, rdx\n");  // Clear rdx for division
                fprintf(asmFile, "    idiv rbx\n");
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
        
        // Check if we're printing a variable that might contain a string
        if (node->print.expr->type == NODE_VAR_REF) {
            const char* varName = node->print.expr->varRef.name;
            printf("Printing variable reference: %s\n", varName);
            
            // Check if this variable was declared as a string
            int isString = 0;
            for (int i = 0; i < symCount; i++) {
                if (strcmp(symTable[i].name, varName) == 0) {
                    if (symTable[i].type == TYPE_STRING) {
                        isString = 1;
                        printf("Variable %s is a string\n", varName);
                    }
                    break;
                }
            }
            
            generateCode(node->print.expr, asmFile);
            
            if (asmFile) {
                if (isString) {
                    fprintf(asmFile, "    mov rdi, rax\n");
                    fprintf(asmFile, "    call print_str\n");
                } else {
                    fprintf(asmFile, "    mov rdi, rax\n");
                    fprintf(asmFile, "    call print_int\n");
                }
            }
        } else if (node->print.expr->type == NODE_STRING_LITERAL) {
            printf("Printing string literal\n");
            generateCode(node->print.expr, asmFile);
            if (asmFile) {
                fprintf(asmFile, "    mov rdi, rax\n");
                fprintf(asmFile, "    call print_str\n");
            }
        } else {
            printf("Printing numeric expression\n");
            generateCode(node->print.expr, asmFile);
            if (asmFile) {
                fprintf(asmFile, "    mov rdi, rax\n");
                fprintf(asmFile, "    call print_int\n");
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
        
        // Compare the operands
        fprintf(asmFile, "    cmp rax, rbx\n");
        
        // Set result based on comparison
        if (strcmp(node->relOp.op, "<") == 0) {
            fprintf(asmFile, "    setl al\n");
        } else if (strcmp(node->relOp.op, ">") == 0) {
            fprintf(asmFile, "    setg al\n");
        } else if (strcmp(node->relOp.op, "<=") == 0) {
            fprintf(asmFile, "    setle al\n");
        } else if (strcmp(node->relOp.op, ">=") == 0) {
            fprintf(asmFile, "    setge al\n");
        } else if (strcmp(node->relOp.op, "==") == 0) {
            fprintf(asmFile, "    sete al\n");
        } else if (strcmp(node->relOp.op, "!=") == 0) {
            fprintf(asmFile, "    setne al\n");
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

    // === Emit DATA section ===
    fprintf(asmFile, "; Assembly code generated by CompilerX\n");
    fprintf(asmFile, "section .data\n");
    fprintf(asmFile, "    newline db 10\n");  // Newline character
    fprintf(asmFile, "    minus_sign db '-'\n");  // Minus sign for negative numbers
    fprintf(asmFile, "    buffer times 20 db 0\n");  // Buffer for integer to string conversion

    // Now stringLiteralCount should be > 0
    for (int i = 0; i < stringLiteralCount; i++) {
        fprintf(asmFile, "    str_%d db \"%s\", 0\n", i, stringLiterals[i]);
        printf("Added string literal to data section: '%s'\n", stringLiterals[i]);
    }

    // Add variables to data section
    for (int i = 0; i < symCount; i++) {
        fprintf(asmFile, "    %s dq 0\n", symTable[i].name);
        printf("Added variable: %s\n", symTable[i].name);
    }

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
    fprintf(asmFile, "    push rax\n");               // Save character
    fprintf(asmFile, "    mov [buffer], al\n");       // Store character in buffer
    fprintf(asmFile, "    mov rax, 1\n");             // sys_write
    fprintf(asmFile, "    mov rdi, 1\n");             // stdout
    fprintf(asmFile, "    lea rsi, [buffer]\n");      // Address of buffer
    fprintf(asmFile, "    mov rdx, 1\n");             // Length = 1
    fprintf(asmFile, "    syscall\n");                // Call kernel
    fprintf(asmFile, "    pop rax\n");                // Restore character
    fprintf(asmFile, "    pop rsi\n");                // Restore string pointer

    // Move to next character
    fprintf(asmFile, "    inc rsi\n");                // Next character
    fprintf(asmFile, "    jmp .print_loop\n");        // Repeat

    // Print newline and return
    fprintf(asmFile, ".print_done:\n");
    fprintf(asmFile, "    mov rax, 1\n");             // sys_write
    fprintf(asmFile, "    mov rdi, 1\n");             // stdout
    fprintf(asmFile, "    lea rsi, [newline]\n");     // Address of newline
    fprintf(asmFile, "    mov rdx, 1\n");             // Length = 1
    fprintf(asmFile, "    syscall\n");                // Call kernel
    fprintf(asmFile, "    mov rsp, rbp\n");
    fprintf(asmFile, "    pop rbp\n");
    fprintf(asmFile, "    ret\n\n");
    
    // Define print_int function using system calls
    fprintf(asmFile, "print_int:\n");
    fprintf(asmFile, "    push rbp\n");
    fprintf(asmFile, "    mov rbp, rsp\n");

    // Convert integer to string
    fprintf(asmFile, "    mov rax, rdi\n");        // Number to print is in rdi
    fprintf(asmFile, "    mov rcx, 10\n");         // Divisor
    fprintf(asmFile, "    mov rsi, buffer\n");
    fprintf(asmFile, "    add rsi, 19\n");         // Start at end of buffer
    fprintf(asmFile, "    mov byte [rsi], 0\n");   // Null terminator

    // Handle negative numbers
    fprintf(asmFile, "    test rax, rax\n");
    fprintf(asmFile, "    jns .positive\n");
    fprintf(asmFile, "    neg rax\n");
    fprintf(asmFile, "    push rax\n");            // Save absolute value
    fprintf(asmFile, "    mov rax, 1\n");          // sys_write
    fprintf(asmFile, "    mov rdi, 1\n");          // stdout
    fprintf(asmFile, "    mov rsi, minus_sign\n");
    fprintf(asmFile, "    mov rdx, 1\n");          // Length
    fprintf(asmFile, "    syscall\n");
    fprintf(asmFile, "    pop rax\n");             // Restore absolute value

    fprintf(asmFile, ".positive:\n");
    fprintf(asmFile, "    mov rbx, rsi\n");        // Save end position

    fprintf(asmFile, ".digit_loop:\n");
    fprintf(asmFile, "    dec rsi\n");             // Move back in buffer
    fprintf(asmFile, "    xor rdx, rdx\n");        // Clear rdx for division
    fprintf(asmFile, "    div rcx\n");             // Divide by 10
    fprintf(asmFile, "    add dl, '0'\n");         // Convert remainder to ASCII
    fprintf(asmFile, "    mov [rsi], dl\n");       // Store digit
    fprintf(asmFile, "    test rax, rax\n");       // Check if quotient is zero
    fprintf(asmFile, "    jnz .digit_loop\n");     // If not, continue

    // Calculate string length and print
    fprintf(asmFile, "    mov rdx, rbx\n");
    fprintf(asmFile, "    sub rdx, rsi\n");        // Length = end - current position
    fprintf(asmFile, "    mov rax, 1\n");          // sys_write
    fprintf(asmFile, "    mov rdi, 1\n");          // stdout
    fprintf(asmFile, "    syscall\n");

    // Print newline
    fprintf(asmFile, "    mov rax, 1\n");          // sys_write
    fprintf(asmFile, "    mov rdi, 1\n");          // stdout
    fprintf(asmFile, "    mov rsi, newline\n");
    fprintf(asmFile, "    mov rdx, 1\n");          // Length
    fprintf(asmFile, "    syscall\n");

    fprintf(asmFile, "    mov rsp, rbp\n");
    fprintf(asmFile, "    pop rbp\n");
    fprintf(asmFile, "    ret\n\n");
    
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
