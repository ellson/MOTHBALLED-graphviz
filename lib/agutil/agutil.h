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

/* attribute handling */

    double ag_scan_float(void *obj, char *name, double low, double high,
			 double defval);
    int ag_scan_int(void *obj, char *name, int low, int high, int defval);


/* a node queue */
    typedef struct Nqueue_s {
	Agnode_t **store, **limit, **head, **tail;
    } Nqueue;

    Nqueue *Nqueue_new(Agraph_t * g);
    void Nqueue_free(Agraph_t * g, Nqueue * q);
    void Nqueue_insert(Nqueue * q, Agnode_t * n);
    Agnode_t *Nqueue_remove(Nqueue * q);

#ifdef __cplusplus
}
#endif
