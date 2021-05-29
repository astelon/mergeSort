#include "integer_list.h"
#include "merge_sort.h"
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>

void freeMultiListNode(struct ListNode *root){
    listDestroy((struct List *) root->value);
    free(root);
}

struct ListNode* integerMultiListNodeCreate(void){
    struct ListNode *node = listNodeCreate(listCreate(integerListFreeNode));
    return node;
}

struct List* multiListCreate(void) {
    struct List *list = listCreate(freeMultiListNode);
    return list;
}

bool multiListPrintElement(void *value, void *fmt) {
    if((value == NULL) || (fmt == NULL)){
        return false;
    }
    struct List *list = (struct List*) value;
    integerListPrint(list);
    printf("\n");
    return true;
}

void multiListPrint(struct List *list){
    printf("\n{\n");
    listForEach(list, multiListPrintElement,"\n");
    printf("\n}\n");
}

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

bool earessThan(int32_t a, int32_t b) {
    return a < b;
}

