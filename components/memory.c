#include <stdlib.h>
#include <string.h>
#include "memory.h"

// Define the memory block structure
typedef struct MemoryBlock {
    void* block;
    struct MemoryBlock* next;
} MemoryBlock;

// Global variables
ASTNode* astHead = NULL;
ASTNode* astTail = NULL;
MemoryBlock* memoryBlockHead = NULL;
Arena* arenaHead = NULL;

// Memory management functions
void* arenaAlloc(size_t size) {
    if (size == 0) {
        printf("Warning: Attempted to allocate 0 bytes\n");
        size = 1; // Allocate at least 1 byte
    }
    
    void* block = malloc(size);
    if (!block) {
        printf("Fatal error: Memory allocation failed for %zu bytes\n", size);
        exit(1);
    }
    
    // Initialize memory to zero to prevent uninitialized memory issues
    memset(block, 0, size);
    
    // Store the allocated block in a linked list for later cleanup
    MemoryBlock* newBlock = malloc(sizeof(MemoryBlock));
    if (!newBlock) {
        printf("Fatal error: Memory allocation failed for memory block tracking\n");
        free(block); // Free the previously allocated block
        exit(1);
    }
    
    newBlock->block = block;
    newBlock->next = memoryBlockHead;
    memoryBlockHead = newBlock;
    
    return block;
}

ASTNode* allocateNode(NodeType type) {
    ASTNode* node = arenaAlloc(sizeof(ASTNode));
    if (!node) {
        printf("Error: Failed to allocate memory for AST node\n");
        exit(1);
    }
    
    // Initialize the node
    memset(node, 0, sizeof(ASTNode));  // Zero out all fields
    node->type = type;
    node->next = NULL;
    
    return node;
}

void freeArena() {
    MemoryBlock* current = memoryBlockHead;
    while (current) {
        MemoryBlock* next = current->next;
        free(current->block);
        free(current);
        current = next;
    }
    
    memoryBlockHead = NULL;
    arenaHead = NULL;
    astHead = NULL;
    astTail = NULL;
}

