#pragma once

#include <assert.h>
#include <stdlib.h>
#include "stdio.h"

typedef struct Node {
    int key;
    unsigned char height;
    struct Node* leftChild;
    struct Node* rightChild;
} Node;

void nodeInit(Node* node, int key);
Node* nodeRemove(Node* node, int key);
Node* nodeInsert(Node* node, int key);
void nodePrint(Node* node);
void nodeDestroy(Node* node);
