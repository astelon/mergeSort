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

/***************************  Utils  ******************************/
/**
 * \brief Reserve memory blocks of a given size or abort on failure.
 * \param count Memory blocks to reserve
 * \param size  Size of every memory block
 * \return      A pointer to the allocated memory block.
 */
void *xzalloc(size_t count, size_t size){
    /* calloc initializes the new memory to 0  so there is no need to
     * explicitly do it here */
    void *ptr = calloc(count, size);
    if(ptr == NULL){
        fprintf(stderr, "Error: Failed to reserve memory");
        abort();
    }
    return ptr;
}

/*************************** Generic List ******************************/

enum ListReturnType {
    RET_OK,
    RET_FAIL
};

struct ListNode {
    void *value;           /* Value stored */
    struct ListNode *next; /* Next element of the list */
    struct ListNode *prev; /* Previous node in the list */
};

typedef void ListFreeNodeCallback(struct ListNode *);

struct List {
    struct ListNode *head; /* First element of the list */
    struct ListNode *tail; /* Last element of the list, changes with every insertion */
    size_t count;          /* Element count */
    ListFreeNodeCallback *freeNode; /* Callback function to free a node */
};

/**
 * \brief Creates a List node with a value
 * \param value Pointer to a value for the element in the node
 * \return A pointer to the newly created list.
 */
struct ListNode *listNodeCreate(void *value){
    struct ListNode *node = (struct ListNode *) xzalloc(1, sizeof(struct ListNode));
    node->value = value;
    return node;
}

/**
 * \brief Creates an List
 * \param freeNode A callback function to be run to free a node. If set to NULL, just use standard free.
 * \return A pointer to the newly created list.
 */
struct List* listCreate(ListFreeNodeCallback freeNode) {
    struct List *list = (struct List *) xzalloc(1, sizeof(struct List));
    list->freeNode = freeNode;
    return list;
}

/**
 * \brief Adds a node to the begining of the list
 * \param list  The list to be modified
 * \param node  Node to be added to the list
 * \return      RET_OK if it was successful or RET_FAIL on a failure.
 */
enum ListReturnType listAppendStart(struct List *list, struct ListNode *node){
    if(list == NULL) return RET_FAIL;
    if(list->tail == NULL) { /* This should only happen when the list is empty */
        list->tail = node;
    }
    if(list->head!=NULL) {
        list->head->prev = node;
    }
    node->next = list->head;
    list->head = node;

    list->count++;
    return RET_OK;
}

/**
 * \brief Adds a node to the end of the list
 * \param list  The list to be modified
 * \param node  Node to be added to the list
 * \return      RET_OK if it was successful or RET_FAIL on a failure.
 */
enum ListReturnType listAppendEnd(struct List *list, struct ListNode *node){
    if(list == NULL) return RET_FAIL;
    if(list->head == NULL) { /* This should only happen when the list is empty */
        list->head = node;
    }
    if(list->tail != NULL){
        node->prev = list->tail;
        list->tail->next = node;
    }
    list->tail = node;
    list->count++;
    return RET_OK;
}

/**
 * \brief Insert a Node after another one
 * \param list     The list to be modified
 * \param previous A pointer to the previous node
 * \param node     A pointer to the node to be inserted
 * \return         RET_OK if it was succesfull or RET_FAIL otherwise.
 */
enum ListReturnType listInsertAfter(struct List* list, struct ListNode *previous, struct ListNode *node) {
    if(list == NULL) return RET_FAIL;
    if(previous == NULL) return RET_FAIL;
    if(list->tail == previous) {
        list->tail = node;
    }
    if(previous->next!=NULL){
        previous->next->prev = node;
    }

    node->next = previous->next;
    node->prev = previous;

    previous->next = node;
    list->count++;
    return RET_OK;
}

/**
 * \brief Insert a Node before another one
 * \param list     The list to be modified
 * \param previous A pointer to the next node
 * \param node     A pointer to the node to be inserted
 * \return         RET_OK if it was succesfull or RET_FAIL otherwise.
 */
enum ListReturnType listInsertBefore(struct List* list, struct ListNode *next, struct ListNode *node) {
    if(list == NULL) return RET_FAIL;
    if(next == NULL) return RET_FAIL;
    if(list->head == next) {
        list->head = node;
    }
    if(next->prev!=NULL){
        next->prev->next = node;
    }
    node->next = next;
    next->prev = node;
    list->count++;
    return RET_OK;
}
/**
 * \brief Get the first element from the list and remove it from the list
 * \param list The list to be modified
 * \return     A pointer to the removed node
 */
struct ListNode *listPop(struct List *list){
    struct ListNode *node = list->head;
    if(list->head == NULL) return NULL;
    if(list->count == 0) return NULL;
    if(list->head == list->tail) {
        //Only 1 element is left
        list->tail=NULL;
    }
    list->head = list->head->next;
    if(list->head!=NULL) {
        list->head->prev = NULL;
    }
    node->next = NULL;
    node->prev = NULL;
    list->count--;
    return node;
}

/**
 * \brief Frees the memory related to a list
 * \param list The list to be freed
 */
void listDestroy(struct List *list){
    /* Fist free all elements */
    struct ListNode *node = listPop(list);
    while(node != NULL){
        if(list->freeNode != NULL){
            list->freeNode(node);
        } else {
            printf("freeNode was null\n");
            free(node);
        }
        node = listPop(list);
    }
    /* Then free the list */
    free(list);
}

typedef bool ListCallback(void *value, void *userData);

/**
 * \brief Run the given callback function on all elements.
 * Run the callback function on every element in the list. The provided userData
 * pointer is passed to the callback function for all values. The callback should
 * return true to continue with the next element or false to end the iteration.
 * \param list     List containing the elements
 * \param callback Callback function to be called for each element
 * \param userData User data to be passed for every element
 */
void listForEach(struct List *list, ListCallback callback, void *userData){
    if(list==NULL || list->head==NULL) return;
    struct ListNode *node = list->head;
    while(node != NULL){
        if(callback(node->value, userData)) {
            node = node->next;
        }else{
            node = NULL;
        }
    }
}

/**
 * \brief Swap the contents of 2 given list nodes
 * \param nodeA One of the nodes to swap their values
 * \param nodeB The other node involved
 */
void listNodeSwapValues(struct ListNode *nodeA, struct ListNode *nodeB) {
    if((nodeA == NULL) || (nodeB==NULL)) {
        return;
    }
    void *tmp = nodeA->value;
    nodeA->value = nodeB->value;
    nodeB->value = tmp;
}


/*************************** Integer List ******************************/
/**
 * \brief Creates a Integer List Node with a given value
 * \param value Value to initialize the node.
 * \return      A pointer to the created node.
 */
struct ListNode *integerListNodeCreate(int32_t value){
    int32_t *ptr = (int32_t *)xzalloc(1,sizeof(int32_t));
    *ptr = value;
    struct ListNode *node = listNodeCreate(ptr);
    return node;
}

/**
 * \brief Adds an integer to the begining of the list
 * \param list  The list to be modified
 * \param value Value to be added to the list
 * \return      RET_OK if it was successful or RET_FAIL on a failure.
 */
enum ListReturnType integerListAppendStart(struct List *list, int32_t value){
    if(list == NULL) return RET_FAIL;
    struct ListNode *node = integerListNodeCreate(value);
    return listAppendStart(list, node);
}

/**
 * \brief Adds an integer to the end of the list
 * \param list  The list to be modified
 * \param value Value to be added to the list
 * \return      RET_OK if it was successful or RET_FAIL on a failure.
 */
enum ListReturnType integerListAppendEnd(struct List *list, int32_t value){
    if(list == NULL) return RET_FAIL;
    struct ListNode *node = integerListNodeCreate(value);
    return listAppendEnd(list,node);
}

void integerListFreeNode(struct ListNode *node){
    free(node->value);
    free(node);
}

/**
 * \brief Creates an Integer List and Initializes it with a set of values
 * \param count    Amount of elements to be added to the new list
 * \param elements Elements to be added to the list
 * \return         A pointer to a new list containing the given elements
 */
struct List* integerListCreateWithElements(size_t count, int32_t elements[]) {
    struct List *list = listCreate(integerListFreeNode);
    for(size_t i=0; i<count; i++) {
        integerListAppendEnd(list, elements[i]);
    }
    return list;
}

/**
 * \brief Print an element of an Integer List
 * \param value Pointer to the value to be printed
 * \param fmt   Pointer to a c string used as format to print the element.
 */
bool integerListPrintElement(void *value, void *fmt) {
    if((value == NULL) || (fmt == NULL)){
        return false;
    }
    int32_t *ivalue = (int32_t *)value;
    printf((const char*)fmt, *ivalue);
    return true;
}

/**
 * \brief Print all elements in an integer List
 * \param list List to be printed
 */
void integerListPrint(struct List *list){
    printf("[");
    listForEach(list, integerListPrintElement," %d ");
    printf("]\n");
}

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
bool runSortTest(int iteration, size_t size, SortFunction sortFunction, int32_t *values, int32_t *expected){
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

    bool result = compareTestResults(size, list, expected);
    listDestroy(list);
    printf("\nComparisons = %d\n",getComparisons());

    printf("Condition: %s\n", result ? "PASSED" : "FAILED");

    printf("Resolved sort in %.3f seconds\n", cpu_time_used);

    return result;
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
