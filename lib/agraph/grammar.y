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

%{

#include <stdio.h>  /* SAFE */
#include "aghdr.h"	/* SAFE */

#ifdef _WIN32
#define gettxt(a,b)	(b)
#endif

static char Key[] = "key";

typedef union s {					/* possible items in generic list */
		Agnode_t		*n;
		Agraph_t		*subg;
		Agedge_t		*e;
		Agsym_t			*asym;	/* bound attribute */
		char			*name;	/* unbound attribute */
		struct item_s	*list;	/* list-of-lists (for edgestmt) */
} val_t;

typedef struct item_s {		/* generic list */
	int				tag;	/* T_node, T_subgraph, T_edge, T_attr */
	val_t			u;		/* primary element */
	char			*str;	/* secondary value - port or attr value */
	struct item_s	*next;
} item;

typedef struct list_s {		/* maintain head and tail ptrs for fast append */
	item			*first;
	item			*last;
} list_t;

/* functions */
static void appendnode(char *name, char *port, char *sport);
static void attrstmt(int tkind, char *macroname);
static void startgraph(char *name, int directed, int strict);
static void bufferedges(void);
static void newedge(Agnode_t *t, char *tport, Agnode_t *h, char *hport, char *key);
static void edgerhs(Agnode_t *n, char *tport, item *hlist, char *key);
static void appendattr(char *name, char *value);
static void bindattrs(int kind);
static void applyattrs(void *obj);
static void endgraph(void);
static void endnode(void);
static void endedge(void);
static char* concat(char*, char*);
static char* concatPort(char*, char*);

static void opensubg(char *name);
static void closesubg(void);

/* global */
static Agraph_t *G;

%}

%union	{
			int				i;
			char			*str;
			struct Agnode_s	*n;
}

%token <i> T_graph T_node T_edge T_digraph T_subgraph T_strict T_edgeop
	/* T_list, T_attr are internal tags, not really tokens */
%token T_list T_attr
%token <str> T_atom T_qatom

%type <i>  optstrict graphtype rcompound attrtype
%type <str> optsubghdr optgraphname optmacroname atom qatom


%%

graph		:  hdr body {endgraph();}
			|  error	{if (G) {agclose(G); G = Ag_G_global = NIL(Agraph_t*);}}
			|  /* empty */
			;

body		: '{' optstmtlist '}' ;

hdr			:	optstrict graphtype optgraphname {startgraph($3,$2,$1);}
			;

optgraphname:	atom {$$=$1;} | /* empty */ {$$=0;} ;

optstrict	:  T_strict  {$$=1;} |  /* empty */ {$$=0;} ;

graphtype	:	T_graph {$$ = 0;} |	T_digraph {$$ = 1;} ;

optstmtlist	:	stmtlist |	/* empty */ ;

stmtlist 	:	stmtlist stmt |	stmt ;

optsemi		: ';' | ;

stmt		:  attrstmt  optsemi
			|  compound	 optsemi
			;

compound 	:	simple rcompound optattr
					{if ($2) endedge(); else endnode();}
			;

simple		:	nodelist | subgraph ;

rcompound	:	T_edgeop {bufferedges();} simple rcompound {$$ = 1;}
			|	/* empty */ {$$ = 0;}
			;


nodelist	: node | nodelist ',' node ;

node		: atom {appendnode($1,NIL(char*),NIL(char*));}
            | atom ':' atom {appendnode($1,$3,NIL(char*));}
            | atom ':' atom ':' atom {appendnode($1,$3,$5);}
            ;

attrstmt	:  attrtype optmacroname attrlist {attrstmt($1,$2);}
			|  graphattrdefs {attrstmt(T_graph,NIL(char*));}
			;

attrtype :	T_graph {$$ = T_graph;}
			| T_node {$$ = T_node;}
			| T_edge {$$ = T_edge;}
			;

optmacroname : atom '=' {$$ = $1;}
			| /* empty */ {$$ = NIL(char*); }
			;

optattr		:	attrlist |  /* empty */ ;

attrlist	: optattr '[' optattrdefs ']' ;

optattrdefs	: optattrdefs attrdefs 
			| /* empty */ ;

attrdefs	:  attritem optseparator
			;

attritem	: attrassignment | attrmacro ; 

attrassignment	:  atom '=' atom {appendattr($1,$3);}
			;

attrmacro	:	'@' atom {appendattr($2,NIL(char*));}	/* not yet impl */
			;

graphattrdefs : attrassignment
			;

subgraph	:  optsubghdr {opensubg($1);} body {closesubg();}
			;

optsubghdr	: T_subgraph atom {$$=$2;}
			| T_subgraph  {$$=NIL(char*);}
			| /* empty */ {$$=NIL(char*);}
			;

optseparator :  ';' | ',' | /*empty*/ ;

atom	:  T_atom {$$ = $1;}
			|  qatom {$$ = $1;}
			;

qatom	:  T_qatom {$$ = $1;}
			|  qatom '+' T_qatom {$$ = concat($1,$3);}
			;
%%

#define NILitem  NIL(item*)

/* globals */
static	Agraph_t	*Subgraph;	/* most recent subgraph that was opened */
static	Agdisc_t	*Disc;		/* discipline passed to agread or agconcat */
static	list_t	Nodelist,Edgelist,Attrlist;

static item *newitem(int tag, void *p0, char *p1)
{
	item	*rv = agalloc(G,sizeof(item));
	rv->tag = tag; rv->u.name = (char*)p0; rv->str = p1;
	return rv;
}

static item *cons_node(Agnode_t *n, char *port)
	{ return newitem(T_node,n,port); }

static item *cons_attr(char *name, char *value)
	{ return newitem(T_atom,name,value); }

static item *cons_list(item *list)
	{ return newitem(T_list,list,NIL(char*)); }

static item *cons_subg(Agraph_t *subg)
	{ return newitem(T_subgraph,subg,NIL(char*)); }

#ifdef NOTDEF
static item *cons_edge(Agedge_t *e)
	{ return newitem(T_edge,e,NIL(char*)); }
#endif

static void delete_items(item *ilist)
{
	item	*p,*pn;

	for (p = ilist; p; p = pn) {
		pn = p->next;
		switch(p->tag) {
			case T_list: delete_items(p->u.list); break;
			case T_atom: case T_attr: agstrfree(G,p->str); break;
		}
		agfree(G,p);
	}
}

static void deletelist(list_t *list)
{
	delete_items(list->first);
	list->first = list->last = NILitem;
}

#ifdef NOTDEF
static void listins(list_t *list, item *v)
{
	v->next = list->first;
	list->first = v;
	if (list->last == NILitem) list->last = v;
}
#endif

static void listapp(list_t *list, item *v)
{
	if (list->last) list->last->next = v;
	list->last = v;
	if (list->first == NILitem) list->first = v;
}


/* attrs */
static void appendattr(char *name, char *value)
{
	item		*v;

	assert(value != NIL(char*));
	v = cons_attr(name,value);
	listapp(&Attrlist,v);
}

static void bindattrs(int kind)
{
	item		*aptr;
	char		*name;

	for (aptr = Attrlist.first; aptr; aptr = aptr->next) {
		assert(aptr->tag == T_atom);	/* signifies unbound attr */
		name = aptr->u.name;
		if ((kind == AGEDGE) && streq(name,Key)) continue;
		if ((aptr->u.asym = agattr(G,kind,name,NIL(char*))) == NILsym)
			aptr->u.asym = agattr(G,kind,name,"");
		aptr->tag = T_attr;				/* signifies bound attr */
		agstrfree(G,name);
	}
}

static void applyattrs(void *obj)
{
	item		*aptr;

	for (aptr = Attrlist.first; aptr; aptr = aptr->next) {
		if (aptr->tag == T_attr) {
			if (aptr->u.asym) {
				agxset(obj,aptr->u.asym,aptr->str);
			}
		}
		else {
			assert(AGTYPE(obj) == AGEDGE);
			assert(aptr->tag == T_atom);
			assert(streq(aptr->u.name,Key));
		}
	}
}

static void nomacros(void)
{
	agerror(AGERROR_UNIMPL,"attribute macros");
}

static void attrstmt(int tkind, char *macroname)
{
	item			*aptr;
	int				kind;

		/* creating a macro def */
	if (macroname) nomacros();
		/* invoking a macro def */
	for (aptr = Attrlist.first; aptr; aptr = aptr->next)
		if (aptr->str == NIL(char*)) nomacros();

	switch(tkind) {
		case T_graph: kind = AGRAPH; break;
		case T_node: kind = AGNODE; break;
		case T_edge: kind = AGEDGE; break;
		default : abort();
	}
	bindattrs(kind);	/* set up defaults for new attributes */
	for (aptr = Attrlist.first; aptr; aptr = aptr->next)
		agattr(G,kind,aptr->u.asym->name,aptr->str);
	deletelist(&Attrlist);
}

/* nodes */

static void appendnode(char *name, char *port, char *sport)
{
	item		*elt;

	if (sport) {
		port = concatPort (port, sport);
	}
	elt = cons_node(agnode(G,name,TRUE),port);
	listapp(&Nodelist,elt);
	agstrfree(G,name);
}

	/* apply current optional attrs to Nodelist and clean up lists */
static void endnode()
{
	item	*ptr;

	bindattrs(AGNODE);
	for (ptr = Nodelist.first; ptr; ptr = ptr->next)
		applyattrs(ptr->u.n);
	deletelist(&Nodelist);
	deletelist(&Attrlist);
}

/* edges - store up node/subg lists until optional edge key can be seen */

static void bufferedges()
{
	item	*v;

	if (Nodelist.first) {
		v = cons_list(Nodelist.first);
		Nodelist.first = Nodelist.last = NILitem;
	}
	else {v = cons_subg(Subgraph); Subgraph = NIL(Agraph_t*);}
	listapp(&Edgelist,v);
}

static void endedge(void)
{
	char			*key;
	item			*aptr,*tptr,*p;

	Agnode_t		*t;
	Agraph_t		*subg;

	bufferedges();	/* pick up the terminal nodelist or subg */
	bindattrs(AGEDGE);

	/* look for "key" pseudo-attribute */
	key = NIL(char*);
	for (aptr = Attrlist.first; aptr; aptr = aptr->next) {
		if ((aptr->tag == T_atom) && streq(aptr->u.name,Key))
			key = aptr->str;
	}

	/* can make edges with node lists or subgraphs */
	for (p = Edgelist.first; p->next; p = p->next) {
		if (p->tag == T_subgraph) {
			subg = p->u.subg;
			for (t = agfstnode(subg); t; t = agnxtnode(t))
				edgerhs(agsubnode(G,t,FALSE),NIL(char*),p->next,key);
		}
		else {
			for (tptr = p->u.list; tptr; tptr = tptr->next)
				edgerhs(tptr->u.n,tptr->str,p->next,key);
		}
	}
	deletelist(&Edgelist);
	deletelist(&Attrlist);
}

/* concat:
 */
static char*
concat (char* s1, char* s2)
{
  char*  s;
  char   buf[BUFSIZ];
  char*  sym;
  int    len = strlen(s1) + strlen(s2) + 1;

  if (len <= BUFSIZ) sym = buf;
  else sym = (char*)malloc(len);
  strcpy(sym,s1);
  strcat(sym,s2);
  s = agstrdup (G,sym);
  agstrfree (G,s1);
  agstrfree (G,s2);
  if (sym != buf) free (sym);
  return s;
}

/* concatPort:
 */
static char*
concatPort (char* s1, char* s2)
{
  char*  s;
  char   buf[BUFSIZ];
  char*  sym;
  int    len = strlen(s1) + strlen(s2) + 2;  /* one more for ':' */

  if (len <= BUFSIZ) sym = buf;
  else sym = (char*)malloc(len);
  sprintf (sym, "%s:%s", s1, s2);
  s = agstrdup (G,sym);
  agstrfree (G,s1);
  agstrfree (G,s2);
  if (sym != buf) free (sym);
  return s;
}


static void edgerhs(Agnode_t *tail, char *tport, item *hlist, char *key)
{
	Agnode_t		*head;
	Agraph_t		*subg;
	item			*hptr;

	if (hlist->tag == T_subgraph) {
		subg = hlist->u.subg;
		for (head = agfstnode(subg); head; head = agnxtnode(head))
			newedge(tail,tport,agsubnode(G,head,FALSE),NIL(char*),key);
	}
	else {
		for (hptr = hlist->u.list; hptr; hptr = hptr->next)
			newedge(tail,tport,agsubnode(G,hptr->u.n,FALSE),hptr->str,key);
	}
}

static void mkport(Agedge_t *e, char *name, char *val)
{
	Agsym_t *attr;
	if (val) {
		if ((attr = agattr(G,AGEDGE,name,NIL(char*))) == NILsym)
			attr = agattr(G,AGEDGE,name,"");
		agxset(e,attr,val);
	}
}

static void newedge(Agnode_t *t, char *tport, Agnode_t *h, char *hport, char *key)
{
	Agedge_t 	*e;

	e = agedge(t,h,key,TRUE);
	if (e) {		/* can fail if graph is strict and t==h */
		char    *tp = tport;
		char    *hp = hport;
		if ((agtail(e) != aghead(e)) && (aghead(e) == t)) {
			/* could happen with an undirected edge */
			char    *temp;
			temp = tp; tp = hp; hp = temp;
		}
		mkport(e,TAIL_ID,tp);
		mkport(e,HEAD_ID,hp);
		applyattrs(e);
	}
}

/* graphs and subgraphs */


static void startgraph(char *name, int directed, int strict)
{
	static Agdesc_t	req;	/* get rid of warnings */

	if (G == NILgraph) {
		req.directed = directed;
		req.strict = strict;
		req.flatlock = FALSE;
		req.maingraph = TRUE;
		Ag_G_global = G = agopen(name,req,Disc);
	}
	else {
		Ag_G_global = G;
	}
	agstrfree(NIL(Agraph_t*),name);
}

static void endgraph()
{
	aglexeof();
	aginternalmapclearlocalnames(G);
}

static void opensubg(char *name)
{
	G = agsubg(G,name,TRUE);
	agstrfree(G,name);
}

static void closesubg()
{
	Subgraph = G;
	if ((G = agparent(G)) == NIL(Agraph_t*))
		yyerror("libgraph: parser lost root graph\n");
}

extern FILE *yyin;
Agraph_t *agconcat(Agraph_t *g, void *chan, Agdisc_t *disc)
{
	yyin = chan;
	G = g;
	Ag_G_global = NILgraph;
	Disc = (disc? disc :  &AgDefaultDisc);
	aglexinit(Disc, chan);
	yyparse();
	return Ag_G_global;
}

Agraph_t *agread(void *fp, Agdisc_t *disc) {return agconcat(NILgraph,fp,disc); }
