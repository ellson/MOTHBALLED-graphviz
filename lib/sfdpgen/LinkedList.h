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

#ifndef LINKED_LIST_H
#define LINKED_LIST_H

typedef struct SingleLinkedList_s {
    void *data;
    struct SingleLinkedList_s *next;
} SingleLinkedList;

SingleLinkedList *SingleLinkedList_new(void *data);
void SingleLinkedList_delete(SingleLinkedList * head,
			     void (*linklist_deallocator) (void *));
SingleLinkedList *SingleLinkedList_prepend(SingleLinkedList * l,
					   void *data);

void *SingleLinkedList_get_data(SingleLinkedList * l);

SingleLinkedList *SingleLinkedList_get_next(SingleLinkedList * l);

#endif
