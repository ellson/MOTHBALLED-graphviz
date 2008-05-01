/* vim:set shiftwidth=4 ts=8: */

/**********************************************************
*      This software is part of the graphviz package      *
*                http://www.graphviz.org/                 *
*                                                         *
*            Copyright (c) 1994-2004 AT&T Corp.           *
*                and is licensed under the                *
*            Common Public License, Version 1.0           *
*                      by AT&T Corp.                      *
*                                                         *
*        Information and Software Systems Research        *
*              AT&T Research, Florham Park NJ             *
**********************************************************/

#include "LinkedList.h"
#include "memory.h"

SingleLinkedList SingleLinkedList_new(void *data)
{
    SingleLinkedList head;
    head = GNEW(struct SingleLinkedList_s);
    head->data = data;
    head->next = NULL;
    return head;
}

void SingleLinkedList_delete(SingleLinkedList  head,
			     void (*linklist_deallocator) (void *))
{
    SingleLinkedList next;

    if (!head)
	return;
    do {
	next = head->next;
	if (head->data)
	    linklist_deallocator(head->data);
	if (head)
	    free(head);
	head = next;
    } while (head);

}


SingleLinkedList SingleLinkedList_prepend(SingleLinkedList  l,
					   void *data)
{
    SingleLinkedList head = SingleLinkedList_new(data);
    head->next = l;
    return head;
}

void *SingleLinkedList_get_data(SingleLinkedList  l)
{
    return l->data;
}

SingleLinkedList SingleLinkedList_get_next(SingleLinkedList  l)
{
    return l->next;
}
