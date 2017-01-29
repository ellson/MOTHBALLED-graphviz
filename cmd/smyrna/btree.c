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

#include "btree.h"
#ifdef _WIN32
#include "regex_win32.h"
#else
#include "regex.h"
#endif

#include <memory.h>

btree_node *new_node(char *attribute, char *regex, float min, float max)
{
    btree_node *n;
    n = GNEW(btree_node);
    n->child_count = 0;
    n->childs = 0;
    n->rank = 0;
    n->attr_name = strdup(attribute);
    n->regex = strdup(regex);
    n->min = 0;
    n->max = 0;
    n->value = -1;
    n->active = 1;
    return n;
}

int insert_node(btree_node * parent_n, btree_node * n)
{
    parent_n->child_count++;
    parent_n->childs = RALLOC(parent_n->child_count, parent_n->childs,
			      btree_node *);
    parent_n->childs[parent_n->child_count - 1] = n;
    n->rank = parent_n->rank + 1;
    n->parent = parent_n;
    return 1;
}

int delete_node_recursive(btree_node * n)
{
    int i = 0;
    //delete recursively
    for (i = 0; i < n->child_count; i++) {
	delete_node(n->childs[i]);
    }
    free(n->attr_name);
    free(n->regex);
    free(n->childs);

    free(n);
    return 1;
}

int delete_node(btree_node * n)
{
    btree_node *parent = n->parent;
    int i = 0;
    int child_found = 0;
    //rmeove from parent's child list
    if (parent) {
	parent->child_count--;
	for (i = 0; i < parent->child_count; i++) {
	    if (parent->childs[i] == n) {
		child_found = 1;
	    }
	    if (child_found)
		parent->childs[i] = parent->childs[i + 1];
	}
    }
    parent->childs =
	RALLOC(parent->child_count, parent->childs, btree_node *);
    delete_node_recursive(n);
    return 1;
}

btree_node *look_up_node_with_string(btree_node * n,
				     char *string_to_lookup)
{
    int i = 0;
    btree_node *nn;
    if (validate_lookup(n, string_to_lookup))
	return n;
    else {
	for (i = 0; i < n->child_count; i++) {
	    nn = look_up_node_with_string(n->childs[i], string_to_lookup);
	    if (nn)
		return nn;
	}
    }
    return 0;

}
int validate_lookup(btree_node * n, char *string_to_lookup)	//it can be anything, in this case attr_name
{
    if (strcmp(n->attr_name, string_to_lookup) == 0)
	return 1;
    return 0;
}

int print_tree(btree_node * root)
{
    return 1;
}

int print_children(btree_node * n)
{
    int i = 0;
    if (n->node_type == 2)
	printf("      %*s=%s(%i)\n", n->rank * 5 + 1, n->attr_name,
	       n->regex, n->rank);
    else
	printf("%*s %c(%i)\n", n->rank * 5 + 1, "", n->op, n->rank);
    for (i = 0; i < n->child_count; i++) 
    {
	print_children(n->childs[i]);
    }
    return 1;

}

int sample_tree(void)
{
    btree_node *root;
    btree_node *n;
    btree_node *nn;
    btree_node *kurbanlik;
    root = new_node("A", "R", 0, 0);
    root->parent = 0;
    insert_node(root, new_node("B", "R", 0, 0));
    insert_node(root, new_node("C", "R", 0, 0));

    n = new_node("D", "R", 0, 0);
    kurbanlik = n;
    insert_node(root, n);

    nn = new_node("E", "R", 0, 0);
    insert_node(n, nn);
    insert_node(n, new_node("X", "R", 0, 0));
    insert_node(n, new_node("Y", "R", 0, 0));
    insert_node(n, new_node("Z", "R", 0, 0));
    n = new_node("F", "R", 0, 0);
    insert_node(root, n);

    print_children(root);
    n = look_up_node_with_string(root, "F");
    if (n)
	printf("found   value-->%s \n", n->attr_name);

    delete_node(kurbanlik);
    print_children(root);
    print_children(tree_from_filter_string("(()(())((())()))"));
    return 1;


}

btree_node *tree_from_filter_string(char *filter_string)
{
    btree_node *root;
    btree_node *cursor;
    btree_node *n;
    btree_node *Nodes[2];
    char buffer[512];
    char *b_cursor = 0;
    char *c_cursor;
    char op = '\0';
    char last_char = '\0';
    root = new_node("R", "", 0, 0);
    cursor = root;
    c_cursor = filter_string;
    while (*c_cursor != '\0') {
	switch (*c_cursor) {
	case '(':
	    n = new_node(".", "", 0, 0);
	    insert_node(cursor, n);
	    last_char = '(';
	    cursor = n;
	    break;
	case ')':
	    last_char = ')';
	    cursor = cursor->parent;
	    break;
	default:
	    if (last_char == '(') {
		b_cursor = buffer;
		while (*c_cursor != ')') {
		    *b_cursor = *c_cursor;
		    b_cursor++;
		    c_cursor++;
		}
		*b_cursor = '\0';
		evaluate_filter_atom(buffer, Nodes, &op);
		if (Nodes[1] != 0) {
		    insert_node(cursor, Nodes[0]);
		    insert_node(cursor, Nodes[1]);
		    cursor->op = op;
		} else		//only one expression in ()
		{
		    insert_node(cursor->parent, Nodes[0]);
		    delete_node(cursor);
		    cursor = Nodes[0];
		}


		c_cursor--;
	    }
	    if (last_char == ')')
		cursor->op = *c_cursor;
	    break;
	};
	c_cursor++;
    }
    return root;
}
int evaluate_filter_atom(char *string, btree_node * Nodes[], char *op)
{
    char *c_cursor;
    char **attrs;
    char buff_attr[512];
    int c_buff_attr = 0;
    char buff_value[512];
    int c_buff_value = 0;
    int attrs_count = 0;
    char **values;
    int values_count = 0;
    btree_node *n;
    int kp_open = 0;		//[ open
    int qt_open = 0;		//" open?
    int i = 0;

    attrs = 0;
    values = 0;

    c_cursor = string;
    while (*c_cursor != '\0') {
	if (kp_open) {
	    if ((*c_cursor == ',') || ((*c_cursor == ']') && (!qt_open))) {
		attrs = RALLOC(attrs_count + 1, attrs, char *);
		attrs[attrs_count] = strdup(buff_attr);
		attrs_count++;
		values = RALLOC(values_count + 1, values, char *);
		values[values_count] = strdup(buff_value);
		values_count++;
		buff_attr[0] = '\0';
		buff_value[0] = '\0';
		c_buff_attr = 0;
		c_buff_value = 0;
	    }
	    if ((*c_cursor != '=') && (*c_cursor != ',')
		&& (*c_cursor != '\"')) {
		if (!qt_open && (*c_cursor != ']') && (*c_cursor != '[')) {
		    buff_attr[c_buff_attr] = *c_cursor;
		    buff_attr[c_buff_attr + 1] = '\0';
		    c_buff_attr++;
		}
		if (qt_open) {
		    buff_value[c_buff_value] = *c_cursor;
		    buff_value[c_buff_value + 1] = '\0';
		    c_buff_value++;
		}
	    }
	    if ((*c_cursor == '\"') && (qt_open))
		qt_open = 0;
	    else if ((*c_cursor == '\"') && (!qt_open))
		qt_open = 1;
	    if ((*c_cursor == ']') && !qt_open)
		kp_open = 0;
	}
	if (*c_cursor == '[')
	    kp_open = 1;
	if (*c_cursor == '&')
	    *op = '&';
	if (*c_cursor == '|')
	    *op = '|';
	c_cursor++;
    }

    n = new_node(attrs[0], values[0], (float) atof(values[1]),
		 (float) atof(values[2]));
    n->node_type = 2;
    Nodes[0] = n;

    if (attrs_count > 5) {
	n = new_node(attrs[3], values[3], (float) atof(values[4]),
		     (float) atof(values[5]));
	n->node_type = 2;
	Nodes[1] = n;
    } else
	Nodes[1] = 0;

    for (i = 0; i < attrs_count; i++) {
	free(attrs[i]);
	free(values[i]);
    }
    free(attrs);
    free(values);

    return 1;
}

int evaluate_expresions(tv_node * TV_Node, btree_node * n)
{
    regex_t preg;
    char *data;
    int i = 0;
    int ii = 0;
    if (!n)
	return 1;
    if (n->op == '&')
	ii = 1;
    for (i = 0; i < n->child_count; i++) {
	evaluate_expresions(TV_Node, n->childs[i]);
	if (n->op == '&')
	    ii = ii && n->childs[i]->value;
//                      if(n->op=='|')
	else
	    ii = ii || n->childs[i]->value;

    }
    if (n->node_type == 2) {
//assert(n);
//assert(n->attr_name);
//fprintf(stderr,"agget(%d,%s)", TV_Node->index , n->attr_name);
	data =
	    agget(view->Topview->Nodes[TV_Node->index].Node, n->attr_name);
//fprintf(stderr,"  = %s\n", data);
	if (data) {
	    regcomp(&preg, n->regex, REG_NOSUB);
	    if (regexec(&preg, data, 0, 0, 0) == 0)
		n->value = 1;
	    else
		n->value = 0;
	    regfree(&preg);
	} else
	    n->value = 1;	//no attribute return 1
    } else
	n->value = ii;
    return n->value;

}
