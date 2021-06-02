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
#include "merge_sort.h"

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
