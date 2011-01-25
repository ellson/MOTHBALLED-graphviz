/* $Id$Revision: */
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

