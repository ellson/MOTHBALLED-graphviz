/* $Id$ $Revision$ */
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

#ifdef __cplusplus
extern "C" {
#endif


#ifndef GENERIC_LIST_H
#define GENERIC_LIST_H

    typedef void *gl_data;

    typedef struct generic_list_s {
	unsigned long used;	/* number of elements in the list */
	unsigned long size;	/* number of elements that the list can hold */
	gl_data *data;		/* pointer to first element */
    } generic_list_t;

    extern generic_list_t *new_generic_list(unsigned long size);
    extern generic_list_t *add_to_generic_list(generic_list_t * list,
					       gl_data element);
    extern void free_generic_list(generic_list_t * list);

#endif				/* GENERIC_LIST_H */

#ifdef __cplusplus
}
#endif
