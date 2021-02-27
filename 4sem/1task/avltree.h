#pragma once

#include <assert.h>
#include <stdlib.h>
#include "stdio.h"

// Forward Declaration
struct Node;
struct avlTree;

// NLR - pre-order traversal: node, left child, right child
// LNR - in-order traversal: left child, node, right child
// RNL - reverse in-order traversal: right child, node, left child
// LRN - post-order traversal: left child, right child, node
typedef enum treeTraversal {
    NLR, LNR, RNL, LRN
} treeTraversal;

// Type of key in the node of the tree
typedef int Elem_t;

// Node definition
typedef struct Node* Node_p;

// Tree definition
typedef struct avlTree* avlTree_p;

// AVL tree API

// Creates the AVL tree
// @return The pointer to the new tree
avlTree_p avlTreeCreate();

// Removes the given key from the tree
// @param tree the pointer to the tree
// @param key the key to be removed
void avlTreeRemove(avlTree_p tree, Elem_t key);

// Inserts the given key into the tree
// @param tree the pointer to the tree
// @param key the key to be put into the tree
void avlTreeInsert(avlTree_p tree, Elem_t key);

// Prints the whole tree in the given order
// @param tree the pointer to the tree
// @param type order of bypass
void avlTreePrint(const avlTree_p tree, treeTraversal type);

// Destroys the given tree
// @param tree the pointer to the tree
void avlTreeDestroy(avlTree_p* tree);

// Finds the minimum key in the given tree
// @param tree the pointer to the tree
// @return the minimum element from the given tree
Elem_t avlTreeFindMin(const avlTree_p tree);

// Finds the maximum key in the given tree
// @param tree the pointer to the tree
// @return the maximum element from the given tree
Elem_t avlTreeFindMax(const avlTree_p tree);

// Finds the given key in the tree
// @param tree the given tree
// @param key the key to be found
// @return The pointer to the found nodem otherwise NULL 
Node_p avlTreeFind(avlTree_p tree, Elem_t key);

// @return the size of the tree
size_t avlTreeSize(const avlTree_p tree);

// Bypass function for the AVL tree with the depth traversal
// @param tree the tree to be interated by
// @param callback the function that is meant to be done to all nodes, requirments:
// 1) Must return EXIT_SUCCESS, if it has been completed right, otherwise EXIT_FAILURE
// 2) It's arguments are: the pointer to the tree - (Node*) node, and the pointer to some memory - (void*) data
// @param type the type of the traversal of the tree needed for this bypass
// @return EXIT_SUCCESS on success, otherwise EXIT_FAILURE
int avlTreeForEach(avlTree_p tree, int(*callback)(Node_p node, void* data), void* data, treeTraversal type);

// Tree Node API

// Get the value of the key of this node
// @param node the pointer to the node
// @return The value of the key
Elem_t nodeKey(const Node_p node);

// Calculates the height of the given tree
// @param node the given tree
// @return The actual height if the given node is not NULL, else zero
unsigned char nodeHeight(const Node_p node);

// Debug information about the certain node, not the whole tree
// @param node the given tree
void nodeDebugPrint(const Node_p node);

// Looks for the given element in the given tree
// @param node the given tree
// @param key element to be found
// @return The found element if it is in the node, otherwise NULL
Node_p nodeFind(Node_p node, Elem_t key);