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


/*
 *  Compile-time and run-time interface between gpr and libexpr
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdlib.h>
#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif
#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <compile.h>
#include <assert.h>
#include <agraph.h>
#include <error.h>
#include <actions.h>
#include <sfstr.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#define ISEDGE(e) (AGTYPE(e)&2)

#include <gdefs.h>

#define BITS_PER_BYTE 8
#ifdef HAVE_INTPTR_T
#define INT2PTR(t,v) ((t)(intptr_t)(v))
#define PTR2INT(v) ((Sflong_t)(intptr_t)(v))
#else
#define INT2PTR(t,v) ((t)(v))
#define PTR2INT(v) ((Sflong_t)(v))
#endif

/* nameOf:
 * Return name of object. 
 * Assumes obj !=  NULL
 */
static char *nameOf(Expr_t * ex, Agobj_t * obj)
{
    Sfio_t *tmps;
    char *s;
    char *key;
    Agedge_t *e;

    switch (AGTYPE(obj)) {
    case AGNODE:
    case AGRAPH:
	s = agnameof(obj);
	break;
    default:			/* edge */
	tmps = sfstropen();
	e = (Agedge_t *) obj;
	key = agnameof(obj);
	sfputr(tmps, agnameof(AGTAIL(e)), -1);
	if (agisdirected(agraphof(e)))
	    sfputr(tmps, "->", -1);
	else
	    sfputr(tmps, "--", -1);
	sfputr(tmps, agnameof(AGHEAD(e)), -1);
	if (*key) {
	    sfputc(tmps, '[');
	    sfputr(tmps, key, -1);
	    sfputc(tmps, ']');
	}
	s = exstring(ex, sfstruse(tmps));
	sfclose(tmps);
	break;
    }
    return s;
}

/* bbOf:
 * If string as form "x,y,u,v" where is all are numeric,
 * return "x,y" or "u,v", depending on getll, else return ""
 */
static char *bbOf(Expr_t * pgm, char *pt, int getll)
{
    double x, y, u, v;
    char *s;
    char *p;
    int len;

    if (sscanf(pt, "%lf,%lf,%lf,%lf", &x, &y, &u, &v) == 4) {
	p = strchr(pt, ',');
	p = strchr(p + 1, ',');
	if (getll) {
	    len = p - pt;
	    s = exstralloc(pgm, 0, len + 1);
	    strncpy(s, pt, len);
	    s[len] = '\0';
	} else
	    s = exstring(pgm, p + 1);
    } else
	s = "";
    return s;
}

/* xyOf:
 * If string as form "x,y" where is x and y are numeric,
 * return "x" or "y", depending on getx, else return ""
 */
static char *xyOf(Expr_t * pgm, char *pt, int getx)
{
    double x, y;
    char *v;
    char *p;
    int len;

    if (sscanf(pt, "%lf,%lf", &x, &y) == 2) {
	p = strchr(pt, ',');
	if (getx) {
	    len = p - pt;
	    v = exstralloc(pgm, 0, len + 1);
	    strncpy(v, pt, len);
	    v[len] = '\0';
	} else
	    v = exstring(pgm, p + 1);
    } else
	v = "";
    return v;
}

#ifdef DEBUG
static char *symName(Expr_t * ex, int op)
{
    if (op >= MINNAME && op <= MAXNAME)
	return gprnames[op];
    else {
	Sfio_t *sf = sfstropen();
	char *s;

	sfprintf(sf, "<unknown (%d)>", op);
	s = exstring(ex, sfstruse(sf));
	sfclose(sf);
	return s;
    }
}
#endif

/* xargs:
 * Convert string argument to graph to type of graph desired.
 *   u => undirected
 *   d => directed
 *   s => strict
 *   n => non-strict
 * Case-insensitive
 * By default, the graph is directed, non-strict.
 */
static Agdesc_t xargs(char *args)
{
    Agdesc_t desc = Agdirected;
    char c;

    while ((c = *args++)) {
	switch (c) {
	case 'u':
	case 'U':
	    desc.directed = 0;
	    break;
	case 'd':
	case 'D':
	    desc.directed = 1;
	    break;
	case 's':
	case 'S':
	    desc.strict = 1;
	    break;
	case 'n':
	case 'N':
	    desc.directed = 0;
	    break;
	default:
	    error(ERROR_WARNING, "unknown graph descriptor '%c' : ignored",
		  c);
	    break;
	}
    }
    return desc;
}

/* deparse:
 * Recreate string representation of expression involving
 * a reference and a symbol.
 * The parameter sf must be a string stream.
 */
static char *deparse(Expr_t * ex, Exnode_t * n, Sfio_t * sf)
{
    exdump(ex, n, sf);
    return (sfstruse(sf));
}

/* deref:
 * Evaluate reference to derive desired graph object.
 * A reference is either DI* or II*
 * The parameter objp is the current object.
 * Assume ref is type-correct.
 */
static Agobj_t *deref(Expr_t * pgm, Exnode_t * x, Exref_t * ref,
		      Agobj_t * objp, Gpr_t * state)
{
    void *ptr;

    if (ref == 0)
	return objp;
    else if (ref->symbol->lex == DYNAMIC) {
	ptr =
	    INT2PTR(void *,
		    x->data.variable.dyna->data.variable.dyna->data.
		    constant.value.integer);
	if (!ptr) {
	    error(ERROR_FATAL, "null reference %s in expression %s.%s",
		  ref->symbol->name, ref->symbol->name, deparse(pgm, x,
								state->
								tmp));
	    return ptr;
	} else
	    return deref(pgm, x, ref->next, (Agobj_t *) ptr, state);
    } else
	switch (ref->symbol->index) {	/* sym->lex == ID */
	case V_outgraph:
	    return deref(pgm, x, ref->next, (Agobj_t *) state->outgraph,
			 state);
	    break;
	case V_this:
	    return deref(pgm, x, ref->next, state->curobj, state);
	    break;
	case V_thisg:
	    return deref(pgm, x, ref->next, (Agobj_t *) state->curgraph,
			 state);
	    break;
	case V_targt:
	    return deref(pgm, x, ref->next, (Agobj_t *) state->target,
			 state);
	    break;
	case M_head:
	    if (!objp && !(objp = state->curobj)) {
		error(ERROR_WARNING, "Current object $ not defined");
		return 0;
	    }
	    if (ISEDGE(objp))
		return deref(pgm, x, ref->next,
			     (Agobj_t *) AGHEAD((Agedge_t *) objp), state);
	    else
		error(ERROR_FATAL, "head of non-edge");
	    break;
	case M_tail:
	    if (!objp && !(objp = state->curobj)) {
		error(ERROR_FATAL, "Current object $ not defined");
		return 0;
	    }
	    if (ISEDGE(objp))
		return deref(pgm, x, ref->next,
			     (Agobj_t *) AGTAIL((Agedge_t *) objp), state);
	    else
		error(ERROR_FATAL, "tail of non-edge %x", objp);
	    break;
	default:
	    error(ERROR_WARNING, "%s : illegal reference",
		  ref->symbol->name);
	    return 0;
	    break;
	}
    return 0;

}

/* setattr:
 * Set object's attribute name to val.
 * Initialize attribute if necessary.
 */
static int
setattr (Agobj_t *objp, char* name, char* val)
{
    Agsym_t *gsym = agattrsym(objp, name);
    if (!gsym) {
	gsym = agattr(agroot(agraphof(objp)), AGTYPE(objp), name, "");
    }
    return agxset(objp, gsym, val);
}

/* lookup:
 * Apply symbol to get field value of objp
 * Assume objp != NULL
 */
static int lookup(Expr_t * pgm, Agobj_t * objp, Exid_t * sym, Extype_t * v)
{
    if (sym->lex == ID) {
	switch (sym->index) {
	case M_head:
	    if (ISEDGE(objp))
		v->integer = PTR2INT(AGHEAD((Agedge_t *) objp));
	    else {
		error(ERROR_WARNING, "head of non-edge");
		return -1;
	    }
	    break;
	case M_tail:
	    if (ISEDGE(objp))
		v->integer = PTR2INT(AGTAIL((Agedge_t *) objp));
	    else {
		error(ERROR_WARNING, "tail of non-edge");
		return -1;
	    }
	    break;
	case M_name:
	    v->string = nameOf(pgm, objp);
	    break;
	case M_indegree:
	    if (AGTYPE(objp) == AGNODE)
		v->integer = agdegree((Agnode_t *) objp, 1, 0);
	    else {
		error(ERROR_FATAL, "indegree of non-node");
		return -1;
	    }
	    break;
	case M_outdegree:
	    if (AGTYPE(objp) == AGNODE)
		v->integer = agdegree((Agnode_t *) objp, 0, 1);
	    else {
		error(ERROR_FATAL, "outdegree of non-node");
		return -1;
	    }
	    break;
	case M_degree:
	    if (AGTYPE(objp) == AGNODE)
		v->integer = agdegree((Agnode_t *) objp, 1, 1);
	    else {
		error(ERROR_FATAL, "degree of non-node");
		return -1;
	    }
	    break;
	case M_parent:
	    if (AGTYPE(objp) == AGRAPH)
		v->integer = PTR2INT(agparent((Agraph_t *) objp));
	    else {
		error(ERROR_FATAL, "root of non-graph");
		return -1;
	    }
	    break;
	case M_root:
	    v->integer = PTR2INT(agroot(agraphof(objp)));
	    break;
	case M_n_edges:
	    if (AGTYPE(objp) == AGRAPH)
		v->integer = agnedges((Agraph_t *) objp);
	    else {
		error(ERROR_FATAL, "n_edges of non-graph");
		return -1;
	    }
	    break;
	case M_n_nodes:
	    if (AGTYPE(objp) == AGRAPH)
		v->integer = agnnodes((Agraph_t *) objp);
	    else {
		error(ERROR_FATAL, "n_nodes of non-graph");
		return -1;
	    }
	    break;
	case M_directed:
	    if (AGTYPE(objp) == AGRAPH)
		v->integer = agisdirected((Agraph_t *) objp);
	    else {
		error(ERROR_FATAL, "directed of non-graph");
		return -1;
	    }
	    break;
	case M_strict:
	    if (AGTYPE(objp) == AGRAPH)
		v->integer = agisstrict((Agraph_t *) objp);
	    else {
		error(ERROR_FATAL, "strict of non-graph");
		return -1;
	    }
	    break;
	default:
	    error(ERROR_WARNING, "%s : illegal reference", sym->name);
	    return -1;
	    break;
	}
    } else
	v->string = agget(objp, sym->name);

    return 0;
}

/* getArg:
 * Return value associated with $n.
 */
static char *getArg(int n, Gpr_t * state)
{
    if (n >= state->argc) {
	error(ERROR_FATAL, "program references ARGV[%d] - undefined", n);
    }
    return (state->argv[n]);
}

/* getval:
 * Return value associated with gpr identifier.
 */
Extype_t
getval(Expr_t * pgm, Exnode_t * node, Exid_t * sym, Exref_t * ref,
       void *env, int elt, Exdisc_t * disc)
{
    Extype_t v;
    Gpr_t *state;
    Extype_t *args;
    Agobj_t *objp;
    Agobj_t *objp1;
    char *key;
    Agraph_t *gp;
    Agnode_t *np;
    Agnode_t *hp;
    Agedge_t *ep;

    assert(sym->lex != CONSTANT);
    if (elt == EX_CALL) {
	args = (Extype_t *) env;
	state = (Gpr_t *) (disc->user);
	switch (sym->index) {
	case F_graph:
	    gp = openG(args[0].string, xargs(args[1].string));
	    v.integer = PTR2INT(gp);
	    break;
	case F_subg:
	    gp = INT2PTR(Agraph_t *, args[0].integer);
	    if (gp) {
		gp = openSubg(gp, args[1].string);
		v.integer = PTR2INT(gp);
	    }
	    break;
	case F_issubg:
	    gp = INT2PTR(Agraph_t *, args[0].integer);
	    if (gp) {
		v.integer = PTR2INT(agsubg(gp, args[1].string, 0));
	    } else {
		error(ERROR_WARNING, "NULL graph passed to isSubg()");
		v.integer = 0;
	    }
	    break;
	case F_fstsubg:
	    gp = INT2PTR(Agraph_t *, args[0].integer);
	    if (gp) {
		gp = agfstsubg(gp);
		v.integer = PTR2INT(gp);
	    } else {
		error(ERROR_WARNING, "NULL graph passed to fstsubg()");
		v.integer = 0;
	    }
	    break;
	case F_nxtsubg:
	    gp = INT2PTR(Agraph_t *, args[0].integer);
	    if (gp) {
		gp = agnxtsubg(gp);
		v.integer = PTR2INT(gp);
	    } else {
		error(ERROR_WARNING, "NULL graph passed to nxtsubg()");
		v.integer = 0;
	    }
	    break;
	case F_node:
	    gp = INT2PTR(Agraph_t *, args[0].integer);
	    if (gp) {
		np = openNode(gp, args[1].string);
		v.integer = PTR2INT(np);
	    } else {
		error(ERROR_WARNING, "NULL graph passed to node()");
		v.integer = 0;
	    }
	    break;
	case F_addnode:
	    gp = INT2PTR(Agraph_t *, args[0].integer);
	    np = INT2PTR(Agnode_t *, args[1].integer);
	    if (!gp) {
		error(ERROR_WARNING, "NULL graph passed to addNode()");
		v.integer = 0;
	    } else if (!np) {
		error(ERROR_WARNING, "NULL node passed to addNode()");
		v.integer = 0;
	    } else
		v.integer = PTR2INT(addNode(gp, np));
	    break;
	case F_fstnode:
	    gp = INT2PTR(Agraph_t *, args[0].integer);
	    if (gp) {
		np = agfstnode(gp);
		v.integer = PTR2INT(np);
	    } else {
		error(ERROR_WARNING, "NULL graph passed to fstnode()");
		v.integer = 0;
	    }
	    break;
	case F_nxtnode:
	    np = INT2PTR(Agnode_t *, args[0].integer);
	    if (np) {
		np = agnxtnode(np);
		v.integer = PTR2INT(np);
	    } else {
		error(ERROR_WARNING, "NULL node passed to nxtnode()");
		v.integer = 0;
	    }
	    break;
	case F_isnode:
	    gp = INT2PTR(Agraph_t *, args[0].integer);
	    if (gp) {
		v.integer = PTR2INT(agnode(gp, args[1].string, 0));
	    } else {
		error(ERROR_WARNING, "NULL graph passed to isNode()");
		v.integer = 0;
	    }
	    break;
	case F_isin:
	    gp = INT2PTR(Agraph_t *, args[0].integer);
	    objp = INT2PTR(Agobj_t *, args[1].integer);
	    if (!gp) {
		error(ERROR_WARNING, "NULL graph passed to isIn()");
		v.integer = 0;
	    } else if (!objp) {
		error(ERROR_WARNING, "NULL object passed to isIn()");
		v.integer = 0;
	    } else
		v.integer = isIn(gp, objp);
	    break;
	case F_compof:
	    gp = INT2PTR(Agraph_t *, args[0].integer);
	    np = INT2PTR(Agnode_t *, args[1].integer);
	    if (!gp) {
		error(ERROR_WARNING, "NULL graph passed to compOf()");
		v.integer = 0;
	    } else if (!np) {
		error(ERROR_WARNING, "NULL node passed to compOf()");
		v.integer = 0;
	    } else
		v.integer = PTR2INT(compOf(gp, np));
	    break;
	case F_edge:
	    key = args[2].string;
	    if (*key == '\0')
		key = 0;
	    np = INT2PTR(Agnode_t *, args[0].integer);
	    hp = INT2PTR(Agnode_t *, args[1].integer);
	    if (!np) {
		error(ERROR_WARNING, "NULL tail node passed to edge()");
		v.integer = 0;
	    } else if (!hp) {
		error(ERROR_WARNING, "NULL head node passed to edge()");
		v.integer = 0;
	    } else {
		ep = openEdge(np, hp, key);
		v.integer = PTR2INT(ep);
	    }
	    break;
	case F_addedge:
	    gp = INT2PTR(Agraph_t *, args[0].integer);
	    ep = INT2PTR(Agedge_t *, args[1].integer);
	    if (!gp) {
		error(ERROR_WARNING, "NULL graph passed to addEdge()");
		v.integer = 0;
	    } else if (!ep) {
		error(ERROR_WARNING, "NULL edge passed to addEdge()");
		v.integer = 0;
	    } else
		v.integer = PTR2INT(addEdge(gp, ep));
	    break;
	case F_isedge:
	    key = args[2].string;
	    if (*key == '\0')
		key = 0;
	    np = INT2PTR(Agnode_t *, args[0].integer);
	    hp = INT2PTR(Agnode_t *, args[1].integer);
	    if (!np) {
		error(ERROR_WARNING, "NULL tail node passed to isEdge()");
		v.integer = 0;
	    } else if (!hp) {
		error(ERROR_WARNING, "NULL head node passed to isEdge()");
		v.integer = 0;
	    } else
		v.integer = PTR2INT(isEdge(np, hp, key));
	    break;
	case F_fstout:
	    np = INT2PTR(Agnode_t *, args[0].integer);
	    if (np) {
		ep = agfstout(np);
		v.integer = PTR2INT(ep);
	    } else {
		error(ERROR_WARNING, "NULL node passed to fstout()");
		v.integer = 0;
	    }
	    break;
	case F_nxtout:
	    ep = INT2PTR(Agedge_t *, args[0].integer);
	    if (ep) {
		ep = agnxtout(ep);
		v.integer = PTR2INT(ep);
	    } else {
		error(ERROR_WARNING, "NULL edge passed to nxtout()");
		v.integer = 0;
	    }
	    break;
	case F_fstin:
	    np = INT2PTR(Agnode_t *, args[0].integer);
	    if (np) {
		ep = agfstin(np);
		v.integer = PTR2INT(ep);
	    } else {
		error(ERROR_WARNING, "NULL node passed to fstin()");
		v.integer = 0;
	    }
	    break;
	case F_nxtin:
	    ep = INT2PTR(Agedge_t *, args[0].integer);
	    if (ep) {
		ep = agnxtin(ep);
		v.integer = PTR2INT(ep);
	    } else {
		error(ERROR_WARNING, "NULL edge passed to nxtin()");
		v.integer = 0;
	    }
	    break;
	case F_fstedge:
	    np = INT2PTR(Agnode_t *, args[0].integer);
	    if (np) {
		ep = agfstedge(np);
		v.integer = PTR2INT(ep);
	    } else {
		error(ERROR_WARNING, "NULL node passed to fstedge()");
		v.integer = 0;
	    }
	    break;
	case F_nxtedge:
	    ep = INT2PTR(Agedge_t *, args[0].integer);
	    np = INT2PTR(Agnode_t *, args[1].integer);
	    if (!ep) {
		error(ERROR_WARNING, "NULL edge passed to nxtedge()");
		v.integer = 0;
	    } else if (!np) {
		error(ERROR_WARNING, "NULL node passed to nxtedge()");
		v.integer = 0;
	    } else {
		ep = agnxtedge(ep, np);
		v.integer = PTR2INT(ep);
	    }
	    break;
	case F_copy:
	    gp = INT2PTR(Agraph_t *, args[0].integer);
	    objp = INT2PTR(Agobj_t *, args[1].integer);
	    if (!objp) {
		error(ERROR_WARNING, "NULL object passed to clone()");
		v.integer = 0;
	    } else
		v.integer = PTR2INT(copy(gp, objp));
	    break;
	case F_clone:
	    gp = INT2PTR(Agraph_t *, args[0].integer);
	    objp = INT2PTR(Agobj_t *, args[1].integer);
	    if (!objp) {
		error(ERROR_WARNING, "NULL object passed to clone()");
		v.integer = 0;
	    } else
		v.integer = PTR2INT(clone(gp, objp));
	    break;
	case F_copya:
	    objp = INT2PTR(Agobj_t *, args[0].integer);
	    objp1 = INT2PTR(Agobj_t *, args[1].integer);
	    if (!(objp && objp1)) {
		error(ERROR_WARNING, "NULL object passed to copyA()");
		v.integer = 0;
	    } else
		v.integer = copyAttr(objp, objp1);
	    break;
	case F_induce:
	    gp = INT2PTR(Agraph_t *, args[0].integer);
	    if (!gp) {
		error(ERROR_WARNING, "NULL graph passed to induce()");
		v.integer = 1;
	    } else {
		nodeInduce(gp);
		v.integer = 0;
	    }
	    break;
	case F_write:
	    gp = INT2PTR(Agraph_t *, args[0].integer);
	    if (!gp) {
		error(ERROR_WARNING, "NULL graph passed to write()");
		v.integer = 1;
	    } else
		v.integer = agwrite(gp, state->outFile);
	    break;
	case F_writeg:
	    gp = INT2PTR(Agraph_t *, args[0].integer);
	    if (!gp) {
		error(ERROR_WARNING, "NULL graph passed to writeG()");
		v.integer = 1;
	    } else
		v.integer = writeFile(gp, args[1].string);
	    break;
	case F_readg:
	    gp = readFile(args[0].string);
	    v.integer = PTR2INT(gp);
	    break;
	case F_fwriteg:
	    gp = INT2PTR(Agraph_t *, args[0].integer);
	    if (!gp) {
		error(ERROR_WARNING, "NULL graph passed to fwriteG()");
		v.integer = 1;
	    } else
		v.integer = fwriteFile(pgm, gp, args[1].integer);
	    break;
	case F_freadg:
	    gp = freadFile(pgm, args[0].integer);
	    v.integer = PTR2INT(gp);
	    break;
	case F_openf:
	    v.integer = openFile(pgm, args[0].string, args[1].string);
	    break;
	case F_closef:
	    v.integer = closeFile(pgm, args[0].integer);
	    break;
	case F_readl:
	    v.string = readLine(pgm, args[0].integer);
	    break;
	case F_isdirect:
	    gp = INT2PTR(Agraph_t *, args[0].integer);
	    if (!gp) {
		error(ERROR_WARNING, "NULL graph passed to isDirect()");
		v.integer = 0;
	    } else {
		v.integer = agisdirected(gp);
	    }
	    break;
	case F_isstrict:
	    gp = INT2PTR(Agraph_t *, args[0].integer);
	    if (!gp) {
		error(ERROR_WARNING, "NULL graph passed to isStrict()");
		v.integer = 0;
	    } else {
		v.integer = agisstrict(gp);
	    }
	    break;
	case F_delete:
	    gp = INT2PTR(Agraph_t *, args[0].integer);
	    objp = INT2PTR(Agobj_t *, args[1].integer);
	    if (!objp) {
		error(ERROR_WARNING, "NULL object passed to delete()");
		v.integer = 1;
	    } else if (objp == (Agobj_t *) (state->curgraph)) {
		error(ERROR_WARNING, "cannot delete current graph $G");
		v.integer = 1;
	    } else if (objp == (Agobj_t *) (state->target)) {
		error(ERROR_WARNING, "cannot delete target graph $T");
		v.integer = 1;
	    } else
		v.integer = deleteObj(gp, objp);
	    break;
	case F_lock:
	    gp = INT2PTR(Agraph_t *, args[0].integer);
	    if (!gp) {
		error(ERROR_WARNING, "NULL graph passed to lock()");
		v.integer = -1;
	    } else
		v.integer = lockGraph(gp, args[1].integer);
	    break;
	case F_nnodes:
	    gp = INT2PTR(Agraph_t *, args[0].integer);
	    if (!gp) {
		error(ERROR_WARNING, "NULL graph passed to nNodes()");
		v.integer = 0;
	    } else {
		v.integer = agnnodes(gp);
	    }
	    break;
	case F_nedges:
	    gp = INT2PTR(Agraph_t *, args[0].integer);
	    if (!gp) {
		error(ERROR_WARNING, "NULL graph passed to nEdges()");
		v.integer = 0;
	    } else {
		v.integer = agnedges(gp);
	    }
	    break;
	case F_sqrt:
	    v.floating = sqrt(args[0].floating);
	    break;
	case F_cos:
	    v.floating = cos(args[0].floating);
	    break;
	case F_sin:
	    v.floating = sin(args[0].floating);
	    break;
	case F_atan2:
	    v.floating = atan2(args[0].floating, args[1].floating);
	    break;
	case F_exp:
	    v.floating = exp(args[0].floating);
	    break;
	case F_pow:
	    v.floating = pow(args[0].floating, args[1].floating);
	    break;
	case F_log:
	    v.floating = log(args[0].floating);
	    break;
	case F_get:
	    objp = INT2PTR(Agobj_t *, args[0].integer);
	    if (!objp) {
		error(ERROR_WARNING, "NULL object passed to aget()");
		v.string = exstring(pgm, "");
	    } else {
		char* name = args[1].string;
		if (name) v.string = agget(objp, name);
		else {
		    error(ERROR_WARNING, "NULL name passed to aget()");
		    v.string = exstring(pgm, "");
		}
            }
	    break;
	case F_set:
	    objp = INT2PTR(Agobj_t *, args[0].integer);
	    if (!objp) {
		error(ERROR_WARNING, "NULL object passed to aset()");
		v.integer = 1;
	    } else {
		char* name = args[1].string;
		char* value = args[2].string;
		if (!name) {
		    error(ERROR_WARNING, "NULL name passed to aset()");
		    v.integer = 1;
		}
		else if (!value) {
		    error(ERROR_WARNING, "NULL value passed to aset()");
		    v.integer = 1;
		}
		else {
		    v.integer = setattr(objp, name, value);
        	}
            }
	    break;
	case F_canon:
	    v.string = canon(pgm, args[0].string);
	    break;
	case F_xof:
	    v.string = xyOf(pgm, args[0].string, 1);
	    break;
	case F_yof:
	    v.string = xyOf(pgm, args[0].string, 0);
	    break;
	case F_llof:
	    v.string = bbOf(pgm, args[0].string, 1);
	    break;
	case F_urof:
	    v.string = bbOf(pgm, args[0].string, 0);
	    break;
	case F_length:
	    v.integer = strlen(args[0].string);
	    break;
	case F_index:
	    v.integer = indexOf(args[0].string, args[1].string);
	    break;
	case F_match:
	    v.integer = match(args[0].string, args[1].string);
	    break;
	default:
	    error(ERROR_FATAL, "unknown function call: %s", sym->name);
	}
	return v;
    } else if (elt == EX_ARRAY) {
	args = (Extype_t *) env;
	state = (Gpr_t *) (disc->user);
	switch (sym->index) {
	case A_ARGV:
	    v.string = getArg(args[0].integer, state);
	    break;
	default:
	    error(ERROR_FATAL, "unknown array name: %s", sym->name);
	}
	return v;
    }

    state = (Gpr_t *) env;
    if (ref) {
	objp = deref(pgm, node, ref, 0, state);
	if (!objp)
	    error(ERROR_FATAL, "null reference in expression %s",
		  deparse(pgm, node, state->tmp));
    } else if ((sym->lex == ID) && (sym->index <= LAST_V)) {
	switch (sym->index) {
	case V_this:
	    v.integer = PTR2INT(state->curobj);
	    break;
	case V_thisg:
	    v.integer = PTR2INT(state->curgraph);
	    break;
	case V_targt:
	    v.integer = PTR2INT(state->target);
	    break;
	case V_outgraph:
	    v.integer = PTR2INT(state->outgraph);
	    break;
	case V_tgtname:
	    v.string = state->tgtname;
	    break;
	case V_infname:
	    v.string = state->infname;
	    break;
	case V_ARGC:
	    v.integer = state->argc;
	    break;
	case V_travtype:
	    v.integer = state->tvt;
	    break;
	case V_travroot:
	    v.integer = PTR2INT(state->tvroot);
	    break;
	}
	return v;
    } else {
	objp = state->curobj;
	if (!objp)
	    error(ERROR_FATAL,
		  "current object $ not defined as reference for %s",
		  deparse(pgm, node, state->tmp));
    }

    if (lookup(pgm, objp, sym, &v))
	error(ERROR_FATAL, "in expression %s",
	      deparse(pgm, node, state->tmp));

    return v;
}

#define MINTYPE (LAST_M+1)	/* First type occurs after last M_ */

static char *typeName(Expr_t * pg, int op)
{
    return typenames[op - MINTYPE];
}

/* setval:
 * Set sym to value v.
 * Return -1 if not allowed.
 * Assume already type correct.
 */
int
setval(Expr_t * pgm, Exnode_t * x, Exid_t * sym, Exref_t * ref,
       void *env, int elt, Extype_t v, Exdisc_t * disc)
{
    Gpr_t *state;
    Agobj_t *objp;
    Agnode_t *np;
    int iv;
    int rv = 0;

    state = (Gpr_t *) env;
    if (ref) {
	objp = deref(pgm, x, ref, 0, state);
	if (!objp) {
	    error(ERROR_FATAL, "in expression %s.%s",
		  ref->symbol->name, deparse(pgm, x, state->tmp));
	    return -1;
	}
    } else if ((MINNAME <= sym->index) && (sym->index <= MAXNAME)) {
	switch (sym->index) {
	case V_outgraph:
	    state->outgraph = INT2PTR(Agraph_t *, v.integer);
	    break;
	case V_travtype:
	    iv = v.integer;
	    if (validTVT(v.integer))
		state->tvt = (trav_type) iv;
	    else
		error(1, "unexpected value %d assigned to %s : ignored",
		      iv, typeName(pgm, V_travtype));
	    break;
	case V_travroot:
	    np = INT2PTR(Agnode_t *, v.integer);
	    if (!np || (agroot(agraphof(np)) == state->curgraph))
		state->tvroot = np;
	    else {
		error(1, "cannot set $tvroot, node %s not in $G : ignored",
		      agnameof(np));
	    }
	    break;
	case V_tgtname:
	    if (!streq(state->tgtname, v.string)) {
		vmfree(pgm->vm, state->tgtname);
		state->tgtname = vmstrdup(pgm->vm, v.string);
		state->name_used = 0;
	    }
	    break;
	default:
	    rv = -1;
	    break;
	}
	return rv;
    } else {
	objp = state->curobj;
	if (!objp)
	    error(ERROR_FATAL,
		  "current object $ undefined in expression %s",
		  deparse(pgm, x, state->tmp));
    }

    
    return setattr(objp, sym->name, v.string);
}

static int codePhase;

#define haveGraph    ((1 <= codePhase) && (codePhase <= 4))
#define haveTarget   ((2 <= codePhase) && (codePhase <= 4))
#define inWalk       ((2 <= codePhase) && (codePhase <= 3))

/* typeChk:
 * Type check input type against implied type of symbol sym.
 * If okay, return result type; else return 0.
 * For functions, input type set must intersect with function domain.
 * This means type errors may occur, but these will be caught at runtime.
 * For non-functions, input type must be 0.
 */
static tctype typeChk(tctype intype, Exid_t * sym)
{
    tctype dom = 0, rng = 0;

    switch (sym->lex) {
    case DYNAMIC:
	dom = 0;
	switch (sym->type) {
	case T_obj:
	    rng = YALL;;
	    break;
	case T_node:
	    rng = Y(V);
	    break;
	case T_graph:
	    rng = Y(G);
	    break;
	case T_edge:
	    rng = Y(E);
	    break;
	case INTEGER:
	    rng = Y(I);
	    break;
	case FLOATING:
	    rng = Y(F);
	    break;
	case STRING:
	    rng = Y(S);
	    break;
	default:
	    exerror("unknown dynamic type %d of symbol %s", sym->type,
		    sym->name);
	    break;
	}
	break;
    case ID:
	if (sym->index <= MAXNAME) {
	    switch (sym->index) {
	    case V_travroot:
	    case V_this:
	    case V_thisg:
		if (!haveGraph)
		    exerror
			("keyword %s cannot be used in BEGIN/END statements",
			 sym->name);
		break;
	    case V_targt:
		if (!haveTarget)
		    exerror
			("keyword %s cannot be used in BEGIN/BEG_G/END statements",
			 sym->name);
		break;
	    }
	    dom = tchk[sym->index][0];
	    rng = tchk[sym->index][1];
	} else {
	    dom = YALL;
	    rng = Y(S);
	}
	break;
    case NAME:
	if (!intype && !haveGraph)
	    exerror
		("undeclared, unmodified names like \"%s\" cannot be\nused in BEGIN and END statements",
		 sym->name);
	dom = YALL;
	rng = Y(S);
	break;
    default:
	exerror("unexpected symbol in typeChk: name %s, lex %d",
		sym->name, sym->lex);
	break;
    }

    if (dom) {
	if (!intype)
	    intype = YALL;	/* type of $ */
	if (!(dom & intype))
	    rng = 0;
    } else if (intype)
	rng = 0;
    return rng;
}

/* typeChkExp:
 * Type check variable expression.
 */
static tctype typeChkExp(Exref_t * ref, Exid_t * sym)
{
    tctype ty;

    if (ref) {
	ty = typeChk(0, ref->symbol);
	for (ref = ref->next; ty && ref; ref = ref->next)
	    ty = typeChk(ty, ref->symbol);
	if (!ty)
	    return 0;
    } else
	ty = 0;
    return typeChk(ty, sym);
}

/* refval:
 * Called during compilation for uses of references:   abc.x
 * Also for abc.f(..),  type abc.v, "abc".x and CONSTANTS.
 * The grammar has been  altered to disallow the first 3.
 * Type check expressions; return value unused.
 */
Extype_t
refval(Expr_t * pgm, Exnode_t * node, Exid_t * sym, Exref_t * ref,
       char *str, int elt, Exdisc_t * disc)
{
    Extype_t v;
    if (sym->lex == CONSTANT) {
	switch (sym->index) {
	case C_dfs:
	    v.integer = TV_dfs;
	    break;
	case C_bfs:
	    v.integer = TV_bfs;
	    break;
	case C_flat:
	    v.integer = TV_flat;
	    break;
	case C_fwd:
	    v.integer = TV_fwd;
	    break;
	case C_rev:
	    v.integer = TV_rev;
	    break;
	case C_ne:
	    v.integer = TV_ne;
	    break;
	case C_en:
	    v.integer = TV_en;
	    break;
	case C_null:
	    v.integer = 0;
	    break;
	default:
	    v = exzero(node->type);
	    break;
	}
    } else {
	if (!typeChkExp(ref, sym))
	    exerror("type error using %s",
		    deparse(pgm, node, sfstropen()));
	v = exzero(node->type);
    }
    return v;
}

#ifdef OLD
static void cvtError(Exid_t * xref, char *msg)
{
    if (xref)
	error(1, "%s: %s", xref->name, msg);
    else
	error(1, "%s", msg);
}
#endif

/* binary:
 * Evaluate (l ex->op r) producing a value of type ex->type,
 * stored in l.
 * May be unary, with r = NULL
 * Return -1 if operation cannot be done, 0 otherwise.
 * If arg is > 0, operation unnecessary; just report possibility.
 */
int
binary(Expr_t * pg, Exnode_t * l, Exnode_t * ex, Exnode_t * r, int arg,
       Exdisc_t * disc)
{
    Agobj_t *lobjp;
    Agobj_t *robjp;
    int ret = -1;

    if (BUILTIN(l->type))
	return -1;
    if (r && BUILTIN(r->type))
	return -1;
    if (!INTEGRAL(ex->type))
	return -1;

    if (l->type == T_tvtyp) {
	int li, ri;

	if (!r)
	    return -1;		/* Assume libexpr handled unary */
	if (r->type != T_tvtyp)
	    return -1;

	li = l->data.constant.value.integer;
	ri = r->data.constant.value.integer;
	switch (ex->op) {
	case EQ:
	    if (arg)
		return 0;
	    l->data.constant.value.integer = (li == ri);
	    ret = 0;
	    break;
	case NE:
	    if (arg)
		return 0;
	    l->data.constant.value.integer = (li != ri);
	    ret = 0;
	    break;
	case '<':
	    if (arg)
		return 0;
	    l->data.constant.value.integer = (li < ri);
	    ret = 0;
	    break;
	case LE:
	    if (arg)
		return 0;
	    l->data.constant.value.integer = (li <= ri);
	    ret = 0;
	    break;
	case GE:
	    if (arg)
		return 0;
	    l->data.constant.value.integer = (li >= ri);
	    ret = 0;
	    break;
	case '>':
	    if (arg)
		return 0;
	    l->data.constant.value.integer = (li > ri);
	    ret = 0;
	    break;
	}
    }

    /* l is a graph object; make sure r is also */
    if (r && (r->type == T_tvtyp))
	return -1;

    lobjp = INT2PTR(Agobj_t *, l->data.constant.value.integer);
    if (r)
	robjp = INT2PTR(Agobj_t *, r->data.constant.value.integer);
    else
	robjp = 0;
    switch (ex->op) {
    case EQ:
	if (arg)
	    return 0;
	l->data.constant.value.integer = !compare(lobjp, robjp);
	ret = 0;
	break;
    case NE:
	if (arg)
	    return 0;
	l->data.constant.value.integer = compare(lobjp, robjp);
	ret = 0;
	break;
    case '<':
	if (arg)
	    return 0;
	l->data.constant.value.integer = (compare(lobjp, robjp) < 0);
	ret = 0;
	break;
    case LE:
	if (arg)
	    return 0;
	l->data.constant.value.integer = (compare(lobjp, robjp) <= 0);
	ret = 0;
	break;
    case GE:
	if (arg)
	    return 0;
	l->data.constant.value.integer = (compare(lobjp, robjp) >= 0);
	ret = 0;
	break;
    case '>':
	if (arg)
	    return 0;
	l->data.constant.value.integer = (compare(lobjp, robjp) > 0);
	ret = 0;
	break;
    }

    return ret;
}

/* stringOf:
 * Convert value x of type string.
 * Assume x does not have a built-in type
 * Return -1 if conversion cannot be done, 0 otherwise.
 * If arg is > 0, conversion unnecessary; just report possibility.
 */
int stringOf(Expr_t * prog, register Exnode_t * x, int arg)
{
    Agobj_t *objp;

    if (arg)
	return 0;

    if (x->type == T_tvtyp) {
	switch (x->data.constant.value.integer) {
	case TV_flat:
	    x->data.constant.value.string = "TV_flat";
	    break;
	case TV_dfs:
	    x->data.constant.value.string = "TV_dfs";
	    break;
	case TV_bfs:
	    x->data.constant.value.string = "TV_bfs";
	    break;
	case TV_fwd:
	    x->data.constant.value.string = "TV_fwd";
	    break;
	case TV_rev:
	    x->data.constant.value.string = "TV_rev";
	    break;
	default:
	    exerror("Unexpected value %d for type tvtype_t",
		    x->data.constant.value.integer);
	    break;
	}
    } else {
	objp = INT2PTR(Agobj_t *, x->data.constant.value.integer);
	if (!objp)
	    exerror("cannot generate name for NULL %s",
		    typeName(prog, x->type));
	x->data.constant.value.string = nameOf(prog, objp);
    }
    x->type = STRING;
    return 0;
}

/* convert:
 * Convert value x of type x->type to type type.
 * Return -1 if conversion cannot be done, 0 otherwise.
 * If arg is > 0, conversion unnecessary; just report possibility.
 * In particular, assume x != 0 if arg == 0.
 * Use #ifdef OLD to remove graph object conversion to strings,
 * as this seemed to dangerous.
 */
int
convert(Expr_t * prog, register Exnode_t * x, int type,
	register Exid_t * xref, int arg, Exdisc_t * disc)
{
    Agobj_t *objp;
    int ret = -1;

    /* If both types are built-in, let libexpr handle */
    if (BUILTIN(type) && BUILTIN(x->type))
	return -1;
    if ((type == T_obj) && (x->type <= T_obj))
	ret = 0;		/* trivial cast from specific graph object to T_obj */
    else if ((type <= T_obj) && (x->type == INTEGER)) {
	if (x->data.constant.value.integer == 0)
	    ret = 0;		/* allow NULL pointer */
    } else if (type == INTEGER) {
	ret = 0;
    } else if (x->type == T_obj) {
	/* check dynamic type */
	if (arg) {
	    if ((type != FLOATING) && (type <= T_obj))
		ret = 0;
	} else {
	    objp = INT2PTR(Agobj_t *, x->data.constant.value.integer);
	    switch (type) {
	    case T_graph:
		if (!objp || AGTYPE(objp) == AGRAPH)
		    ret = 0;
		break;
	    case T_node:
		if (!objp || AGTYPE(objp) == AGNODE)
		    ret = 0;
		break;
	    case T_edge:
		if (!objp || ISEDGE(objp))
		    ret = 0;
		break;
#ifdef OLD
	    case STRING:
		x->data.constant.value.string = nameOf(prog, objp);
		ret = 0;
		break;
#endif
	    }
	}
    } else if (type == STRING) {
	if (x->type == T_tvtyp) {
	    ret = 0;
	    if (!arg)
		switch (x->data.constant.value.integer) {
		case TV_flat:
		    x->data.constant.value.string = "TV_flat";
		    break;
		case TV_dfs:
		    x->data.constant.value.string = "TV_dfs";
		    break;
		case TV_bfs:
		    x->data.constant.value.string = "TV_bfs";
		    break;
		case TV_fwd:
		    x->data.constant.value.string = "TV_fwd";
		    break;
		case TV_rev:
		    x->data.constant.value.string = "TV_rev";
		    break;
		default:
		    error(3, "Unexpected value %d for type tvtype_t",
			  x->data.constant.value.integer);
		    break;
		}
	}
#ifdef OLD
	else {
	    objp = INT2PTR(Agobj_t *, x->data.constant.value.integer);
	    if (objp) {
		x->data.constant.value.string = nameOf(prog, objp);
		ret = 0;
	    } else
		cvtError(xref, "Uninitialized object");
	}
#endif
    } else if ((type == T_tvtyp) && (x->type == INTEGER)) {
	if (arg)
	    ret = 0;
	else
	    switch (x->data.constant.value.integer) {
	    case TV_flat:
	    case TV_dfs:
	    case TV_bfs:
	    case TV_fwd:
	    case TV_rev:
		break;
	    default:
		error(1, "Integer value %d not legal for type tvtype_t",
		      x->data.constant.value.integer);
		break;
	    }
    }
    /* in case libexpr hands us the trivial case */
    else if (x->type == type) {
	ret = 0;
    } else if (x->type == STRING) {
	char *s;
	if (type == T_tvtyp) {
	    if (arg)
		ret = 0;
	    else {
		s = x->data.constant.value.string;
		if (!strncmp(s, "TV_", 3)) {
		    if (!strcmp(s + 3, "flat")) {
			x->data.constant.value.integer = TV_flat;
			ret = 0;
		    } else if (!strcmp(s + 3, "dfs")) {
			x->data.constant.value.integer = TV_dfs;
			ret = 0;
		    } else if (!strcmp(s + 3, "bfs")) {
			x->data.constant.value.integer = TV_bfs;
			ret = 0;
		    } else if (!strcmp(s + 3, "fwd")) {
			x->data.constant.value.integer = TV_fwd;
			ret = 0;
		    } else if (!strcmp(s + 3, "rev")) {
			x->data.constant.value.integer = TV_rev;
			ret = 0;
		    } else
			error(ERROR_FATAL,
			      "illegal string \"%s\" for type tvtype_t",
			      s);
		}
	    }
	}
    }
    if (!arg && (ret == 0))
	x->type = type;
    return ret;
}

/* keyval;
 * Calculate unique key for object.
 * We use this to unify local copies of nodes and edges.
 */
static Extype_t keyval(Expr_t * pgm, Extype_t v, int type, Exdisc_t * disc)
{
    if (type <= T_obj) {
	v.integer = AGID(INT2PTR(Agobj_t *, v.integer));
    }
    return v;
}

/* matchval:
 * Pattern match strings.
 */
static int
matchval(Expr_t * pgm, Exnode_t * xstr, const char *str, Exnode_t * xpat,
	 const char *pat, void *env, Exdisc_t * disc)
{
    return strgrpmatch(str, pat, NiL, 0,
		       STR_MAXIMAL | STR_LEFT | STR_RIGHT);
}

/* a2t:
 * Convert type indices to symbolic name.
 */
static int
 a2t[] = { 0, FLOATING, INTEGER, STRING,
    T_node, T_edge, T_graph, T_obj
};

/* initDisc:
 * Create and initialize expr discipline.
 */
static Exdisc_t *initDisc(Gpr_t * state)
{
    Exdisc_t *dp;

    dp = newof(0, Exdisc_t, 1, 0);
    if (!dp)
	error(ERROR_FATAL,
	      "could not create libexp discipline: out of memory");

    dp->version = EX_VERSION;
    dp->flags = EX_CHARSTRING | EX_FATAL | EX_UNDECLARED;
    dp->symbols = symbols;
    dp->convertf = convert;
    dp->stringof = stringOf;
    dp->binaryf = binary;
    dp->typename = typeName;
    dp->errorf = (Exerror_f) errorf;
    dp->keyf = keyval;
    dp->getf = getval;
    dp->reff = refval;
    dp->setf = setval;
    dp->matchf = matchval;
    dp->types = a2t;
    dp->user = state;

    return dp;
}

/* compile:
 * Compile given string, then extract and return
 * typed expression.
 */
static Exnode_t *compile(Expr_t * prog, char *src, char *input, int line,
			 char *lbl, char *sfx, int kind)
{
    Exnode_t *e;
    Sfio_t *sf;
    Sfio_t *prefix;

    /* create input stream */
    if (sfx) {
	sf = sfopen(0, sfx, "rs");
	if (input) {
	    prefix = sfopen(0, input, "rs");
	    sfstack(sf, prefix);
	}
    } else
	sf = sfopen(0, input, "rs");

    /*  prefixing label if necessary */
    if (lbl) {
	prefix = sfopen(0, 0, "sr+");
	sfprintf(prefix, "%s:\n", lbl);
	sfseek(prefix, 0, 0);
	sfstack(sf, prefix);
	line--;
    }

    /* prog is set to exit on errors */
    if (!src)
	src = "<command line>";
    excomp(prog, src, line, 0, sf);
    sfclose(sf);

    e = exexpr(prog, lbl, NiL, kind);

    return e;
}

/* checkGuard:
 * Check if guard is an assignment and warn.
 */
static void checkGuard(Exnode_t * gp, char *src, int line)
{
    gp = exnoncast(gp);
    if (gp && exisAssign(gp)) {
	if (src) {
	    error_info.file = src;
	    error_info.line = line;
	}
	error(ERROR_WARNING, "assignment used as bool in guard");
    }
}

/* mkStmts:
 */
static case_stmt *mkStmts(Expr_t * prog, char *src, case_info * sp,
			  int cnt, char *lbl)
{
    case_stmt *cs;
    int i;
    Sfio_t *tmps = sfstropen();

    cs = newof(0, case_stmt, cnt, 0);

    for (i = 0; i < cnt; i++) {
	if (sp->guard) {
	    sfprintf(tmps, "%s_g%d", lbl, i);
	    cs[i].guard = compile(prog, src, sp->guard, sp->gstart,
				  sfstruse(tmps), 0, INTEGER);
	    checkGuard(cs[i].guard, src, sp->gstart);
	}
	if (sp->action) {
	    sfprintf(tmps, "%s_a%d", lbl, i);
	    cs[i].action = compile(prog, src, sp->action, sp->astart,
				   sfstruse(tmps), 0, INTEGER);
	}
	sp = sp->next;
    }

    sfclose(tmps);
    return cs;
}

/* doFlags:
 * Convert command line flags to actions in END_G.
 */
static char *doFlags(int flags, Sfio_t * s)
{
    sfprintf(s, "\n");
    if (flags & SRCOUT)
	sfprintf(s, "$O = $G;\n");
    if (flags & INDUCE)
	sfprintf(s, "induce($O);\n");
    return sfstruse(s);
}

/* compileProg:
 * Convert gpr sections in libexpr program.
 */
comp_prog *compileProg(parse_prog * inp, Gpr_t * state, int flags)
{
    comp_prog *p;
    Exdisc_t *dp;
    Sfio_t *tmps = sfstropen();
    char *endg_sfx = 0;

    /* Make sure we have enough bits for types */
    assert(BITS_PER_BYTE * sizeof(tctype) >= (1 << TBITS));

    p = newof(0, comp_prog, 1, 0);
    if (!p)
	error(ERROR_FATAL,
	      "could not create compiled program: out of memory");

    if (flags) {
	endg_sfx = doFlags(flags, tmps);
	if (*endg_sfx == '\0')
	    endg_sfx = 0;
    }

    dp = initDisc(state);
    p->prog = exopen(dp);

    codePhase = 0;
    if (inp->begin_stmt) {
	p->begin_stmt = compile(p->prog, inp->source, inp->begin_stmt,
				inp->l_begin, 0, 0, VOID);
    }

    codePhase = 1;
    if (inp->begg_stmt) {
	tchk[V_this][1] = Y(G);
	p->begg_stmt = compile(p->prog, inp->source, inp->begg_stmt,
			       inp->l_beging, "_begin_g", 0, VOID);
    }

    codePhase = 2;
    if (inp->node_stmts) {
	tchk[V_this][1] = Y(V);
	p->n_nstmts = inp->n_nstmts;
	p->node_stmts = mkStmts(p->prog, inp->source, inp->node_stmts,
				inp->n_nstmts, "_nd");
    }

    codePhase = 3;
    if (inp->edge_stmts) {
	tchk[V_this][1] = Y(E);
	p->n_estmts = inp->n_estmts;
	p->edge_stmts = mkStmts(p->prog, inp->source, inp->edge_stmts,
				inp->n_estmts, "_eg");
    }

    codePhase = 4;
    if (inp->endg_stmt || endg_sfx) {
	tchk[V_this][1] = Y(G);
	p->endg_stmt = compile(p->prog, inp->source, inp->endg_stmt,
			       inp->l_endg, "_end_g", endg_sfx, VOID);
    }

    codePhase = 5;
    if (inp->end_stmt)
	p->end_stmt = compile(p->prog, inp->source, inp->end_stmt,
			      inp->l_end, "_end_", 0, VOID);
    sfclose(tmps);
    error_info.line = 0;	/* execution errors have no line numbers */
    return p;
}

/* walksGraph;
 * Returns true if program actually has node or edge statements.
 */
int walksGraph(comp_prog * p)
{
    return (p->n_nstmts || p->n_estmts);
}

/* usesGraph;
 * Returns true if program uses the graph, i.e., has
 * N/E/BEG_G/END_G statments
 */
int usesGraph(comp_prog * p)
{
    return (walksGraph(p) || p->begg_stmt || p->endg_stmt);
}

void ptchk(void)
{
    int i;
    for (i = 0; i <= LAST_M; i++)
	printf("%d: %d %d\n", i, tchk[i][0], tchk[i][1]);
}

static int iofread(void *chan, char *buf, int bufsize)
{
    return read(sffileno((Sfio_t *) chan), buf, bufsize);
}

static int ioputstr(void *chan, char *str)
{
    return sfputr((Sfio_t *) chan, str, -1);
}

static int ioflush(void *chan)
{
    return sfsync((Sfio_t *) chan);
}

static Agiodisc_t gprIoDisc = { iofread, ioputstr, ioflush };

static Agdisc_t gprDisc = { &AgMemDisc, &AgIdDisc, &gprIoDisc };

/* readG:
 * Read graph from file and initialize
 * dynamic data.
 */
Agraph_t *readG(Sfio_t * fp)
{
    Agraph_t *g;

    g = agread(fp, &gprDisc);
    if (g) {
	aginit(g, AGRAPH, UDATA, sizeof(gdata), 0);
	aginit(g, AGNODE, UDATA, sizeof(ndata), 0);
	aginit(g, AGEDGE, UDATA, sizeof(edata), 0);
    }
    return g;
}

/* openG:
 * Open graph and initialize dynamic data.
 */
Agraph_t *openG(char *name, Agdesc_t desc)
{
    Agraph_t *g;

    g = agopen(name, desc, &gprDisc);
    if (g)
	agbindrec(g, UDATA, sizeof(gdata), 0);
    return g;
}

/* openSubg:
 * Open subgraph and initialize dynamic data.
 */
Agraph_t *openSubg(Agraph_t * g, char *name)
{
    Agraph_t *sg;

    sg = agsubg(g, name, 1);
    if (sg && !aggetrec(sg, UDATA, 0))
	agbindrec(sg, UDATA, sizeof(gdata), 0);
    return sg;
}

/* openNode:
 * Create node and initialize dynamic data.
 */
Agnode_t *openNode(Agraph_t * g, char *name)
{
    Agnode_t *np;

    np = agnode(g, name, 1);
    if (np && !aggetrec(np, UDATA, 0))
	agbindrec(np, UDATA, sizeof(ndata), 0);
    return np;
}

/* openEdge:
 * Create edge and initialize dynamic data.
 * The edge is always created in the root graph.
 */
Agedge_t *openEdge(Agnode_t * t, Agnode_t * h, char *key)
{
    Agedge_t *ep;
    Agraph_t *root;

    root = sameG(t, h, "openEdge", "tail and head node");
    if (!root)
	return 0;

    t = (Agnode_t *) agrebind(root, OBJ(t));
    h = (Agnode_t *) agrebind(root, OBJ(h));
    ep = agedge(t, h, key, 1);
    if (ep && !aggetrec(ep, UDATA, 0))
	agbindrec(ep, UDATA, sizeof(edata), 0);
    return ep;
}
