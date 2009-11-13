/* $Id$Revision: */
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

#include "topviewdata.h"
#include "btree.h"
#include "glcomppanel.h"
#include "glcompbutton.h"

#include <memory.h>

static int validate_group_node(tv_node * TV_Node, char *regex_string)
{
    btree_node *n = 0;
//              n=tree_from_filter_string("([IP=\"^10.*\",min=\"0\",max=\"0\"])");
    int valid = 0;
    n = tree_from_filter_string(regex_string);
    valid = evaluate_expresions(TV_Node, n);
//      delete_node(n); 
    return valid;
}

