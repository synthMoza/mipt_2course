#include "avltree.h"

// The structure of the node for the tree
struct Node {
    Elem_t key;
    unsigned char height;
    struct Node* leftChild;
    struct Node* rightChild;
};

// Constructs the node with the given key
// @param node the pointer to the node
// @param keys the key to be put into the tree
static Node_p nodeCreate(Elem_t key) {
    Node_p node = (Node_p) calloc(1, sizeof(*node));
    if (node == NULL) {
        // Calloc error
        return node;
    }

    node->height = 1;
    node->key = key;
    node->leftChild = NULL;
    node->rightChild = NULL;

    return node;
}

Elem_t nodeKey(const Node_p node) {
    assert(node);

    return node->key;
}

unsigned char nodeHeight(const Node_p node) {
    if (node == NULL)
        return 0;
    else
        return node->height;
}

// Calculates the "balance-factor" of the given tree
// @param node the given tree
// @return The difference between the rightChild's and leftChild's heights
static int nodeBFactor(const Node_p node) {
    return nodeHeight(node->rightChild) - nodeHeight(node->leftChild);
}

// Recalculates the height of the given tree
// @param node the given tree
static void nodeFixHeight(Node_p node) {
    unsigned char height1 = nodeHeight(node->leftChild);
    unsigned char height2 = nodeHeight(node->rightChild);
    if (height1 > height2) 
        node->height = height1 + 1;
    else
        node->height = height2 + 1;
}

// Performs a simple right rotation of the given node
// @param p the given tree
// @return The pointer to the rotated tree
static Node_p nodeRotateRight(Node_p p) {
    assert(p);
    assert(p->leftChild);

    Node_p q = p->leftChild;
    p->leftChild = q->rightChild;
    q->rightChild = p;

    nodeFixHeight(p);
    nodeFixHeight(q);

    return q;
}

// Performs a simple left rotation of the given node
// @param p the given tree
// @return The pointer to the rotated tree
static Node_p nodeRotateLeft(Node_p q) {
    assert(q);
    assert(q->rightChild);

    Node_p p = q->rightChild;
    q->rightChild = p->leftChild;
    p->leftChild = q;

    nodeFixHeight(p);
    nodeFixHeight(q);

    return p;
}

// Balances the given tree in terms of AVL
// @param node the given tree
// @return The pointer to the balanced tree
static Node_p nodeBalance(Node_p node) {
    nodeFixHeight(node);
    int bFactor = nodeBFactor(node);

    switch (bFactor) {
        case 2:
            if (nodeBFactor(node->rightChild) < 0)
                node->rightChild = nodeRotateRight(node->rightChild);
        
            return nodeRotateLeft(node);
        case -2:
            if (nodeBFactor(node->leftChild) > 0)
                node->leftChild = nodeRotateLeft(node->leftChild);
            
            return nodeRotateRight(node);
        default:
            // No need to balance the tree
            return node;
    }   
}

// Insert the key into the given node
// @param node the pointer to the node
// @param key the key to be put into the node
// @return The pointer to the new tree
static Node_p nodeInsert(Node_p node, Elem_t key) {
    if (node == NULL) {
        // Create new node
        return nodeCreate(key);
    }

    if (key < node->key)
        node->leftChild = nodeInsert(node->leftChild, key);
    else
        node->rightChild = nodeInsert(node->rightChild, key);

    return nodeBalance(node);
}

// Finds the minimum key in the given tree
// @param node the pointer to the node
// @return The pointer to the node with the minimum key
static Node_p nodeFindMin(Node_p node) {
    if (node->leftChild != NULL)
        return nodeFindMin(node->leftChild);
    else
        return node;
}

// Finds the maximum key in the given tree
// @param node the pointer to the node
// @return The pointer to the node with the maximum key
static Node_p nodeFindMax(Node_p node) {
    if (node->rightChild != NULL)
        return nodeFindMax(node->rightChild);
    else
        return node;
}

// Removes the minimum element in the given node
// @param node the pointer to the node
// @return The pointer to the tree without the minimum node
// @note Does not free memory of this node
static Node_p nodeRemoveMin(Node_p node) {
    if (node->leftChild == NULL)
        return node->rightChild;
    
    node->leftChild = nodeRemoveMin(node->leftChild);
    return nodeBalance(node);
}

// Removes the given node from the tree
// @param node the pointer to the node
// @param key the key to be put into the node
// @return The pointer to the tree without the node with this key
static Node_p nodeRemove(Node_p node, Elem_t key) {
    if (key < node->key)
        node->leftChild = nodeRemove(node->leftChild, key);
    else if (key > node->key)
        node->rightChild = nodeRemove(node->rightChild, key);
    else {
        // Found the key
        Node_p q = node->leftChild;
        Node_p r = node->rightChild;
        free(node);
        if (r == NULL)
            return q;
        Node_p min = nodeFindMin(r);
        min->rightChild = nodeRemoveMin(r);
        min->leftChild = q;
        return nodeBalance(min);
    }

    return nodeBalance(node);
}

// Helpful fuinction for nodeForEach, traversal - LNR
static int nodeForEachLNR(Node_p tree, int(*callback)(Node_p node, void* data), void* data) {
    int result = 0;

    // Visit left child
    if (tree->leftChild != NULL) {
            result = nodeForEachLNR(tree->leftChild, callback, data);
            if (result != EXIT_SUCCESS)
                return EXIT_FAILURE;
    }

    // Visit node
    result = callback(tree, data);
        if (result != EXIT_SUCCESS)
            return EXIT_FAILURE;
    
    // Visit right child
    if (tree->rightChild != NULL) {
            result = nodeForEachLNR(tree->rightChild, callback, data);
            if (result != EXIT_SUCCESS)
                return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

// Helpful fuinction for nodeForEach, traversal - RNL
static int nodeForEachRNL(Node_p tree, int(*callback)(Node_p node, void* data), void* data) {
    int result = 0;

    // Visit right child
    if (tree->rightChild != NULL) {
            result = nodeForEachRNL(tree->rightChild, callback, data);
            if (result != EXIT_SUCCESS)
                return EXIT_FAILURE;
    }

    // Visit node
    result = callback(tree, data);
        if (result != EXIT_SUCCESS)
            return EXIT_FAILURE;

    // Visit left child
    if (tree->leftChild != NULL) {
            result = nodeForEachRNL(tree->leftChild, callback, data);
            if (result != EXIT_SUCCESS)
                return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

// Helpful fuinction for nodeForEach, traversal - NLR
static int nodeForEachNLR(Node_p tree, int(*callback)(Node_p node, void* data), void* data) {
    int result = 0;

    // Visit node
    result = callback(tree, data);
        if (result != EXIT_SUCCESS)
            return EXIT_FAILURE;

    // Visit left child
    if (tree->leftChild != NULL) {
            result = nodeForEachNLR(tree->leftChild, callback, data);
            if (result != EXIT_SUCCESS)
                return EXIT_FAILURE;
    }

    // Visit right child
    if (tree->rightChild != NULL) {
            result = nodeForEachNLR(tree->rightChild, callback, data);
            if (result != EXIT_SUCCESS)
                return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

// Helpful fuinction for nodeForEach, traversal - LRN
static int nodeForEachLRN(Node_p tree, int(*callback)(Node_p node, void* data), void* data) {
    int result = 0;

    // Visit left child
    if (tree->leftChild != NULL) {
            result = nodeForEachLRN(tree->leftChild, callback, data);
            if (result != EXIT_SUCCESS)
                return EXIT_FAILURE;
    }

    // Visit right child
    if (tree->rightChild != NULL) {
            result = nodeForEachLRN(tree->rightChild, callback, data);
            if (result != EXIT_SUCCESS)
                return EXIT_FAILURE;
    }

    // Visit node
    result = callback(tree, data);
        if (result != EXIT_SUCCESS)
            return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

static int nodeForEach(Node_p tree, int(*callback)(Node_p node, void* data), void* data, treeTraversal type) {
    assert(tree);

    switch (type) {
        case NLR:
            return nodeForEachNLR(tree, callback, data);
        case LNR:
            return nodeForEachLNR(tree, callback, data);
        case RNL:
            return nodeForEachRNL(tree, callback, data);
        case LRN:
            return nodeForEachLRN(tree, callback, data);
        default:
            // Unknown type
            return EXIT_FAILURE;
    }
}

// Debug information about the certain node, not the whole tree
// @param node the given tree
void nodeDebugPrint(const Node_p node) {
    assert(node);

    printf("Node adress: %p\n", node);
    // Format of the key is necessary
    printf("Key: %d\n", node->key);
    printf("Height: %u\n", node->height);
    printf("Left child adress: %p\n", node->leftChild);
    printf("Right child adress: %p\n", node->rightChild);
}

// Function for printing the Node key (nodeForEach format)
// @param node the given tree
// @param data not needed in this function, may be anything
static int print_node(Node_p node, void* data) {
    printf("%d ", node->key);
    return EXIT_SUCCESS;
}

// Prints the given tree (in-order)
// @param node the given tree
// @param type the type of the traversal of the tree
static void nodePrint(const Node_p node, treeTraversal type) {
    nodeForEach((Node_p) node, print_node, NULL, type);
    printf("\n");
} 

// Destroys the given tree
// @param tree the given tree
static void nodeDestroy(Node_p node) {
    if (node == NULL)
        return ;
    
    nodeDestroy(node->leftChild);
    nodeDestroy(node->rightChild);
    free(node);
}

Node_p nodeFind(Node_p node, Elem_t key) {
    if (node == NULL)
        return NULL;
    
    if (key > node->key)
        return nodeFind(node->rightChild, key);
    else if (key < node->key)
        return nodeFind(node->leftChild, key);
    else
        // Found the element
        return node;
}

//==============================================================

// The structure for the tree itself
struct avlTree {
    struct Node* root;
    size_t size;
};

avlTree_p avlTreeCreate() {
    avlTree_p tree = (avlTree_p) calloc(1, sizeof(*tree));
    if (tree == NULL)
        // Calloc error
        return tree;

    tree->root = NULL;
    tree->size = 0;

    return tree;
}

void avlTreeRemove(avlTree_p tree, Elem_t key) {
    assert(tree);

    // Remove the element only if it presented in the tree
    if (nodeFind(tree->root, key) != NULL) {
        tree->root = nodeRemove(tree->root, key);
        tree->size--;
    }
}

void avlTreeInsert(avlTree_p tree, Elem_t key) {
    assert(tree);

    tree->root = nodeInsert(tree->root, key);
    tree->size++;
}

void avlTreePrint(const avlTree_p tree, treeTraversal type) {
    assert(tree);

    nodePrint(tree->root, type);
}

void avlTreeDestroy(avlTree_p* tree) {
    assert(tree);

    if ((*tree)->size != 0)
        // Destroy the tree if it itsn't empty
        nodeDestroy((*tree)->root);

    free(*tree);
}

Elem_t avlTreeFindMin(const avlTree_p tree) {
    assert(tree);

    Node_p min = nodeFindMin(tree->root);
    return min->key;
}

Elem_t avlTreeFindMax(const avlTree_p tree) {
    assert(tree);

    Node_p max = nodeFindMax(tree->root);
    return max->key;
}

Node_p avlTreeFind(avlTree_p tree, Elem_t key) {
    assert(tree);

    return nodeFind(tree->root, key);
}

size_t avlTreeSize(const avlTree_p tree) {
    assert(tree);

    return tree->size;
}

int avlTreeForEach(avlTree_p tree, int(*callback)(Node_p node, void* data), void* data, treeTraversal type) {
    assert(tree);
    assert(callback);

    return nodeForEach(tree->root, callback, data, type);
}