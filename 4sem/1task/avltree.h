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

// The node structure for being used as a tree
typedef struct Node {
    Elem_t key;
    unsigned char height;
    struct Node* leftChild;
    struct Node* rightChild;
} Node_t;

// API

// Constructs the node with the given key
// @param node the pointer to the node
// @param keys the key to be put into the tree
void nodeInit(Node_t* node, Elem_t key);

// Removes the given node from the tree
// @param node the pointer to the node
// @param key the key to be put into the node
// @return The pointer to the tree without the node with this key
Node_t* nodeRemove(Node_t* node, Elem_t key);

// Insert the key into the given node
// @param node the pointer to the node
// @param key the key to be put into the node
// @return The pointer to the new tree
Node_t* nodeInsert(Node_t* node, Elem_t key);

// Prints the given tree (in-order)
// @param node the given tree
// @param type the type of the traversal of the tree
void nodePrint(const Node_t* node, treeTraversal type);

// Destroys the given tree
// @param tree the given tree
void nodeDestroy(Node_t* node);

// Finds the minimum key in the given tree
// @param node the pointer to the node
// @return The pointer to the node with the minimum key
Node_t* nodeFindMin(Node_t* node);

// Finds the maximum key in the given tree
// @param node the pointer to the node
// @return The pointer to the node with the maximum key
Node_t* nodeFindMax(Node_t* node);

// Calculates the height of the given tree
// @param node the given tree
// @return The actual height if the given node is not NULL, else zero
unsigned char nodeHeight(const Node_t* node);

// Bypass function for the AVL tree with the depth traversal
// @param tree the tree to be interated by
// @param callback the function that is meant to be done to all nodes, requirments:
// 1) Must return EXIT_SUCCESS, if it has been completed right, otherwise EXIT_FAILURE
// 2) It's arguments are: the pointer to the tree - (Node*) node, and the pointer to some memory - (void*) data
// @param type the type of the traversal of the tree needed for this bypass
// @return EXIT_SUCCESS on success, otherwise EXIT_FAILURE
int nodeForEach(Node_t* tree, int(*callback)(Node_t* node, void* data), void* data, treeTraversal type);

// Debug information about the certain node, not the whole tree
// @param node the given tree
void nodeDebugPrint(const Node_t* node);