#include "avltree.h"

int main() {
    Node tree;
    nodeInit(&tree, 10);

    nodeInsert(&tree, 20);
    nodeInsert(&tree, 30);
    nodeInsert(&tree, 40);

    nodePrint(&tree);
    return 0;
}