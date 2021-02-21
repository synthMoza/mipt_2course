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

// Type of key in the node of the tree
typedef int Elem_t;

typedef struct Node {
    Elem_t key;
    unsigned char height;
    struct Node* leftChild;
    struct Node* rightChild;
} Node_t;

// API

void nodeInit(Node_t* node, Elem_t key);
Node_t* nodeRemove(Node_t* node, Elem_t key);
Node_t* nodeInsert(Node_t* node, Elem_t key);
void nodePrint(const Node_t* node, treeTraversal type);
void nodeDestroy(Node_t* node);
Node_t* nodeFindMin(Node_t* node);
Node_t* nodeFindMax(Node_t* node);
unsigned char nodeHeight(const Node_t* node);
int nodeForEach(Node_t* tree, int(*callback)(Node_t* node, void* data), void* data, treeTraversal type);
void nodeDebugPrint(const Node_t* node);