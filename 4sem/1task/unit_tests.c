#include "avltree.h"

#define UTESTS_NUM 7

// New functions used:
// nodeInsert -> nodeInit
//            -> nodeBalance -> nodeFixHeight           -> nodeHeight
//                           -> nodeBFactor             -> nodeHeight
//                           -> nodeRotateLeft/Right    -> nodeFixHeight -> nodeHeight
// nodePrint -> nodeForEach
//           -> nodePrintNode 
// nodeDestroy  
int unit_test_1() {
    Node_t* tree = NULL;

    for (int i = 0; i < 10; ++i) {
        tree = nodeInsert(tree, 2 * i + 1);
    }

    nodePrint(tree, LNR);
    nodePrint(tree, RNL);
    nodePrint(tree, NLR);
    nodePrint(tree, LRN);

    nodeDestroy(tree);
    return EXIT_SUCCESS;
}

int unit_test_2() {
    Node_t* tree = NULL;

    for (int i = 10; i > 0; --i) {
        tree = nodeInsert(tree, 2 * i + 1);
    }
    
    nodePrint(tree, LNR);
    nodePrint(tree, RNL);
    nodePrint(tree, NLR);
    nodePrint(tree, LRN);

    nodeDestroy(tree);
    return EXIT_SUCCESS;
}

// New functions used:
// nodeFindMin
// nodeFindMax
int unit_test_3() {
    Node_t* tree = NULL;
    Node_t* max = NULL;
    Node_t* min = NULL;

    for (int i = 0; i < 10; ++i) {
        tree = nodeInsert(tree, 2 * i + 1);
    }

    max = nodeFindMax(tree);
    nodeDebugPrint(max);
    min = nodeFindMin(tree);
    nodeDebugPrint(min);

    if (max->key != 19 || min->key != 1) {
        nodeDestroy(tree);
        return EXIT_FAILURE;
    }
    else {
        nodeDestroy(tree);
        return EXIT_SUCCESS;
    }
}

// New functions used:
// nodeRemove -> nodeRemoveMin
int unit_test_4() {
    Node_t* tree = NULL;

    for (int i = 0; i < 10; ++i) {
        tree = nodeInsert(tree, i);
    }

    for (int i = 0; i < 10; ++i) {
        tree = nodeRemove(tree, i);
    }

    nodeDestroy(tree);
    return EXIT_SUCCESS;
}

// Other cases needed: 
// nodeBalance
// nodeRemoveMin
// nodeRemove
int unit_test_5() {
    Node_t* tree = NULL;
    int numbers[10] = {0};

    tree = nodeRemove(tree, 0);
    for (int i = 0; i < 10; ++i) {
        numbers[i] = random() % 100;
        tree = nodeInsert(tree, numbers[i]);
    }

    for (int i = 0; i < 10; ++i) {
        tree = nodeRemove(tree, numbers[i]);
    }

    nodeDestroy(tree);
    return EXIT_SUCCESS;
}

// Other cases needed:
// nodeForEach

int callback_ut6(Node_t* node, void* data) {
    return EXIT_FAILURE;
}

int unit_test_6() {
    Node_t* tree = NULL;
    int result = 0;

    for (int i = 0; i < 10; ++i) {
        tree = nodeInsert(tree, i);
    }

    result &= nodeForEach(tree, callback_ut6, NULL, LNR);
    result &= nodeForEach(tree, callback_ut6, NULL, RNL);
    result &= nodeForEach(tree, callback_ut6, NULL, NLR);
    result &= nodeForEach(tree, callback_ut6, NULL, LRN);

    nodeDestroy(tree);
    return result;
}

// Other cases needed:
// nodeForEach

int callback_ut7(Node_t* node, void* data) {
    if (node->key % 2 == 0)
        return EXIT_SUCCESS;
    else
        return EXIT_FAILURE;
}

int unit_test_7() {
    Node_t* tree = NULL;
    int result = 0;

    for (int i = 0; i < 10; ++i)
        tree = nodeInsert(tree, 2 * i);
    
    tree = nodeInsert(tree, 51);
    result &= nodeForEach(tree, callback_ut7, NULL, LNR);
    result &= nodeForEach(tree, callback_ut7, NULL, RNL);
    result &= nodeForEach(tree, callback_ut7, NULL, NLR);
    result &= nodeForEach(tree, callback_ut7, NULL, LRN);

    nodeDestroy(tree);
    return result;
}

void print_result(int result) {
    if (result == EXIT_SUCCESS)
        printf("SUCCESS\n");
    else
        printf("FAILURE\n");
}

int main() {
    int result = 0;

    printf("==================\n");
    printf("Test number: %d\n", UTESTS_NUM);

    result = unit_test_1();
    printf("Test 1: ");
    print_result(result);

    result = unit_test_2();
    printf("Test 2: ");
    print_result(result);

    result = unit_test_3();
    printf("Test 3: ");
    print_result(result);

    result = unit_test_4();
    printf("Test 4: ");
    print_result(result);

    result = unit_test_5();
    printf("Test 5: ");
    print_result(result);

    result = unit_test_6();
    printf("Test 6: ");
    print_result(result);
    
    result = unit_test_7();
    printf("Test 7: ");
    print_result(result);

    return EXIT_SUCCESS;
}