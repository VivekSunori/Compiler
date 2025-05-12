// codegen.c
#include "../parsers/header/parser.h"

void generateCode(ASTNode *node, FILE *asmFile)
{
    if (!node)
        return;

    switch (node->type)
    {
    case NODE_IF:
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
        fprintf(asmFile, ".loop_start_%p:\n", (void *)node);
        generateCode(node->whileNode.condition, asmFile);
        fprintf(asmFile, "    cmp rax, 0\n");
        fprintf(asmFile, "    je .loop_end_%p\n", (void *)node);
        generateCode(node->whileNode.body, asmFile);
        fprintf(asmFile, "    jmp .loop_start_%p\n", (void *)node);
        fprintf(asmFile, ".loop_end_%p:\n", (void *)node);
        break;

    case NODE_FUNC_DEF:
        fprintf(asmFile, "%s:\n", node->funcDef.name);
        fprintf(asmFile, "    push rbp\n    mov rbp, rsp\n");
        generateCode(node->funcDef.body, asmFile);
        fprintf(asmFile, "    mov rsp, rbp\n    pop rbp\n    ret\n");
        break;

    case NODE_FUNC_CALL:
        generateCode(node->funcCall.args, asmFile);
        fprintf(asmFile, "    call %s\n", node->funcCall.name);
        break;

    case NODE_FOR:
        // Generate initialization (before loop starts)
        generateCode(node->forNode.initialization, asmFile);

        // Loop label
        fprintf(asmFile, ".for_condition_%p:\n", (void *)node);

        // Generate condition check (if the condition is false, jump out of the loop)
        generateCode(node->forNode.condition, asmFile);
        fprintf(asmFile, "    cmp rax, 0\n");
        fprintf(asmFile, "    je .for_end_%p\n", (void *)node); // Jump to end if condition is false

        // Generate loop body
        generateCode(node->forNode.body, asmFile);

        // Generate increment (after the loop body)
        generateCode(node->forNode.increment, asmFile);

        // Jump back to the condition check
        fprintf(asmFile, "    jmp .for_condition_%p\n", (void *)node);

        // Loop exit label
        fprintf(asmFile, ".for_end_%p:\n", (void *)node);
        break;
    case NODE_DO_WHILE:
        // Loop start label (before loop body)
        fprintf(asmFile, ".do_start_%p:\n", (void *)node);

        // Generate loop body
        generateCode(node->doWhileNode.body, asmFile);

        // Generate condition check (if the condition is false, jump out of the loop)
        generateCode(node->doWhileNode.condition, asmFile);
        fprintf(asmFile, "    cmp rax, 0\n");
        fprintf(asmFile, "    je .do_end_%p\n", (void *)node); // Jump to end if condition is false

        // Jump back to the start of the loop
        fprintf(asmFile, "    jmp .do_start_%p\n", (void *)node);

        // Loop exit label
        fprintf(asmFile, ".do_end_%p:\n", (void *)node);
        break;

    case NODE_PRINT:
        generateCode(node->print.expr, asmFile);
        fprintf(asmFile, "    mov rdi, rax\n");
        fprintf(asmFile, "    call print_int\n");
        break;

    default:
        break;
    }
    generateCode(node->next, asmFile);
}

void generateAssembly(const char *filename)
{
    FILE *asmFile = fopen(filename, "w");
    if (!asmFile)
    {
        printf("Error: Cannot open ASM file\n");
        exit(1);
    }

    // Data Section
    fprintf(asmFile, "section .data\n");
    for (int i = 0; i < symCount; i++)
    {
        fprintf(asmFile, "    %s dq 0\n", symTable[i].name);
    }

    // Text Section
    fprintf(asmFile, "section .text\n");
    fprintf(asmFile, "global _start\n");
    fprintf(asmFile, "_start:\n");

    generateCode(astHead, asmFile);

    fprintf(asmFile, "    mov rax, 60\n");
    fprintf(asmFile, "    xor rdi, rdi\n");
    fprintf(asmFile, "    syscall\n");

    fclose(asmFile);
    
    // Free memory allocated by the arena
    freeArena();
}
