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

#ifndef BTREE_H
#define	BTREE_H

#include "tvnodes.h"
#ifdef _WIN32
#include "regex_win32.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

    btree_node *new_node(char *attribute, char *regex, float min,
			 float max);
    int insert_node(btree_node * parent_n, btree_node * n);
    int delete_node(btree_node * n);
    int delete_node_recursive(btree_node * n);	//internal function
    btree_node *look_up_node_with_string(btree_node * n,
					 char *string_to_lookup);
    int validate_lookup(btree_node * n, char *string_to_lookup);	//it can be anything, in this case attr_name
    int print_tree(btree_node * root);
    int print_children(btree_node * n);
    btree_node *tree_from_filter_string(char *filter_string);
    int evaluate_filter_atom(char *string, btree_node * Nodes[], char *op);
    int evaluate_expresions(tv_node * Node, btree_node * root);

#ifdef __cplusplus
}				/* end extern "C" */
#endif
#endif
