#include "avltree.h"

void nodeInit(Node_t* node, Elem_t key) {
    assert(node);

    node->height = 1;
    node->key = key;
    node->leftChild = NULL;
    node->rightChild = NULL;
}

unsigned char nodeHeight(const Node_t* node) {
    if (node == NULL)
        return 0;
    else
        return node->height;
}

// Calculates the "balance-factor" of the given tree
// @param node the given tree
// @return The difference between the rightChild's and leftChild's heights
int nodeBFactor(const Node_t* node) {
    int result = nodeHeight(node->rightChild) - nodeHeight(node->leftChild);
    return result;
}

// Recalculates the height of the given tree
// @param node the given tree
void nodeFixHeight(Node_t* node) {
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
Node_t* nodeRotateRight(Node_t* p) {
    assert(p);
    assert(p->leftChild);

    Node_t* q = p->leftChild;
    p->leftChild = q->rightChild;
    q->rightChild = p;

    nodeFixHeight(p);
    nodeFixHeight(q);

    return q;
}

// Performs a simple left rotation of the given node
// @param p the given tree
// @return The pointer to the rotated tree
Node_t* nodeRotateLeft(Node_t* q) {
    assert(q);
    assert(q->rightChild);

    Node_t* p = q->rightChild;
    q->rightChild = p->leftChild;
    p->leftChild = q;

    nodeFixHeight(p);
    nodeFixHeight(q);

    return p;
}

// Balances the given tree in terms of AVL
// @param node the given tree
// @return The pointer to the balanced tree
Node_t* nodeBalance(Node_t* node) {
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

Node_t* nodeInsert(Node_t* node, Elem_t key) {
    if (node == NULL) {
        // Create new node
        Node_t* temp = (Node_t*) calloc(1, sizeof(*temp));
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

Node_t* nodeFindMin(Node_t* node) {
    if (node->leftChild != NULL)
        return nodeFindMin(node->leftChild);
    else
        return node;
}

Node_t* nodeFindMax(Node_t* node) {
    if (node->rightChild != NULL)
        return nodeFindMax(node->rightChild);
    else
        return node;
}

// Removes the minimum element in the given node
// @param node the pointer to the node
// @return The pointer to the tree without the minimum node
// @note Does not free memory of this node
Node_t* nodeRemoveMin(Node_t* node) {
    if (node->leftChild == NULL)
        return node->rightChild;
    
    node->leftChild = nodeRemoveMin(node->leftChild);
    return nodeBalance(node);
}

Node_t* nodeRemove(Node_t* node, Elem_t key) {
    if (node == NULL)
        return NULL;
    
    if (key < node->key)
        node->leftChild = nodeRemove(node->leftChild, key);
    else if (key > node->key)
        node->rightChild = nodeRemove(node->rightChild, key);
    else {
        // Found the key
        Node_t* q = node->leftChild;
        Node_t* r = node->rightChild;
        free(node);
        if (r == NULL)
            return q;
        Node_t* min = nodeFindMin(r);
        min->rightChild = nodeRemoveMin(r);
        min->leftChild = q;
        return nodeBalance(min);
    }

    return nodeBalance(node);
}

int nodeForEach(Node_t* tree, int(*callback)(Node_t* node, void* data), void* data, treeTraversal type) {
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

void nodeDebugPrint(const Node_t* node) {
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
int print_node(Node_t* node, void* data) {
    printf("%d ", node->key);
    return EXIT_SUCCESS;
}

void nodePrint(const Node_t* node, treeTraversal type) {
    nodeForEach((Node_t*) node, print_node, NULL, type);
    printf("\n");
} 

void nodeDestroy(Node_t* node) {
    if (node == NULL)
        return ;
    
    nodeDestroy(node->leftChild);
    nodeDestroy(node->rightChild);
    free(node);
}