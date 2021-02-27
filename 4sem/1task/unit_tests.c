#include "avltree.h"

#define UTESTS_NUM 7

int unit_test_1() {
    avlTree_p tree = avlTreeCreate();
    if (tree == NULL)
        return EXIT_FAILURE;

    for (int i = 0; i < 20; ++i) {
        avlTreeInsert(tree, 2 * i + 1);
    }

    avlTreePrint(tree, LNR);
    avlTreePrint(tree, RNL);
    avlTreePrint(tree, NLR);
    avlTreePrint(tree, LRN);

    avlTreeDestroy(&tree);
    return EXIT_SUCCESS;
}

int unit_test_2() {
    avlTree_p tree = avlTreeCreate();
    if (tree == NULL)
        return EXIT_FAILURE;

    for (int i = 20; i > 0; --i) {
        avlTreeInsert(tree, 2 * i + 1);
    }
    
    avlTreePrint(tree, LNR);
    avlTreePrint(tree, RNL);
    avlTreePrint(tree, NLR);
    avlTreePrint(tree, LRN);

    avlTreeDestroy(&tree);
    return EXIT_SUCCESS;
}

int unit_test_3() {
    avlTree_p tree = avlTreeCreate();
    if (tree == NULL)
        return EXIT_FAILURE;

    int max = 0;
    int min = 0;

    for (int i = 0; i < 20; ++i) {
        avlTreeInsert(tree, 2 * i + 1);
    }

    max = avlTreeFindMax(tree);
    min = avlTreeFindMin(tree);

    if (max != 39 || min != 1) {
        avlTreeDestroy(&tree);
        return EXIT_FAILURE;
    }
    else {
        avlTreeDestroy(&tree);
        return EXIT_SUCCESS;
    }
}

int unit_test_4() {
    avlTree_p tree = avlTreeCreate();
    if (tree == NULL)
        return EXIT_FAILURE;

    for (int i = 0; i < 20; ++i) {
        avlTreeInsert(tree, i);
    }

    for (int i = 0; i < 20; ++i) {
        avlTreeInsert(tree, i);
    }

    avlTreeDestroy(&tree);
    return EXIT_SUCCESS;
}

int unit_test_5() {
    avlTree_p tree = avlTreeCreate();
    if (tree == NULL)
        return EXIT_FAILURE;

    int numbers[20] = {0};

    avlTreeRemove(tree, 0);
    for (int i = 0; i < 20; ++i) {
        numbers[i] = random() % 100;
        avlTreeInsert(tree, numbers[i]);
    }

    for (int i = 0; i < 20; ++i) {
        avlTreeRemove(tree, numbers[i]);
    }

    avlTreeDestroy(&tree);
    return EXIT_SUCCESS;
}

int callback_ut6(Node_p node, void* data) {
    return EXIT_FAILURE;
}

int unit_test_6() {
    avlTree_p tree = avlTreeCreate();
    if (tree == NULL)
        return EXIT_FAILURE;

    int result = 0;

    for (int i = 0; i < 20; ++i) {
        avlTreeInsert(tree, i);
    }

    printf("Size: %ld\n", avlTreeSize(tree));
    result &= avlTreeForEach(tree, callback_ut6, NULL, LNR);
    result &= avlTreeForEach(tree, callback_ut6, NULL, RNL);
    result &= avlTreeForEach(tree, callback_ut6, NULL, NLR);
    result &= avlTreeForEach(tree, callback_ut6, NULL, LRN);
    result &= !avlTreeForEach(tree, callback_ut6, NULL, 9);

    avlTreeDestroy(&tree);
    return result;
}

int callback_ut7(Node_p node, void* data) {
    if (nodeKey(node) % 2 == 0) {
        nodeDebugPrint(node);
        return EXIT_SUCCESS;
    }
    else
        return EXIT_FAILURE;
}

int unit_test_7() {
    avlTree_p tree = avlTreeCreate();
    if (tree == NULL)
        return EXIT_FAILURE;
        
    int result = 0;

    for (int i = 0; i < 20; ++i)
        avlTreeInsert(tree, 2 * i); 
    
    avlTreeInsert(tree, 51);
    printf("The found element adress: %p", avlTreeFind(tree, 10));
    result &= avlTreeForEach(tree, callback_ut7, NULL, LNR);
    result &= avlTreeForEach(tree, callback_ut7, NULL, RNL);
    result &= avlTreeForEach(tree, callback_ut7, NULL, NLR);
    result &= avlTreeForEach(tree, callback_ut7, NULL, LRN);

    avlTreeDestroy(&tree);
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
    printf("Tests number: %d\n", UTESTS_NUM);

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