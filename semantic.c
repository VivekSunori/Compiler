// semantic.c
#include "parser.c"

void generateCode(ASTNode *node, FILE *asmFile) {
    if (!node) return;

    switch (node->type) {
        case NODE_IF:
            generateCode(node->ifNode.condition, asmFile);
            fprintf(asmFile, "    cmp rax, 0\n");
            fprintf(asmFile, "    je .else_%p\n", (void*)node);
            generateCode(node->ifNode.thenStmt, asmFile);
            fprintf(asmFile, "    jmp .endif_%p\n", (void*)node);
            fprintf(asmFile, ".else_%p:\n", (void*)node);
            generateCode(node->ifNode.elseStmt, asmFile);
            fprintf(asmFile, ".endif_%p:\n", (void*)node);
            break;

        case NODE_WHILE:
            fprintf(asmFile, ".loop_start_%p:\n", (void*)node);
            generateCode(node->whileNode.condition, asmFile);
            fprintf(asmFile, "    cmp rax, 0\n");
            fprintf(asmFile, "    je .loop_end_%p\n", (void*)node);
            generateCode(node->whileNode.body, asmFile);
            fprintf(asmFile, "    jmp .loop_start_%p\n", (void*)node);
            fprintf(asmFile, ".loop_end_%p:\n", (void*)node);
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

        default:
            // Handle other cases as before
            break;
    }
    generateCode(node->next, asmFile);
}

void generateAssembly(const char* filename) {
    FILE *asmFile = fopen(filename, "w");
    if (!asmFile) {
        printf("Error: Cannot open ASM file\n");
        exit(1);
    }

    // Data Section
    fprintf(asmFile, "section .data\n");
    for (int i = 0; i < symCount; i++) {
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
}

