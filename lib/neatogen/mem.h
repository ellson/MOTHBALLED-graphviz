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



#ifndef MEMORY_H
#define MEMORY_H

#ifndef NULL
#define NULL 0
#endif

    /* Support for freelists */

    typedef struct freelist {
	struct freenode *head;	/* List of free nodes */
	struct freeblock *blocklist;	/* List of malloced blocks */
	int nodesize;		/* Size of node */
    } Freelist;

    extern void *getfree(Freelist *);
    extern void freeinit(Freelist *, int);
    extern void makefree(void *, Freelist *);

#endif


#ifdef __cplusplus
}
#endif
