// semantic.c
#include "components/generator/codegen.c"
void checkSemantic(ASTNode *node) {
    if (!node) return;

    switch (node->type) {
        case NODE_ASSIGN:
            if (lookupSymbol(node->assign.name) == -1) { 
                printf("Semantic Error: Variable '%s' not declared before assignment\n", node->assign.name);
                exit(1);
            }
            checkSemantic(node->assign.expr);
            break;

        case NODE_VAR_DECL:
            if (lookupSymbol(node->varDecl.name) != -1) { 
                printf("Semantic Error: Variable '%s' is already declared\n", node->varDecl.name);
                exit(1);
            }
            addSymbol(node->varDecl.name, 0); 
            break;

        case NODE_FUNC_CALL:
            if (lookupSymbol(node->funcCall.name) == -1) { 
                printf("Semantic Error: Function '%s' is not defined\n", node->funcCall.name);
                exit(1);
            }
            checkSemantic(node->funcCall.args);
            break;

        default:
            break;
    }

    checkSemantic(node->next);
}
