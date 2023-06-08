#include <stdlib.h>
#include "list.h"
#include "compiler.h"

struct Node {
    int val;
    struct Node* next;
};

/**
 * 创建一个仅含val的新链表，表示四元式数组的一个下标，
 * 函数返回指向这个链表的指针。
 */
List makelist(int val) {
    List l = (struct Node *) malloc(sizeof(struct Node));
    l->val = val;
    l->next = NULL;
    return l;
}

/**
 * 将以l1和l2为链首的两条链合并为一，返回合并后的链首。
 */
List merge(List l1, List l2) {
    if (l1 == NULL && l2 == NULL) return NULL;
    if (l1 == NULL) return l2;
    if (l2 == NULL) return l1;

    List p = l1;
    while (p->next != NULL)
        p = p->next;
    p->next = l2;
    return l1;
}

/**
 * “回填”，把l所链接的每个四元式的第四区段都填为addr，并释放链表。
 */
void backpatch(List l, char *addr) {
    while (l != NULL){
        backpatch_quad(l->val, addr);
        List lnext = l->next;
        free(l);
        l = lnext;
    }
}
