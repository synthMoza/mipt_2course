#include "avltree.h"

// Simple initialization of the node with the given key
void nodeInit(Node* node, Elem_t key) {
    assert(node);

    node->height = 1;
    node->key = key;
    node->leftChild = NULL;
    node->rightChild = NULL;
}

// Calculates the height of the given tree
// Returns the actual height if the given node is not NULL, else returns 0
unsigned char nodeHeight(const Node* node) {
    if (node == NULL)
        return 0;
    else
        return node->height;
}

// Calculates the "balance-factor" of the given tree
// Returns the difference between the rightChild's and leftChild's heights
int nodeBFactor(const Node* node) {
    int result = nodeHeight(node->rightChild) - nodeHeight(node->leftChild);
    return result;
}

// Recalculates the height of the given tree
void nodeFixHeight(Node* node) {
    unsigned char height1 = nodeHeight(node->leftChild);
    unsigned char height2 = nodeHeight(node->rightChild);
    if (height1 > height2) 
        node->height = height1 + 1;
    else
        node->height = height2 + 1;
}

// Performs a simple right rotation of the given node
// Returns rotated node
Node* nodeRotateRight(Node* p) {
    assert(p);
    assert(p->leftChild);

    Node* q = p->leftChild;
    p->leftChild = q->rightChild;
    q->rightChild = p;

    nodeFixHeight(p);
    nodeFixHeight(q);

    return q;
}

// Performs a simple left rotation of the given node
// Returns rotated node
Node* nodeRotateLeft(Node* q) {
    assert(q);
    assert(q->rightChild);

    Node* p = q->rightChild;
    q->rightChild = p->leftChild;
    p->leftChild = q;

    nodeFixHeight(p);
    nodeFixHeight(q);

    return p;
}

// Balances the given tree in terms of AVL
// Returns the balanced tree
Node* nodeBalance(Node* node) {
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
// Returns the new tree with the inserted key
Node* nodeInsert(Node* node, Elem_t key) {
    if (node == NULL) {
        // Create new node
        Node* temp = (Node*) calloc(1, sizeof(*temp));
        if (temp == NULL) {
            // Calloc error
            printf("Calloc error: failed to allocate Node!\n");
            abort();
        }
        nodeInit(temp, key);
        return temp;
    }

    if (key < node->key)
        node->leftChild = nodeInsert(node->leftChild, key);
    else
        node->rightChild = nodeInsert(node->rightChild, key);

    return nodeBalance(node);
}

// Finds the minimum key in the given tree
// Returns the node with the found key
Node* nodeFindMin(Node* node) {
    if (node->leftChild != NULL)
        return nodeFindMin(node->leftChild);
    else
        return node;
}

// Removes the minimum element in the given node
// Returns the new tree
Node* nodeRemoveMin(Node* node) {
    if (node->leftChild == NULL)
        return node->rightChild;
    
    node->leftChild = nodeRemoveMin(node->leftChild);
    return nodeBalance(node);
}

// Removes the given node from the tree
// Returns the new tree
Node* nodeRemove(Node* node, Elem_t key) {
    if (node == NULL)
        return NULL;
    
    if (key < node->key)
        node->leftChild = nodeRemove(node->leftChild, key);
    else if (key > node->key)
        node->rightChild = nodeRemove(node->rightChild, key);
    else {
        // Found the key
        Node* q = node->leftChild;
        Node* r = node->rightChild;
        free(node);
        if (r == NULL)
            return q;
        Node* min = nodeFindMin(r);
        min->rightChild = nodeRemoveMin(r);
        min->leftChild = q;
        return nodeBalance(min);
    }

    return nodeBalance(node);
}

// Bypass function for the AVL tree with the depth traversal
int nodeForEach(Node* tree, int(*callback)(Node* node, void* data), void* data, treeTraversal type) {
    assert(tree);
    int result = 0;

    if (type == NLR) {
        // Visit node
        result = callback(tree, data);
        if (result != EXIT_SUCCESS)
            return EXIT_FAILURE;
    } else if (type == RNL) {
        // Visit right child
        if (tree->rightChild != NULL) {
            result = nodeForEach(tree->rightChild, callback, data, type);
            if (result != EXIT_SUCCESS)
                return EXIT_FAILURE;
        }
    } else {
        // Visit left child
        if (tree->leftChild != NULL) {
            result = nodeForEach(tree->leftChild, callback, data, type);
            if (result != EXIT_SUCCESS)
                return EXIT_FAILURE;
        }
    }

    if (type == NLR) {
        // Visit left child
        if (tree->leftChild != NULL) {
            result = nodeForEach(tree->leftChild, callback, data, type);
            if (result != EXIT_SUCCESS)
                return EXIT_FAILURE;
        }
    } else if (type == LRN) {
        // Visit right child
        if (tree->rightChild != NULL) {
            result = nodeForEach(tree->rightChild, callback, data, type);
            if (result != EXIT_SUCCESS)
                return EXIT_FAILURE;
        }
    } else {
        // Visit node
        result = callback(tree, data);
        if (result != EXIT_SUCCESS)
            return EXIT_FAILURE;
    }

    if (type == RNL) {
        // Visit left child
        if (tree->leftChild != NULL) {
            result = nodeForEach(tree->leftChild, callback, data, type);
            if (result != EXIT_SUCCESS)
                return EXIT_FAILURE;
        }
    } else if (type == LRN) {
        // Visit node
        result = callback(tree, data);
        if (result != EXIT_SUCCESS)
            return EXIT_FAILURE;
    } else {
        // Visit right child
        if (tree->rightChild != NULL) {
            result = nodeForEach(tree->rightChild, callback, data, type);
            if (result != EXIT_SUCCESS)
                return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}

// Debug information about the certain node, not the whole tree
void nodeDebugPrint(const Node* node) {
    assert(node);

    printf("Node adress: %p\n", node);
    // Format of the key is necessary
    printf("Key: %d\n", node->key);
    printf("Height: %u\n", node->height);
    printf("Left child adress: %p\n", node->leftChild);
    printf("Right child adress: %p\n", node->rightChild);
}

// Function for printing the Node key (nodeForEach format)
int print_node(Node* node, void* data) {
    printf("%d ", node->key);
    return EXIT_SUCCESS;
}

// Prints the given tree (in-order)
void nodePrint(const Node* node, treeTraversal type) {
    nodeForEach((Node*) node, print_node, NULL, type);
    printf("\n");
} 

// Destroys the given tree
void nodeDestroy(Node* node) {
    if (node == NULL)
        return ;
    
    nodeDestroy(node->leftChild);
    nodeDestroy(node->rightChild);
    free(node);
}