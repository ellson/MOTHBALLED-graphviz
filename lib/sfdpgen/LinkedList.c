#include "general.h"
#include "LinkedList.h"


SingleLinkedList SingleLinkedList_new(void *data){
  SingleLinkedList head;
  head = MALLOC(sizeof(struct SingleLinkedList_struct));
  head->data = data;
  head->next = NULL;
  return head;
}

void SingleLinkedList_delete(SingleLinkedList head,  void (*linklist_deallocator)(void*)){
  SingleLinkedList next;

  if (!head) return;
  do {
    next = head->next;
    if (head->data) linklist_deallocator(head->data);
    if (head) FREE(head);
    head = next;
  } while (head);

}


SingleLinkedList SingleLinkedList_prepend(SingleLinkedList l, void *data){
  SingleLinkedList head = SingleLinkedList_new(data);
  head->next = l;
  return head;
}

void* SingleLinkedList_get_data(SingleLinkedList l){
  return l->data;
}

SingleLinkedList SingleLinkedList_get_next(SingleLinkedList l){
  return l->next;
}

