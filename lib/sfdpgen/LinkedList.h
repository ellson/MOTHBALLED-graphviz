#ifndef LINKED_LIST_H
#define LINKED_LIST_H

typedef struct SingleLinkedList_struct* SingleLinkedList;

struct SingleLinkedList_struct {
  void *data;
  SingleLinkedList next;
};

SingleLinkedList SingleLinkedList_new(void *data);
void SingleLinkedList_delete(SingleLinkedList head, void (*linklist_deallocator)(void*));
SingleLinkedList SingleLinkedList_prepend(SingleLinkedList l, void *data);

void* SingleLinkedList_get_data(SingleLinkedList l);

SingleLinkedList SingleLinkedList_get_next(SingleLinkedList l);

#endif
