#include <stddef.h>
#include <stdlib.h>
#include "utils.h"
#include "list.h"
#include <stdio.h>

struct ListNode *listNodeCreate(void *value){
    struct ListNode *node = (struct ListNode *) xzalloc(1, sizeof(struct ListNode));
    node->value = value;
    return node;
}

struct List* listCreate(ListFreeNodeCallback freeNode) {
    struct List *list = (struct List *) xzalloc(1, sizeof(struct List));
    list->freeNode = freeNode;
    return list;
}

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

void listNodeSwapValues(struct ListNode *nodeA, struct ListNode *nodeB) {
    if((nodeA == NULL) || (nodeB==NULL)) {
        return;
    }
    void *tmp = nodeA->value;
    nodeA->value = nodeB->value;
    nodeB->value = tmp;
}
