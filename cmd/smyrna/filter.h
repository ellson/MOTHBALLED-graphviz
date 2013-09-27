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

#ifndef FILTER_H
#define FILTER_H
#include "btree.h"

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct _tv_filters {
	tv_filter **filters;
	int filter_count;
    } tv_filters;

    int clear_filter(tv_filter * f);
    int init_filters(tv_filters * filters);
    int add_filter_to_filters(tv_filters * filters, tv_filter * filter);
    int clear_filters(tv_filters * filters);
    int union_filter(tv_filter * f1, tv_filter * f2);
    int intersect_filter(tv_filter * f1, tv_filter * f2);

#ifdef __cplusplus
}				/* end extern "C" */
#endif
#endif
