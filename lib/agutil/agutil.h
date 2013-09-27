/* $Id$ $Revision$ */
/* vim:set shiftwidth=4 ts=8: */

/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: See CVS logs. Details at http://www.graphviz.org/
 *************************************************************************/

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
