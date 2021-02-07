#pragma once

#include <assert.h>
#include <stdlib.h>
#include "stdio.h"

// NLR - pre-order traversal: node, left child, right child
// LNR - in-order traversal: left child, node, right child
// RNL - reverse in-order traversal: right child, node, left child
// LRN - post-order traversal: left child, right child, node
typedef enum treeTraversal {
    NLR, LNR, RNL, LRN
} treeTraversal;

typedef int Elem_t;

typedef struct Node {
    Elem_t key;
    unsigned char height;
    struct Node* leftChild;
    struct Node* rightChild;
} Node;

// Interface
void nodeInit(Node* node, Elem_t key);
Node* nodeRemove(Node* node, Elem_t key);
Node* nodeInsert(Node* node, Elem_t key);
void nodePrint(const Node* node, treeTraversal type);
void nodeDestroy(Node* node);
Node* nodeFindMin(Node* node);
void nodeDebugPrint(const Node* node);
int nodeForEach(Node* tree, int(*callback)(Node* node, void* data), void* data, treeTraversal type);