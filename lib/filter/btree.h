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

#ifndef BTREE_H
#define	BTREE_H
#include "tvnodes.h"

btree_node* new_node(char* attribute,char* regex, float min, float max);
int insert_node (btree_node* parent_n,btree_node* n);
int delete_node(btree_node* n);
int delete_node_recursive(btree_node* n);	//internal function
btree_node* look_up_node_with_string(btree_node* n,char* string_to_lookup);
int validate_lookup(btree_node* n,char* string_to_lookup);	//it can be anything, in this case attr_name
int print_tree(btree_node* root);
int print_children(btree_node* n);
btree_node* tree_from_filter_string(char* filter_string);
int evaluate_filter_atom(char* string,btree_node* Nodes[],char* op);
int evaluate_expresions(tv_node* Node,btree_node* root);
#endif
