/*
 * Library implementing a list of numbers and a sorting argorithm
 *  A generic list is implemented and later specific wrappers are provided to
 *  create a list of integers.
 *  The selected algorithm for this implementation is merge sort on a
 *  linked list.
 *  Author: Luis Guillermo Marin Blanco
 *  Date: 06/22/2021
 *
 *  Assumptions:
 *      - The list contains only 32bit integer values.
 *      - At some point in the system, we could want to use the list in the
 *        insertion order so we are not interested in ordering the list on insert.
 *      - If at any time, we fail to reserve memoty, then the whole program will
 *        abort.
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "list.h"
#include "integer_list.h"
#include "utils.h"

/*************************** Sort ******************************/

/**
 * \brief Frees a list inside a listNode
 * \param root pointer to the list node containing a list
 */
void freeMultiListNode(struct ListNode *root){
    listDestroy((struct List *) root->value);
    free(root);
}

/**
 * \brief Creates a Node for a multi list, containing a List of Integers
 * \return A pointer to the created node.
 */
struct ListNode* integerMultiListNodeCreate(void){
    struct ListNode *node = listNodeCreate(listCreate(integerListFreeNode));
    return node;
}

/**
 * \brief Creates a List of Lists
 * \return A pointer to the newly created list.
 */
struct List* multiListCreate(void) {
    struct List *list = listCreate(freeMultiListNode);
    return list;
}

/**
 * \brief Print an element of an List of Lists
 * \param value Pointer to the value to be printed
 * \param fmt   Pointer to a c string used as format to print the element.
 */
bool multiListPrintElement(void *value, void *fmt) {
    if((value == NULL) || (fmt == NULL)){
        return false;
    }
    struct List *list = (struct List*) value;
    integerListPrint(list);
    printf("\n");
    return true;
}

/**
 * \brief Print all elements in acList of Lists
 * \param list List to be printed
 */
void multiListPrint(struct List *list){
    printf("\n{\n");
    listForEach(list, multiListPrintElement,"\n");
    printf("\n}\n");
}

/*
 * The compare function is used to determine if element a should be located
 * before (closer to the head) the element b. The idea to define this as a
 * callback is to allow a specific implementation for testing to be created.
 */
typedef bool IntegerCompareFunction(int32_t a, int32_t b);

/**
 * \brief Merge 2 ordered lists of integer elements into the right list.
 *
 * Merges 2 ordered lists of integers into the right list. The left list gets destroyed.
 * \param right The List that will get updated with the sorted values from both lists.
 * \param left  The list that is a source of values and will be consumed by the right list.
 */
void integerListMergeSortMerge(struct List **right, struct List *left, IntegerCompareFunction compare){
    struct ListNode *nodeA=NULL;
    struct ListNode *nodeB=NULL;
    nodeA = listPop(*right);
    nodeB = listPop(left);
    struct List *result = listCreate(integerListFreeNode);
    while((nodeA != NULL) || (nodeB!=NULL)){
        if(nodeA == NULL) {
            listAppendEnd(result, nodeB);
            nodeB = listPop(left);
            continue;
        } else if (nodeB == NULL){
            listAppendEnd(result, nodeA);
            nodeA = listPop(*right);
            continue;
        }
        if(compare(*((int32_t *)nodeA->value), *((int32_t *)nodeB->value))) {
            listAppendEnd(result,nodeA);
            nodeA = listPop(*right);
        } else {
            listAppendEnd(result, nodeB);
            nodeB = listPop(left);
        }
    }
    /* Destroy the input lists and leave the result in the right list */
    listDestroy(*right);
    *right = result;
    listDestroy(left);
}

/**
 * \brief A MergeSort implementation using the generic list in this file.
 * The basic algorithm is:
 *  1. Sepparate the list of integers in a list of lists of integers with only 1
 *  integer value each.
 *  2. Take out 2 lists of lists and merge them using integerListMergeSortMerge.
 *  3. When only 1 list remains in the list of lists, that list should contain
 *  all of the elements properly sorted, move those values back to the oriinal list.
 */
void integerListMergeSort(struct List *list, IntegerCompareFunction compare) {
    if(list->count <= 1) return;

    /*Create the list of lists*/
    struct List *integerMultiList = multiListCreate();

    /* Initialize the list of lists O(n)*/
    struct ListNode *node = listPop(list);
    while(node != NULL){
        struct ListNode *tlist = integerMultiListNodeCreate();
        listAppendEnd((struct List*)tlist->value, node);
        listAppendEnd(integerMultiList, tlist);
        node = listPop(list);
    }

    /* Take 2 lists and merge them, store the result to be used in the nexr iteration*/
    while(integerMultiList->count > 1) {
        struct ListNode *aList = listPop(integerMultiList);
        struct List* nextIntegerMultiList = multiListCreate(); /*Temporary list to hold the result */
        while(aList != NULL){
            struct ListNode *bList = listPop(integerMultiList);
            if(bList != NULL){
                integerListMergeSortMerge((struct List**)(&aList->value), (struct List *)bList->value, compare);
                /* bList has been merged into aList, the MergeSort merge has deleted the contents of bList,
                 * but it is still required to free the node*/
                free(bList);
                bList = NULL;
            }
            /* Append at the end */
            listAppendEnd(nextIntegerMultiList, aList);
            aList = listPop(integerMultiList);
        }

        listDestroy(integerMultiList); /*This list should be empty already, free it */
        integerMultiList = nextIntegerMultiList; /* Update the integerMultiList for the next cycle */
    }

    /* Copy the result to the original list O(n)*/
    struct ListNode *rNode = listPop(integerMultiList);
    struct List *rList = (struct List*) rNode->value;
    node = listPop(rList);
    while(node != NULL){
        listAppendEnd(list, node);
        node = listPop(rList);
    }
    /* Free memory for the temporary variables */
    listDestroy(rList);
    free(rNode);
    listDestroy(integerMultiList);
    integerMultiList = NULL;
}

typedef void SortFunction(struct List *list, IntegerCompareFunction compare);

void naiveSort(struct List *list, IntegerCompareFunction compare) {
    struct ListNode *pivot = list->head;
    //if(pivot == NULL) return;
    while(pivot!=NULL){
        struct ListNode *itr = pivot->next;
        while(itr!=NULL){
            if(!compare(*(int32_t*)pivot->value, *(int32_t*)itr->value)){
                listNodeSwapValues(pivot,itr);
            }
            itr = itr->next;
        }
        pivot = pivot->next;
    }
}
/**
 * \brief Compare function for production code
 * \param a value to be compared
 * \param b value to be compared
 * \return true if a<b
 */
bool lessThan(int32_t a, int32_t b) {
    return a < b;
}

/*************************** Testing ******************************/

#define ARRAY_SIZE(x) sizeof((x))/sizeof((x)[0])

struct TestExpectedValueData {
    size_t size;                /* Expected size of the list */
    int32_t *expectedValues;    /* List of ordered values expected by the test */
    size_t iterator;            /* Index of the current value iterated in the list */
    bool result;                /* Total result of the comparison */
};

static int comparisons = 0;

void resetComparisons(void){
    comparisons = 0;
}

int getComparisons(void){
    return comparisons;
}

/**
 * \brief Compare function for testing
 * This compare function counts the amount of comparisons made to report it
 * back to the test.
 */
bool lessThanForTesting(int32_t a, int32_t b) {
    comparisons++;
    return a < b;
}

/**
 * \brief Callback for comparing one of the values in a List to an array of expected values.
 * \param value Pointer to the value stored in the list
 * \param data  Pointer to a TestExpectedValueData structure containing the status
 *              of the iteration and comparison.
 */
bool checkExpectedElement(void *value, void *data){
    int32_t v = *(int32_t *)value;
    struct TestExpectedValueData *testData = (struct TestExpectedValueData *) data;
    size_t i = testData->iterator;
    if(i >= testData->size){
        testData->result = false;
        return false;
    }
    if(v != testData->expectedValues[i]){
        testData->result = false;
        return false;
    }
    testData->iterator++; /* increment the index for the next element */
    return true;
}

/**
 * \brief Compares all elements in a list with their expected values.
 * \param size Size of the expected values array
 * \param values list of Integer values
 * \param expected An array representing the expected output of the sorting algorithm,
 */
bool compareTestResults(size_t size, struct List *values, int32_t *expected){
    struct TestExpectedValueData data = {
        .size = size,
        .expectedValues = expected,
        .iterator = 0,
        .result = true
    };
    listForEach(values, checkExpectedElement, &data);
    return data.result;
}

/**
 * \brief Run a Sort test case
 * This function creates a list based on a set of integers, sorts the list and
 * compares the output against an expected output.
 * \param iteration Number of the test to be printed
 * \param size      Size of both the values and expected arrays
 * \param values    Initial state of the integer list
 * \param expected  Expected final state for the integer list
 */
void runSortTest(int iteration, size_t size, SortFunction sortFunction, int32_t *values, int32_t *expected){
    printf("\n-- Test %d --\n", iteration);

    resetComparisons();
    struct List *list = integerListCreateWithElements(size, values);
    printf("List Size = %lu\n", (unsigned long int) size);
    printf("Input:\n");
    if(size<100) integerListPrint(list);
    else printf("Too large to be printed\n");

    clock_t start, end;
    double cpu_time_used;

    start = clock();
    sortFunction(list, lessThanForTesting);
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

    printf("Output:\n");
    if(size<100) integerListPrint(list);
    else printf("Too large to be printed\n");

    bool succeeded = compareTestResults(size, list, expected);
    listDestroy(list);
    printf("\nComparisons = %d\n",getComparisons());

    printf("Condition: %s\n", succeeded ? "PASSED" : "FAILED");

    printf("Resolved sort in %.3f seconds\n", cpu_time_used);

    if(!succeeded){
        abort();
    }
}

#define TEST3_ARRAY_SIZE 100000

int main(int argc, const char **argv){
    int32_t test1[]         = {1, 18, 3, 7, 9, 6, 106, 2, 75, 10, 5, -1};
    int32_t test1Expected[] = {-1, 1, 2, 3, 5, 6, 7, 9, 10, 18, 75, 106};
    int32_t test2[]         = {4, 18764, -3245, 75321, 9784, 631, 106, 20, 35, 109, 575, 4, -118, 20789, 2, 18};
    int32_t test2Expected[] = {-3245, -118, 2, 4, 4, 18, 20, 35, 106, 109, 575, 631, 9784, 18764, 20789, 75321};
    int32_t test3[TEST3_ARRAY_SIZE] = {};
    int32_t test3Expected[TEST3_ARRAY_SIZE] = {};

    runSortTest(0, ARRAY_SIZE(test1), integerListMergeSort, test1, test1Expected);
    runSortTest(1, ARRAY_SIZE(test1), naiveSort, test1, test1Expected);
    runSortTest(2, ARRAY_SIZE(test2), integerListMergeSort, test2, test2Expected);
    runSortTest(3, ARRAY_SIZE(test2), naiveSort, test2, test2Expected);

    //Fill array with increased values, the test array will be filled in decreasing order and we expect it to be in ascending order.
    for (int i=0; i<TEST3_ARRAY_SIZE; i++) {
        test3Expected[i] = i;
        test3[(TEST3_ARRAY_SIZE-1)-i] = i;
    }

    runSortTest(4, ARRAY_SIZE(test3), integerListMergeSort, test3, test3Expected);
    runSortTest(5, ARRAY_SIZE(test3), naiveSort, test3, test3Expected);

    return 0;
}
