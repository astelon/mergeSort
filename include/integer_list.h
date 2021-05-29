/**
 * Integer implementation of a list structure
 */
#ifndef __INTEGER_LIST_H__
#define __INTEGER_LIST_H__
#include <stdint.h>
#include <stdlib.h>
#include "list.h"

/**
 * \brief Creates a Integer List Node with a given value
 * \param value Value to initialize the node.
 * \return      A pointer to the created node.
 */
struct ListNode *integerListNodeCreate(int32_t value);

/**
 * \brief Adds an integer to the begining of the list
 * \param list  The list to be modified
 * \param value Value to be added to the list
 * \return      RET_OK if it was successful or RET_FAIL on a failure.
 */
enum ListReturnType integerListAppendStart(struct List *list, int32_t value);

/**
 * \brief Adds an integer to the end of the list
 * \param list  The list to be modified
 * \param value Value to be added to the list
 * \return      RET_OK if it was successful or RET_FAIL on a failure.
 */
enum ListReturnType integerListAppendEnd(struct List *list, int32_t value);

void integerListFreeNode(struct ListNode *node);

/**
 * \brief Creates an Integer List and Initializes it with a set of values
 * \param count    Amount of elements to be added to the new list
 * \param elements Elements to be added to the list
 * \return         A pointer to a new list containing the given elements
 */
struct List* integerListCreateWithElements(size_t count, int32_t elements[]);

/**
 * \brief Print an element of an Integer List
 * \param value Pointer to the value to be printed
 * \param fmt   Pointer to a c string used as format to print the element.
 */
bool integerListPrintElement(void *value, void *fmt);

/**
 * \brief Print all elements in an integer List
 * \param list List to be printed
 */
void integerListPrint(struct List *list);

#endif //__INTEGER_LIST_H__
