#ifndef LIST_H_
#define LIST_H_

struct Node;
typedef struct Node *List;

List makelist(int val);
List merge(List l1, List l2);
void backpatch(List l, char *addr);

#endif
