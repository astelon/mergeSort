/**
 * A double linked list implementation containing a list of lists of integers
 */
#ifndef __MERGE_SORT_H__
#define __MERGE_SORT_H__

#include "list.h"

/**
 * \brief Frees a list inside a listNode
 * \param root pointer to the list node containing a list
 */
void freeMultiListNode(struct ListNode *root);

/**
 * \brief Creates a Node for a multi list, containing a List of Integers
 * \return A pointer to the created node.
 */
struct ListNode* integerMultiListNodeCreate(void);

/**
 * \brief Creates a List of Lists
 * \return A pointer to the newly created list.
 */
struct List* multiListCreate(void);

/**
 * \brief Print an element of an List of Lists
 * \param value Pointer to the value to be printed
 * \param fmt   Pointer to a c string used as format to print the element.
 */
bool multiListPrintElement(void *value, void *fmt);

/**
 * \brief Print all elements in acList of Lists
 * \param list List to be printed
 */
void multiListPrint(struct List *list);

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
void integerListMergeSortMerge(struct List **right, struct List *left, IntegerCompareFunction compare);

/**
 * \brief A MergeSort implementation using the generic list in this file.
 * The basic algorithm is:
 *  1. Sepparate the list of integers in a list of lists of integers with only 1
 *  integer value each.
 *  2. Take out 2 lists of lists and merge them using integerListMergeSortMerge.
 *  3. When only 1 list remains in the list of lists, that list should contain
 *  all of the elements properly sorted, move those values back to the oriinal list.
 */
void integerListMergeSort(struct List *list, IntegerCompareFunction compare);

/**
 * \brief Data type for the sorting function
 */
typedef void SortFunction(struct List *list, IntegerCompareFunction compare);

/**
 * \brief A naive implementation of a sort function with O(n^2) complexity
 */
void naiveSort(struct List *list, IntegerCompareFunction compare);

/**
 * Default implementation to compare integer list elements
 */
bool lessThan(int32_t a, int32_t b);

#endif //__MERGE_SORT_H__
