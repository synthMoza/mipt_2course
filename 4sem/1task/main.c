#include "avltree.h"

int main() {
    Node* tree = NULL;
    int n = 0;
    int elem = 0;

    scanf("%d", &n);
    for (int i = 0; i < n; ++i) {
        scanf("%d", &elem);
        tree = nodeInsert(tree, elem);
    }

    nodePrint(tree, LNR);

    nodeDestroy(tree);
    return 0;
}