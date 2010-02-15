#include	<search.h>
#include	"../dthdr.h"

/*	POSIX tsearch library based on cdt
**	Written by Kiem-Phong Vo (AT&T Labs, 07/19/95)
*/

typedef struct _tree_s
{	Dtlink_t	link;
	Void_t*		key;
} Tree_t;

typedef struct _treedisc_s
{	Dtdisc_t	disc;
	int(*		comparf)_ARG_((const Void_t*, const Void_t*));
} Treedisc_t;

/* compare function */
#if __STD_C
static int treecompare(Dt_t* dt, char* one, char* two, Dtdisc_t* disc)
#else
static int treecompare(dt, one, two, disc)
Dt_t*		dt;
char*		one;
char*		two;
Dtdisc_t*	disc;
#endif
{
	return (*((Treedisc_t*)disc)->comparf)((Void_t*)one,(Void_t*)two);
}

static Treedisc_t	Treedisc =
{	{ sizeof(Dtlink_t), -1,	/* object is key		*/
	  0,
	  NIL(Dtmake_f), NIL(Dtfree_f),
	  treecompare,
	  NIL(Dthash_f),
	  NIL(Dtmemory_f),
	  NIL(Dtevent_f)
	},
	0
};

#if __STD_C
Void_t* tsearch(const Void_t* key, Void_t** rootp,
		int(*comparf)(const Void_t*,const Void_t*) )
#else
Void_t* tsearch(key, rootp, comparf)
Void_t*		key;
Void_t**	rootp;
int(*		comparf)();
#endif
{
	reg Dt_t*	dt;
	reg Tree_t*	o;

	if(!rootp ||
	   (!(dt = *((Dt_t**)rootp)) && !(dt = dtopen((Dtdisc_t*)(&Treedisc),Dtoset))) )
		return NIL(Void_t*);

	/* dangerous to set comparf on each call but that's tsearch */
	Treedisc.comparf = comparf;

	if(!(o = (Tree_t*)dtmatch(dt,key)) )
	{	if(!(o = (Tree_t*)malloc(sizeof(Tree_t))) )
			return NIL(Void_t*);
		o->key = (Void_t*)key;
		dtinsert(dt,o);
	}

	if(o)
		*rootp = (Void_t*)dt;
	else if(*rootp == NIL(Void_t*) )
		dtclose(dt);

	return (Void_t*)(&o->key);
}

#if __STD_C
Void_t* tfind(const Void_t* key, Void_t*const* rootp,
		int(*comparf)(const Void_t*, const Void_t*) )
#else
Void_t* tfind(key, rootp, comparf)
Void_t*		key;
Void_t**	rootp;
int(*		comparf)();
#endif
{
	reg Dt_t*	dt;
	reg Tree_t*	o;

	if(!rootp || !(dt = *((Dt_t**)rootp)) )
		return NIL(Void_t*);
	Treedisc.comparf = comparf;

	return (o = (Tree_t*)dtmatch(dt,key)) ? (Void_t*)(&o->key) : NIL(Void_t*);
}

/* the original tdelete() specifies that it will return the parent pointer
** in the tree if there is one. Since we are using a splay tree, a deleted
** node is always rotated to the root first. So this implementation always
** returns the key of the new root.
*/
#if __STD_C
Void_t* tdelete(const Void_t* key, Void_t** rootp,
		int(*comparf)(const Void_t*, const Void_t*) )
#else
Void_t* tdelete(key, rootp, comparf)
Void_t*		key;
Void_t**	rootp;
int(*		comparf)();
#endif
{
	reg Dt_t*	dt;
	reg Tree_t*	o;
	Tree_t		obj;

	if(!rootp || !(dt = *((Dt_t**)rootp)) )
		return NIL(Void_t*);

	Treedisc.comparf = comparf;

	obj.key = (Void_t*)key;
	dtdelete(dt,&obj);

	if(!(o = dtfinger(dt)) )
	{	dtclose(dt);
		*rootp = NIL(Void_t*);
	}

	return o ? (Void_t*)(&o->key) : NIL(Void_t*);
}

/* the below routine assumes a particular layout of Dtlink_t.
** If this ever gets changed, this routine should be redone.
*/
#define lchild	link.hl._left
#define rchild	link.right

#if __STD_C
static void _twalk(Tree_t* obj, void(*action)(Void_t*,VISIT,int), int level)
#else
static void _twalk(obj,action,level)
Tree_t*	obj;
void(*		action)();
int		level;
#endif
{	if(!obj->lchild && !obj->rchild)
		(*action)((Void_t*)obj,leaf,level);
	else
	{	(*action)((Void_t*)obj,preorder,level);
		if(obj->lchild)
			_twalk((Tree_t*)obj->lchild,action,level+1);
		(*action)((Void_t*)obj,postorder,level);
		if(obj->rchild)
			_twalk((Tree_t*)obj->rchild,action,level+1);
		(*action)((Void_t*)obj,endorder,level);
	}
}

/* the original twalk allows specifying arbitrary node to start traversal.
** Since our root is a dictionary structure, the search here will start
** at whichever node happens to be current root.
*/
#if __STD_C
void twalk(Void_t* root, void(*action)(Void_t*,VISIT,int) )
#else
void twalk(root, action)
Void_t*	root;
void(*	action)();
#endif
{
	reg Tree_t*	o;

	if(root && (o = (Tree_t*)dtfinger((Dt_t*)root)) )
		_twalk(o,action,0);
}
