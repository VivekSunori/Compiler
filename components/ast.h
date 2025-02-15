// ast.h
typedef enum { NODE_NUMBER, NODE_BINARY_OP, NODE_ASSIGN, NODE_IF, NODE_WHILE, NODE_FUNC_DEF, NODE_FUNC_CALL } NodeType;

typedef struct ASTNode {
    NodeType type;
    union {
        int number;
        struct { char op; struct ASTNode *left, *right; } binaryOp;
        struct { char name[50]; struct ASTNode *expr; } assign;
        struct { struct ASTNode *condition; struct ASTNode *thenStmt; struct ASTNode *elseStmt; } ifNode;
        struct { struct ASTNode *condition; struct ASTNode *body; } whileNode;
        struct { char name[50]; struct ASTNode *params; struct ASTNode *body; } funcDef;
        struct { char name[50]; struct ASTNode *args; } funcCall;
    };
    struct ASTNode *next;
} ASTNode;

// Global AST head
ASTNode *astHead = NULL;
ASTNode *astTail = NULL;
