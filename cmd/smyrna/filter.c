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

#include "filter.h"
#include "memory.h"

int clear_filter(tv_filter * f)
{
    delete_node(f->root);
    return 1;
}

int init_filters(tv_filters * filters)
{
    filters->filter_count = 0;
    filters->filters = 0;
    return 1;

}

int add_filter_to_filters(tv_filters * filters, tv_filter * filter)
{
    filters->filters =
	RALLOC(filters->filter_count + 1, filters->filters, tv_filter *);
    filters->filters[filters->filter_count] = filter;
    filters->filter_count++;
    return 1;

}

int clear_filters(tv_filters * filters)
{
    //deep clear
    int ind = 0;
    for (ind = 0; ind < filters->filter_count; ind++) {
	clear_filter(filters->filters[ind]);
	free(filters->filters[ind]);
    }
    filters->filter_count = 0;
    return 1;

}

int union_filter(tv_filter * f1, tv_filter * f2)
{
    return 1;

}

int intersect_filter(tv_filter * f1, tv_filter * f2)
{
    return 1;

}
