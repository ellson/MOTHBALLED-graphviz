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

#include "red_black_tree.h"
#include <stdlib.h>
#include <stdio.h>

/*  CONVENTIONS:  All data structures for red-black trees have the prefix */
/*                "rb_" to prevent name conflicts. */
/*                                                                      */
/*                Function names: Each word in a function name begins with */
/*                a capital letter.  An example funcntion name is  */
/*                CreateRedTree(a,b,c). Furthermore, each function name */
/*                should begin with a capital letter to easily distinguish */
/*                them from variables. */
/*                                                                     */
/*                Variable names: Each word in a variable name begins with */
/*                a capital letter EXCEPT the first letter of the variable */
/*                name.  For example, int newLongInt.  Global variables have */
/*                names beginning with "g".  An example of a global */
/*                variable name is gNewtonsConstant. */

void Assert(int assertion, char *error);
void *SafeMalloc(size_t size);

/*--------------------- stack.h --------*/

/*  CONVENTIONS:  All data structures for stacks have the prefix */
/*                "stk_" to prevent name conflicts. */
/*                                                                      */
/*                Function names: Each word in a function name begins with */
/*                a capital letter.  An example funcntion name is  */
/*                CreateRedTree(a,b,c). Furthermore, each function name */
/*                should begin with a capital letter to easily distinguish */
/*                them from variables. */
/*                                                                     */
/*                Variable names: Each word in a variable name begins with */
/*                a capital letter EXCEPT the first letter of the variable */
/*                name.  For example, int newLongInt.  Global variables have */
/*                names beginning with "g".  An example of a global */
/*                variable name is gNewtonsConstant. */

/*  if DATA_TYPE is undefined then stack.h and stack.c will be code for */
/*  stacks of void *, if they are defined then they will be stacks of the */
/*  appropriate data_type */

#ifndef DATA_TYPE
#define DATA_TYPE void *
#endif

typedef struct stk_stack_node {
    DATA_TYPE info;
    struct stk_stack_node *next;
} stk_stack_node;

typedef struct stk_stack {
    stk_stack_node *top;
    stk_stack_node *tail;
} stk_stack;

/*  These functions are all very straightforward and self-commenting so */
/*  I didn't think additional comments would be useful */
stk_stack *StackJoin(stk_stack * stack1, stk_stack * stack2);
stk_stack *StackCreate();
void StackPush(stk_stack * theStack, DATA_TYPE newInfoPointer);
void *StackPop(stk_stack * theStack);
int StackNotEmpty(stk_stack *);

/* comment out the line below to remove all the debugging assertion */
/* checks from the compiled code.  */
#define DEBUG_ASSERT 1

/***********************************************************************/
/*  FUNCTION:  RBTreeCreate */
 /**/
/*  INPUTS:  All the inputs are names of functions.  CompFunc takes to */
/*  void pointers to keys and returns 1 if the first arguement is */
/*  "greater than" the second.   DestFunc takes a pointer to a key and */
/*  destroys it in the appropriate manner when the node containing that */
/*  key is deleted.  InfoDestFunc is similiar to DestFunc except it */
/*  recieves a pointer to the info of a node and destroys it. */
/*  PrintFunc recieves a pointer to the key of a node and prints it. */
/*  PrintInfo recieves a pointer to the info of a node and prints it. */
/*  If RBTreePrint is never called the print functions don't have to be */
/*  defined and NullFunction can be used.  */
     /**/
/*  OUTPUT:  This function returns a pointer to the newly created */
/*  red-black tree. */
     /**/
/*  Modifies Input: none */
/***********************************************************************/
rb_red_blk_tree *
RBTreeCreate(int (*CompFunc) (const void *, const void *),
	     void (*DestFunc) (void *), void (*InfoDestFunc) (void *),
	     void (*PrintFunc) (const void *), void (*PrintInfo) (void *))
{
    rb_red_blk_tree *newTree;
    rb_red_blk_node *temp;

    newTree = (rb_red_blk_tree *) SafeMalloc(sizeof(rb_red_blk_tree));
    newTree->Compare = CompFunc;
    newTree->DestroyKey = DestFunc;
    newTree->PrintKey = PrintFunc;
    newTree->PrintInfo = PrintInfo;
    newTree->DestroyInfo = InfoDestFunc;

    /*  see the comment in the rb_red_blk_tree structure in red_black_tree.h */
    /*  for information on nil and root */
    temp = newTree->nil =
	(rb_red_blk_node *) SafeMalloc(sizeof(rb_red_blk_node));
    temp->parent = temp->left = temp->right = temp;
    temp->red = 0;
    temp->key = 0;
    temp = newTree->root =
	(rb_red_blk_node *) SafeMalloc(sizeof(rb_red_blk_node));
    temp->parent = temp->left = temp->right = newTree->nil;
    temp->key = 0;
    temp->red = 0;
    return (newTree);
}

/***********************************************************************/
/*  FUNCTION:  LeftRotate */
 /**/
/*  INPUTS:  This takes a tree so that it can access the appropriate */
/*           root and nil pointers, and the node to rotate on. */
     /**/
/*  OUTPUT:  None */
     /**/
/*  Modifies Input: tree, x */
     /**/
/*  EFFECTS:  Rotates as described in _Introduction_To_Algorithms by */
/*            Cormen, Leiserson, Rivest (Chapter 14).  Basically this */
/*            makes the parent of x be to the left of x, x the parent of */
/*            its parent before the rotation and fixes other pointers */
/*            accordingly. */
/***********************************************************************/
void LeftRotate(rb_red_blk_tree * tree, rb_red_blk_node * x)
{
    rb_red_blk_node *y;
    rb_red_blk_node *nil = tree->nil;

    /*  I originally wrote this function to use the sentinel for */
    /*  nil to avoid checking for nil.  However this introduces a */
    /*  very subtle bug because sometimes this function modifies */
    /*  the parent pointer of nil.  This can be a problem if a */
    /*  function which calls LeftRotate also uses the nil sentinel */
    /*  and expects the nil sentinel's parent pointer to be unchanged */
    /*  after calling this function.  For example, when RBDeleteFixUP */
    /*  calls LeftRotate it expects the parent pointer of nil to be */
    /*  unchanged. */

    y = x->right;
    x->right = y->left;

    if (y->left != nil)
	y->left->parent = x;	/* used to use sentinel here */
    /* and do an unconditional assignment instead of testing for nil */

    y->parent = x->parent;

    /* instead of checking if x->parent is the root as in the book, we */
    /* count on the root sentinel to implicitly take care of this case */
    if (x == x->parent->left) {
	x->parent->left = y;
    } else {
	x->parent->right = y;
    }
    y->left = x;
    x->parent = y;

#ifdef DEBUG_ASSERT
    Assert(!tree->nil->red, "nil not red in LeftRotate");
#endif
}


/***********************************************************************/
/*  FUNCTION:  RighttRotate */
 /**/
/*  INPUTS:  This takes a tree so that it can access the appropriate */
/*           root and nil pointers, and the node to rotate on. */
     /**/
/*  OUTPUT:  None */
     /**/
/*  Modifies Input?: tree, y */
     /**/
/*  EFFECTS:  Rotates as described in _Introduction_To_Algorithms by */
/*            Cormen, Leiserson, Rivest (Chapter 14).  Basically this */
/*            makes the parent of x be to the left of x, x the parent of */
/*            its parent before the rotation and fixes other pointers */
/*            accordingly. */
/***********************************************************************/
void RightRotate(rb_red_blk_tree * tree, rb_red_blk_node * y)
{
    rb_red_blk_node *x;
    rb_red_blk_node *nil = tree->nil;

    /*  I originally wrote this function to use the sentinel for */
    /*  nil to avoid checking for nil.  However this introduces a */
    /*  very subtle bug because sometimes this function modifies */
    /*  the parent pointer of nil.  This can be a problem if a */
    /*  function which calls LeftRotate also uses the nil sentinel */
    /*  and expects the nil sentinel's parent pointer to be unchanged */
    /*  after calling this function.  For example, when RBDeleteFixUP */
    /*  calls LeftRotate it expects the parent pointer of nil to be */
    /*  unchanged. */

    x = y->left;
    y->left = x->right;

    if (nil != x->right)
	x->right->parent = y;	/*used to use sentinel here */
    /* and do an unconditional assignment instead of testing for nil */

    /* instead of checking if x->parent is the root as in the book, we */
    /* count on the root sentinel to implicitly take care of this case */
    x->parent = y->parent;
    if (y == y->parent->left) {
	y->parent->left = x;
    } else {
	y->parent->right = x;
    }
    x->right = y;
    y->parent = x;

#ifdef DEBUG_ASSERT
    Assert(!tree->nil->red, "nil not red in RightRotate");
#endif
}

/***********************************************************************/
/*  FUNCTION:  TreeInsertHelp  */
 /**/
/*  INPUTS:  tree is the tree to insert into and z is the node to insert */
     /**/
/*  OUTPUT:  none */
     /**/
/*  Modifies Input:  tree, z */
     /**/
/*  EFFECTS:  Inserts z into the tree as if it were a regular binary tree */
/*            using the algorithm described in _Introduction_To_Algorithms_ */
/*            by Cormen et al.  This funciton is only intended to be called */
/*            by the RBTreeInsert function and not by the user */
/***********************************************************************/
void TreeInsertHelp(rb_red_blk_tree * tree, rb_red_blk_node * z)
{
    /*  This function should only be called by InsertRBTree (see above) */
    rb_red_blk_node *x;
    rb_red_blk_node *y;
    rb_red_blk_node *nil = tree->nil;

    z->left = z->right = nil;
    y = tree->root;
    x = tree->root->left;
    while (x != nil) {
	y = x;
	if (1 == tree->Compare(x->key, z->key)) {	/* x.key > z.key */
	    x = x->left;
	} else {		/* x,key <= z.key */
	    x = x->right;
	}
    }
    z->parent = y;
    if ((y == tree->root) || (1 == tree->Compare(y->key, z->key))) {	/* y.key > z.key */
	y->left = z;
    } else {
	y->right = z;
    }

#ifdef DEBUG_ASSERT
    Assert(!tree->nil->red, "nil not red in TreeInsertHelp");
#endif
}

/*  Before calling Insert RBTree the node x should have its key set */

/***********************************************************************/
/*  FUNCTION:  RBTreeInsert */
 /**/
/*  INPUTS:  tree is the red-black tree to insert a node which has a key */
/*           pointed to by key and info pointed to by info.  */
     /**/
/*  OUTPUT:  This function returns a pointer to the newly inserted node */
/*           which is guarunteed to be valid until this node is deleted. */
/*           What this means is if another data structure stores this */
/*           pointer then the tree does not need to be searched when this */
/*           is to be deleted. */
     /**/
/*  Modifies Input: tree */
     /**/
/*  EFFECTS:  Creates a node node which contains the appropriate key and */
/*            info pointers and inserts it into the tree. */
/***********************************************************************/
    rb_red_blk_node * RBTreeInsert(rb_red_blk_tree * tree, void *key,
				   void *info)
{
    rb_red_blk_node *y;
    rb_red_blk_node *x;
    rb_red_blk_node *newNode;

    x = (rb_red_blk_node *) SafeMalloc(sizeof(rb_red_blk_node));
    x->key = key;
    x->info = info;

    TreeInsertHelp(tree, x);
    newNode = x;
    x->red = 1;
    while (x->parent->red) {	/* use sentinel instead of checking for root */
	if (x->parent == x->parent->parent->left) {
	    y = x->parent->parent->right;
	    if (y->red) {
		x->parent->red = 0;
		y->red = 0;
		x->parent->parent->red = 1;
		x = x->parent->parent;
	    } else {
		if (x == x->parent->right) {
		    x = x->parent;
		    LeftRotate(tree, x);
		}
		x->parent->red = 0;
		x->parent->parent->red = 1;
		RightRotate(tree, x->parent->parent);
	    }
	} else {		/* case for x->parent == x->parent->parent->right */
	    y = x->parent->parent->left;
	    if (y->red) {
		x->parent->red = 0;
		y->red = 0;
		x->parent->parent->red = 1;
		x = x->parent->parent;
	    } else {
		if (x == x->parent->left) {
		    x = x->parent;
		    RightRotate(tree, x);
		}
		x->parent->red = 0;
		x->parent->parent->red = 1;
		LeftRotate(tree, x->parent->parent);
	    }
	}
    }
    tree->root->left->red = 0;
    return (newNode);

#ifdef DEBUG_ASSERT
    Assert(!tree->nil->red, "nil not red in RBTreeInsert");
    Assert(!tree->root->red, "root not red in RBTreeInsert");
#endif
}

/***********************************************************************/
/*  FUNCTION:  TreeSuccessor  */
 /**/
/*    INPUTS:  tree is the tree in question, and x is the node we want the */
/*             the successor of. */
     /**/
/*    OUTPUT:  This function returns the successor of x or NULL if no */
/*             successor exists. */
     /**/
/*    Modifies Input: none */
     /**/
/*    Note:  uses the algorithm in _Introduction_To_Algorithms_ */
/***********************************************************************/
    rb_red_blk_node * TreeSuccessor(rb_red_blk_tree * tree,
				    rb_red_blk_node * x)
{
    rb_red_blk_node *y;
    rb_red_blk_node *nil = tree->nil;
    rb_red_blk_node *root = tree->root;

    if (nil != (y = x->right)) {	/* assignment to y is intentional */
	while (y->left != nil) {	/* returns the minium of the right subtree of x */
	    y = y->left;
	}
	return (y);
    } else {
	y = x->parent;
	while (x == y->right) {	/* sentinel used instead of checking for nil */
	    x = y;
	    y = y->parent;
	}
	if (y == root)
	    return (nil);
	return (y);
    }
}

/***********************************************************************/
/*  FUNCTION:  Treepredecessor  */
 /**/
/*    INPUTS:  tree is the tree in question, and x is the node we want the */
/*             the predecessor of. */
     /**/
/*    OUTPUT:  This function returns the predecessor of x or NULL if no */
/*             predecessor exists. */
     /**/
/*    Modifies Input: none */
     /**/
/*    Note:  uses the algorithm in _Introduction_To_Algorithms_ */
/***********************************************************************/
    rb_red_blk_node * TreePredecessor(rb_red_blk_tree * tree,
				      rb_red_blk_node * x)
{
    rb_red_blk_node *y;
    rb_red_blk_node *nil = tree->nil;
    rb_red_blk_node *root = tree->root;

    if (nil != (y = x->left)) {	/* assignment to y is intentional */
	while (y->right != nil) {	/* returns the maximum of the left subtree of x */
	    y = y->right;
	}
	return (y);
    } else {
	y = x->parent;
	while (x == y->left) {
	    if (y == root)
		return (nil);
	    x = y;
	    y = y->parent;
	}
	return (y);
    }
}

/***********************************************************************/
/*  FUNCTION:  InorderTreePrint */
 /**/
/*    INPUTS:  tree is the tree to print and x is the current inorder node */
     /**/
/*    OUTPUT:  none  */
     /**/
/*    EFFECTS:  This function recursively prints the nodes of the tree */
/*              inorder using the PrintKey and PrintInfo functions. */
     /**/
/*    Modifies Input: none */
     /**/
/*    Note:    This function should only be called from RBTreePrint */
/***********************************************************************/
void InorderTreePrint(rb_red_blk_tree * tree, rb_red_blk_node * x)
{
    rb_red_blk_node *nil = tree->nil;
    rb_red_blk_node *root = tree->root;
    if (x != tree->nil) {
	InorderTreePrint(tree, x->left);
	printf("info=");
	tree->PrintInfo(x->info);
	printf("  key=");
	tree->PrintKey(x->key);
	printf("  l->key=");
	if (x->left == nil)
	    printf("NULL");
	else
	    tree->PrintKey(x->left->key);
	printf("  r->key=");
	if (x->right == nil)
	    printf("NULL");
	else
	    tree->PrintKey(x->right->key);
	printf("  p->key=");
	if (x->parent == root)
	    printf("NULL");
	else
	    tree->PrintKey(x->parent->key);
	printf("  red=%i\n", x->red);
	InorderTreePrint(tree, x->right);
    }
}


/***********************************************************************/
/*  FUNCTION:  TreeDestHelper */
 /**/
/*    INPUTS:  tree is the tree to destroy and x is the current node */
     /**/
/*    OUTPUT:  none  */
     /**/
/*    EFFECTS:  This function recursively destroys the nodes of the tree */
/*              postorder using the DestroyKey and DestroyInfo functions. */
     /**/
/*    Modifies Input: tree, x */
     /**/
/*    Note:    This function should only be called by RBTreeDestroy */
/***********************************************************************/
void TreeDestHelper(rb_red_blk_tree * tree, rb_red_blk_node * x)
{
    rb_red_blk_node *nil = tree->nil;
    if (x != nil) {
	TreeDestHelper(tree, x->left);
	TreeDestHelper(tree, x->right);
	tree->DestroyKey(x->key);
	tree->DestroyInfo(x->info);
	free(x);
    }
}


/***********************************************************************/
/*  FUNCTION:  RBTreeDestroy */
 /**/
/*    INPUTS:  tree is the tree to destroy */
     /**/
/*    OUTPUT:  none */
     /**/
/*    EFFECT:  Destroys the key and frees memory */
     /**/
/*    Modifies Input: tree */
     /**/
/***********************************************************************/
void RBTreeDestroy(rb_red_blk_tree * tree)
{
    TreeDestHelper(tree, tree->root->left);
    free(tree->root);
    free(tree->nil);
    free(tree);
}


/***********************************************************************/
/*  FUNCTION:  RBTreePrint */
 /**/
/*    INPUTS:  tree is the tree to print */
     /**/
/*    OUTPUT:  none */
     /**/
/*    EFFECT:  This function recursively prints the nodes of the tree */
/*             inorder using the PrintKey and PrintInfo functions. */
     /**/
/*    Modifies Input: none */
     /**/
/***********************************************************************/
void RBTreePrint(rb_red_blk_tree * tree)
{
    InorderTreePrint(tree, tree->root->left);
}


/***********************************************************************/
/*  FUNCTION:  RBExactQuery */
 /**/
/*    INPUTS:  tree is the tree to print and q is a pointer to the key */
/*             we are searching for */
     /**/
/*    OUTPUT:  returns the a node with key equal to q.  If there are */
/*             multiple nodes with key equal to q this function returns */
/*             the one highest in the tree */
     /**/
/*    Modifies Input: none */
     /**/
/***********************************************************************/
    rb_red_blk_node * RBExactQuery(rb_red_blk_tree * tree, void *q)
{
    rb_red_blk_node *x = tree->root->left;
    rb_red_blk_node *nil = tree->nil;
    int compVal;
    if (x == nil)
	return (0);
    compVal = tree->Compare(x->key, (int *) q);
    while (0 != compVal) {	/*assignemnt */
	if (1 == compVal) {	/* x->key > q */
	    x = x->left;
	} else {
	    x = x->right;
	}
	if (x == nil)
	    return (0);
	compVal = tree->Compare(x->key, (int *) q);
    }
    return (x);
}


/***********************************************************************/
/*  FUNCTION:  RBDeleteFixUp */
 /**/
/*    INPUTS:  tree is the tree to fix and x is the child of the spliced */
/*             out node in RBTreeDelete. */
     /**/
/*    OUTPUT:  none */
     /**/
/*    EFFECT:  Performs rotations and changes colors to restore red-black */
/*             properties after a node is deleted */
     /**/
/*    Modifies Input: tree, x */
     /**/
/*    The algorithm from this function is from _Introduction_To_Algorithms_ */
/***********************************************************************/
void RBDeleteFixUp(rb_red_blk_tree * tree, rb_red_blk_node * x)
{
    rb_red_blk_node *root = tree->root->left;
    rb_red_blk_node *w;

    while ((!x->red) && (root != x)) {
	if (x == x->parent->left) {
	    w = x->parent->right;
	    if (w->red) {
		w->red = 0;
		x->parent->red = 1;
		LeftRotate(tree, x->parent);
		w = x->parent->right;
	    }
	    if ((!w->right->red) && (!w->left->red)) {
		w->red = 1;
		x = x->parent;
	    } else {
		if (!w->right->red) {
		    w->left->red = 0;
		    w->red = 1;
		    RightRotate(tree, w);
		    w = x->parent->right;
		}
		w->red = x->parent->red;
		x->parent->red = 0;
		w->right->red = 0;
		LeftRotate(tree, x->parent);
		x = root;	/* this is to exit while loop */
	    }
	} else {		/* the code below is has left and right switched from above */
	    w = x->parent->left;
	    if (w->red) {
		w->red = 0;
		x->parent->red = 1;
		RightRotate(tree, x->parent);
		w = x->parent->left;
	    }
	    if ((!w->right->red) && (!w->left->red)) {
		w->red = 1;
		x = x->parent;
	    } else {
		if (!w->left->red) {
		    w->right->red = 0;
		    w->red = 1;
		    LeftRotate(tree, w);
		    w = x->parent->left;
		}
		w->red = x->parent->red;
		x->parent->red = 0;
		w->left->red = 0;
		RightRotate(tree, x->parent);
		x = root;	/* this is to exit while loop */
	    }
	}
    }
    x->red = 0;

#ifdef DEBUG_ASSERT
    Assert(!tree->nil->red, "nil not black in RBDeleteFixUp");
#endif
}


/***********************************************************************/
/*  FUNCTION:  RBDelete */
 /**/
/*    INPUTS:  tree is the tree to delete node z from */
     /**/
/*    OUTPUT:  none */
     /**/
/*    EFFECT:  Deletes z from tree and frees the key and info of z */
/*             using DestoryKey and DestoryInfo.  Then calls */
/*             RBDeleteFixUp to restore red-black properties */
     /**/
/*    Modifies Input: tree, z */
     /**/
/*    The algorithm from this function is from _Introduction_To_Algorithms_ */
/***********************************************************************/
void RBDelete(rb_red_blk_tree * tree, rb_red_blk_node * z)
{
    rb_red_blk_node *y;
    rb_red_blk_node *x;
    rb_red_blk_node *nil = tree->nil;
    rb_red_blk_node *root = tree->root;

    y = ((z->left == nil)
	 || (z->right == nil)) ? z : TreeSuccessor(tree, z);
    x = (y->left == nil) ? y->right : y->left;
    if (root == (x->parent = y->parent)) {	/* assignment of y->p to x->p is intentional */
	root->left = x;
    } else {
	if (y == y->parent->left) {
	    y->parent->left = x;
	} else {
	    y->parent->right = x;
	}
    }
    if (y != z) {		/* y should not be nil in this case */

#ifdef DEBUG_ASSERT
	Assert((y != tree->nil), "y is nil in RBDelete\n");
#endif
	/* y is the node to splice out and x is its child */

	if (!(y->red))
	    RBDeleteFixUp(tree, x);

	tree->DestroyKey(z->key);
	tree->DestroyInfo(z->info);
	y->left = z->left;
	y->right = z->right;
	y->parent = z->parent;
	y->red = z->red;
	z->left->parent = z->right->parent = y;
	if (z == z->parent->left) {
	    z->parent->left = y;
	} else {
	    z->parent->right = y;
	}
	free(z);
    } else {
	tree->DestroyKey(y->key);
	tree->DestroyInfo(y->info);
	if (!(y->red))
	    RBDeleteFixUp(tree, x);
	free(y);
    }

#ifdef DEBUG_ASSERT
    Assert(!tree->nil->red, "nil not black in RBDelete");
#endif
}


/***********************************************************************/
/*  FUNCTION:  RBDEnumerate */
 /**/
/*    INPUTS:  tree is the tree to look for keys >= low */
/*             and <= high with respect to the Compare function */
     /**/
/*    OUTPUT:  stack containing pointers to the nodes between [low,high] */
     /**/
/*    Modifies Input: none */
/***********************************************************************/
    stk_stack * RBEnumerate(rb_red_blk_tree * tree, void *low, void *high)
{
    stk_stack *enumResultStack;
    rb_red_blk_node *nil = tree->nil;
    rb_red_blk_node *x = tree->root->left;
    rb_red_blk_node *lastBest = nil;

    enumResultStack = StackCreate();
    while (nil != x) {
	if (1 == (tree->Compare(x->key, high))) {	/* x->key > high */
	    x = x->left;
	} else {
	    lastBest = x;
	    x = x->right;
	}
    }
    while ((lastBest != nil) && (1 != tree->Compare(low, lastBest->key))) {
	StackPush(enumResultStack, lastBest);
	lastBest = TreePredecessor(tree, lastBest);
    }
    return (enumResultStack);
}








/***********************************************************************/
/*  FUNCTION:  void Assert(int assertion, char* error)  */
 /**/
/*  INPUTS: assertion should be a predicated that the programmer */
/*  assumes to be true.  If this assumption is not true the message */
/*  error is printed and the program exits. */
     /**/
/*  OUTPUT: None. */
     /**/
/*  Modifies input:  none */
     /**/
/*  Note:  If DEBUG_ASSERT is not defined then assertions should not */
/*         be in use as they will slow down the code.  Therefore the */
/*         compiler will complain if an assertion is used when */
/*         DEBUG_ASSERT is undefined. */
/***********************************************************************/
void Assert(int assertion, char *error)
{
    if (!assertion) {
	printf("Assertion Failed: %s\n", error);
	exit(-1);
    }
}


/*------------------------ misc.c -------------*/

/***********************************************************************/
/*  FUNCTION:  SafeMalloc */
 /**/
/*    INPUTS:  size is the size to malloc */
     /**/
/*    OUTPUT:  returns pointer to allocated memory if succesful */
     /**/
/*    EFFECT:  mallocs new memory.  If malloc fails, prints error message */
/*             and terminates program. */
     /**/
/*    Modifies Input: none */
     /**/
/***********************************************************************/
void *SafeMalloc(size_t size)
{
    void *result;

    if ((result = malloc(size))) {	/* assignment intentional */
	return (result);
    } else {
	printf("memory overflow: malloc failed in SafeMalloc.");
	printf("  Exiting Program.\n");
	exit(-1);
	return (0);
    }
}

/*  NullFunction does nothing it is included so that it can be passed */
/*  as a function to RBTreeCreate when no other suitable function has */
/*  been defined */


void NullFunction(void *junk)
{;
}



/*----------------- stack.c ------------ */

int StackNotEmpty(stk_stack * theStack)
{
    return (theStack ? (theStack->top != NULL) : 0);
}

stk_stack *StackJoin(stk_stack * stack1, stk_stack * stack2)
{
    if (!stack1->tail) {
	free(stack1);
	return (stack2);
    } else {
	stack1->tail->next = stack2->top;
	stack1->tail = stack2->tail;
	free(stack2);
	return (stack1);
    }
}

stk_stack *StackCreate()
{
    stk_stack *newStack;

    newStack = (stk_stack *) SafeMalloc(sizeof(stk_stack));
    newStack->top = newStack->tail = NULL;
    return (newStack);
}


void StackPush(stk_stack * theStack, DATA_TYPE newInfoPointer)
{
    stk_stack_node *newNode;

    if (!theStack->top) {
	newNode = (stk_stack_node *) SafeMalloc(sizeof(stk_stack_node));
	newNode->info = newInfoPointer;
	newNode->next = theStack->top;
	theStack->top = newNode;
	theStack->tail = newNode;
    } else {
	newNode = (stk_stack_node *) SafeMalloc(sizeof(stk_stack_node));
	newNode->info = newInfoPointer;
	newNode->next = theStack->top;
	theStack->top = newNode;
    }

}

DATA_TYPE StackPop(stk_stack * theStack)
{
    DATA_TYPE popInfo;
    stk_stack_node *oldNode;

    if (theStack->top) {
	popInfo = theStack->top->info;
	oldNode = theStack->top;
	theStack->top = theStack->top->next;
	free(oldNode);
	if (!theStack->top)
	    theStack->tail = NULL;
    } else {
	popInfo = NULL;
    }
    return (popInfo);
}

void StackDestroy(stk_stack * theStack, void DestFunc(void *a))
{
    stk_stack_node *x = theStack->top;
    stk_stack_node *y;

    if (theStack) {
	while (x) {
	    y = x->next;
	    DestFunc(x->info);
	    free(x);
	    x = y;
	}
	free(theStack);
    }
}
