#include "integer_list.h"
#include "utils.h"
#include <stdio.h>

struct ListNode *integerListNodeCreate(int32_t value){
    int32_t *ptr = (int32_t *)xzalloc(1,sizeof(int32_t));
    *ptr = value;
    struct ListNode *node = listNodeCreate(ptr);
    return node;
}

enum ListReturnType integerListAppendStart(struct List *list, int32_t value){
    if(list == NULL) return RET_FAIL;
    struct ListNode *node = integerListNodeCreate(value);
    return listAppendStart(list, node);
}

enum ListReturnType integerListAppendEnd(struct List *list, int32_t value){
    if(list == NULL) return RET_FAIL;
    struct ListNode *node = integerListNodeCreate(value);
    return listAppendEnd(list,node);
}

void integerListFreeNode(struct ListNode *node){
    free(node->value);
    free(node);
}

struct List* integerListCreateWithElements(size_t count, int32_t elements[]) {
    struct List *list = listCreate(integerListFreeNode);
    for(size_t i=0; i<count; i++) {
        integerListAppendEnd(list, elements[i]);
    }
    return list;
}

bool integerListPrintElement(void *value, void *fmt) {
    if((value == NULL) || (fmt == NULL)){
        return false;
    }
    int32_t *ivalue = (int32_t *)value;
    printf((const char*)fmt, *ivalue);
    return true;
}

void integerListPrint(struct List *list){
    printf("[");
    listForEach(list, integerListPrintElement," %d ");
    printf("]\n");
}
