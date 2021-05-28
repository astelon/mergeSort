/**
 * A generic linked list to be used in several algorithms
 */

#ifndef __LIST_H__
#define __LIST_H__
#include <stdbool.h>

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
struct ListNode *listNodeCreate(void *value);

/**
 * \brief Creates an List
 * \param freeNode A callback function to be run to free a node. If set to NULL, just use standard free.
 * \return A pointer to the newly created list.
 */
struct List* listCreate(ListFreeNodeCallback freeNode);

/**
 * \brief Adds a node to the begining of the list
 * \param list  The list to be modified
 * \param node  Node to be added to the list
 * \return      RET_OK if it was successful or RET_FAIL on a failure.
 */
enum ListReturnType listAppendStart(struct List *list, struct ListNode *node);

/**
 * \brief Adds a node to the end of the list
 * \param list  The list to be modified
 * \param node  Node to be added to the list
 * \return      RET_OK if it was successful or RET_FAIL on a failure.
 */
enum ListReturnType listAppendEnd(struct List *list, struct ListNode *node);

/**
 * \brief Insert a Node after another one
 * \param list     The list to be modified
 * \param previous A pointer to the previous node
 * \param node     A pointer to the node to be inserted
 * \return         RET_OK if it was succesfull or RET_FAIL otherwise.
 */
enum ListReturnType listInsertAfter(struct List* list, struct ListNode *previous, struct ListNode *node);

/**
 * \brief Insert a Node before another one
 * \param list     The list to be modified
 * \param previous A pointer to the next node
 * \param node     A pointer to the node to be inserted
 * \return         RET_OK if it was succesfull or RET_FAIL otherwise.
 */
enum ListReturnType listInsertBefore(struct List* list, struct ListNode *next, struct ListNode *node);

/**
 * \brief Get the first element from the list and remove it from the list
 * \param list The list to be modified
 * \return     A pointer to the removed node
 */
struct ListNode *listPop(struct List *list);

/**
 * \brief Frees the memory related to a list
 * \param list The list to be freed
 */
void listDestroy(struct List *list);

/**
 * \brief The callback to be called for each element in the list
 */
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
void listForEach(struct List *list, ListCallback callback, void *userData);

/**
 * \brief Swap the contents of 2 given list nodes
 * \param nodeA One of the nodes to swap their values
 * \param nodeB The other node involved
 */
void listNodeSwapValues(struct ListNode *nodeA, struct ListNode *nodeB);

#endif //__LIST_H__
