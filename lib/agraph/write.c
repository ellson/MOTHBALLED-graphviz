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


#ifdef WIN32
#include <compat.h>
#endif
#include <stdio.h>		/* need sprintf() */
#include <ctype.h>
#include "aghdr.h"

#define EMPTY(s)		((s == 0) || (s)[0] == '\0')
#define MAX(a,b)     ((a)>(b)?(a):(b))

typedef void iochan_t;

static void ioput(Agraph_t * g, iochan_t * ofile, char *str)
{
    AGDISC(g, io)->putstr(ofile, str);
}

static void write_body(Agraph_t * g, iochan_t * ofile);
static int Level;
static Agsym_t *Tailport, *Headport;

static void indent(Agraph_t * g, iochan_t * ofile)
{
    int i;
    for (i = Level; i > 0; i--)
	ioput(g, ofile, "\t");
}

#ifndef HAVE_STRCASECMP

#include <string.h>

int strcasecmp(const char *s1, const char *s2)
{
    while ((*s1 != '\0')
	   && (tolower(*(unsigned char *) s1) ==
	       tolower(*(unsigned char *) s2))) {
	s1++;
	s2++;
    }

    return tolower(*(unsigned char *) s1) - tolower(*(unsigned char *) s2);
}

#endif				/* HAVE_STRCASECMP */

/* _agcanonstr:
 * Canonicalize ordinary strings. 
 * Assumes buf is large enough to hold output.
 */
static char*
_agcanonstr(char *arg, char *buf)
{
    char *s, *p;
    unsigned char uc;
    int cnt = 0;
    int needs_quotes = FALSE;
    int maybe_num;
    static const char *tokenlist[]	/* must agree with scan.l */
    = { "node", "edge", "strict", "graph", "digraph", "subgraph",
	    NIL(char *) };
    const char **tok;

    if (EMPTY(arg))
	return "\"\"";
    s = arg;
    p = buf;
    *p++ = '\"';
    uc = *(unsigned char *) s++;
    maybe_num = (isdigit(uc) || (uc == '.'));
    while (uc) {
	if (uc == '\"') {
	    *p++ = '\\';
	    needs_quotes = TRUE;
	} else {
	    if (!ISALNUM(uc))
		needs_quotes = TRUE;
	    else if (maybe_num && (!isdigit(uc) && (uc != '.')))
		needs_quotes = TRUE;
	}
	*p++ = (char) uc;
	uc = *(unsigned char *) s++;
	cnt++;
	if (cnt >= MAX_OUTPUTLINE) {
	    *p++ = '\\';
	    *p++ = '\n';
	    needs_quotes = TRUE;
	    cnt = 0;
	}
    }
    *p++ = '\"';
    *p = '\0';
    if (needs_quotes)
	return buf;

    /* Use quotes to protect tokens (example, a node named "node") */
    /* It would be great if it were easier to use flex here. */
    for (tok = tokenlist; *tok; tok++)
	if (!strcasecmp(*tok, arg))
	    return buf;
    return arg;
}

/* agcanonhtmlstr:
 * Canonicalize html strings. 
 */
static char *agcanonhtmlstr(char *arg, char *buf)
{
    char *s, *p;

    s = arg;
    p = buf;
    *p++ = '<';
    while (*s)
	*p++ = *s++;
    *p++ = '>';
    *p = '\0';
    return buf;
}

/*
 * canonicalize a string for printing.
 * must agree with strings in scan.l
 * Unsafe if buffer is not large enough.
 */
char*
agcanonstr(char *arg, char *buf)
{
    if (aghtmlstr(arg))
	return agcanonhtmlstr(arg, buf);
    else
	return _agcanonstr(arg, buf);
}

static char *getoutputbuffer(char *str)
{
    static char *rv;
    static int len;
    int req;

    req = MAX(2 * strlen(str) + 2, BUFSIZ);
    if (req > len) {
	if (rv)
	    rv = realloc(rv, req);
	else
	    rv = malloc(req);
	len = req;
    }
    return rv;
}

/*
 * canonicalize a string for printing.
 * must agree with strings in scan.l
 */
char*
agcanonStr(char *str)
{
    return agcanonstr(str, getoutputbuffer(str));
}

static void _write_canonstr(Agraph_t * g, iochan_t * ofile, char *str, int chk)
{
    if (chk)
	str = agcanonStr(str);
    else
	str = _agcanonstr(str, getoutputbuffer(str));
    ioput(g, ofile, str);
}

static void write_canonstr(Agraph_t * g, iochan_t * ofile, char *str)
{
    _write_canonstr(g, ofile, str, TRUE);
}

static void write_dict(Agraph_t * g, iochan_t * ofile, char *name,
		       Dict_t * dict)
{
    int cnt = 0;
    Dict_t *view;
    Agsym_t *sym, *psym;

    view = dtview(dict, NIL(Dict_t *));
    for (sym = (Agsym_t *) dtfirst(dict); sym;
	 sym = (Agsym_t *) dtnext(dict, sym)) {
	if (EMPTY(sym->defval)) {	/* try to skip empty str (default) */
	    if (view == NIL(Dict_t *))
		continue;	/* no parent */
	    psym = (Agsym_t *) dtsearch(view, sym);
	    assert(psym);
	    if (EMPTY(psym->defval))
		continue;	/* also empty in parent */
	}
	if (cnt++ == 0) {
	    indent(g, ofile);
	    ioput(g, ofile, name);
	    ioput(g, ofile, " [");
	    Level++;
	} else {
	    ioput(g, ofile, ",\n");
	    indent(g, ofile);
	}
	write_canonstr(g, ofile, sym->name);
	ioput(g, ofile, "=");
	write_canonstr(g, ofile, sym->defval);
    }
    if (cnt > 0) {
	Level--;
	if (cnt > 1) {
	    ioput(g, ofile, "\n");
	    indent(g, ofile);
	}
	ioput(g, ofile, "];\n");
    }
    dtview(dict, view);		/* restore previous view */
}

static void write_dicts(Agraph_t * g, iochan_t * ofile)
{
    Agdatadict_t *def;
    if ((def = agdatadict(g))) {
	write_dict(g, ofile, "graph", def->dict.g);
	write_dict(g, ofile, "node", def->dict.n);
	write_dict(g, ofile, "edge", def->dict.e);
    }
}

static void write_hdr(Agraph_t * g, iochan_t * ofile, int top)
{
    char *name, *sep, *kind, *strict;
    int root = 0;

    strict = "";
    if (NOT(top) && agparent(g))
	kind = "sub";
    else {
	root = 1;
	if (g->desc.directed)
	    kind = "di";
	else
	    kind = "";
	if (agisstrict(g))
	    strict = "strict ";
	Tailport = agattr(g, AGEDGE, TAIL_ID, NIL(char *));
	Headport = agattr(g, AGEDGE, HEAD_ID, NIL(char *));
    }
    name = agnameof(g);
    sep = " ";
    if (!name || name[0] == LOCALNAMEPREFIX)
	sep = name = "";
    indent(g, ofile);
    ioput(g, ofile, strict);

    /* output "<kind>graph" only for root graphs or graphs with names */
    if (*name || root) {
	ioput(g, ofile, kind);
	ioput(g, ofile, "graph ");
    }
    if (name[0])
	write_canonstr(g, ofile, name);
    ioput(g, ofile, sep);
    ioput(g, ofile, "{\n");
    Level++;
    write_dicts(g, ofile);
	AGATTRWF(g) = TRUE;
}

static void write_trl(Agraph_t * g, iochan_t * ofile)
{
    NOTUSED(g);
    Level--;
    indent(g, ofile);
    ioput(g, ofile, "}\n");
}

static int irrelevant_subgraph(Agraph_t * g)
{
    int i, n;
    Agattr_t *sdata, *pdata, *rdata;
    Agdatadict_t *dd;

    char *name;

    name = agnameof(g);
    if (name && name[0] != LOCALNAMEPREFIX)
	return FALSE;
    if ((sdata = agattrrec(g)) && (pdata = agattrrec(agparent(g)))) {
	rdata = agattrrec(agroot(g));
	n = dtsize(rdata->dict);
	for (i = 0; i < n; i++)
	    if (sdata->str[i] && pdata->str[i]
		&& strcmp(sdata->str[i], pdata->str[i]))
		return FALSE;
    }
    dd = agdatadict(g);
    if ((dtsize(dd->dict.n) > 0) || (dtsize(dd->dict.e) > 0))
	return FALSE;
    return TRUE;
}

int node_in_subg(Agraph_t * g, Agnode_t * n)
{
    Agraph_t *subg;

    for (subg = agfstsubg(g); subg; subg = agnxtsubg(subg)) {
	if (irrelevant_subgraph(subg))
	    continue;
	if (agsubnode(subg, n, FALSE))
	    return TRUE;
    }
    return FALSE;
}

static int has_no_edges(Agnode_t * n)
{
    return ((n->in == NIL(Agedge_t *)) && (n->out == NIL(Agedge_t *)));
}

static int has_no_predecessor_below(Agnode_t * n, unsigned long val)
{
    Agedge_t *e;

    if (AGSEQ(n) < val)
	return FALSE;
    for (e = agfstin(n); e; e = agnxtin(e))
	if (AGSEQ(e->node) < val)
	    return FALSE;
    return TRUE;
}

static int not_default_attrs(Agraph_t * g, Agnode_t * n)
{
    Agattr_t *data;
    Agsym_t *sym;

    NOTUSED(g);
    if ((data = agattrrec(n))) {
	for (sym = (Agsym_t *) dtfirst(data->dict); sym;
	     sym = (Agsym_t *) dtnext(data->dict, sym)) {
	    if (data->str[sym->id] != sym->defval)
		return TRUE;
	}
    }
    return FALSE;
}

static void write_subgs(Agraph_t * g, iochan_t * ofile)
{
    Agraph_t *subg;

    for (subg = agfstsubg(g); subg; subg = agnxtsubg(subg)) {
	if (irrelevant_subgraph(subg))
	    continue;
	write_hdr(subg, ofile, FALSE);
	write_body(subg, ofile);
	write_trl(subg, ofile);
    }
}

static int write_edge_name(Agedge_t * e, iochan_t * ofile, int terminate)
{
    int rv;
    char *p;
    Agraph_t *g;

    p = agnameof(e);
    g = agraphof(e);
    if (NOT(EMPTY(p))) {
	ioput(g, ofile, " [key=");
	write_canonstr(g, ofile, p);
	if (terminate)
	    ioput(g, ofile, "]");
	rv = TRUE;
    } else
	rv = FALSE;
    return rv;
}


static void write_nondefault_attrs(void *obj, iochan_t * ofile,
				   Dict_t * defdict)
{
    Agattr_t *data;
    Agsym_t *sym;
    Agraph_t *g;
    int cnt = 0;

    if ((AGTYPE(obj) == AGINEDGE) || (AGTYPE(obj) == AGOUTEDGE)) {
	if (write_edge_name(obj, ofile, FALSE))
	    cnt++;
    }
    data = agattrrec(obj);
    g = agraphof(obj);
    if (data)
	for (sym = (Agsym_t *) dtfirst(defdict); sym;
	     sym = (Agsym_t *) dtnext(defdict, sym)) {
	    if ((AGTYPE(obj) == AGINEDGE) || (AGTYPE(obj) == AGOUTEDGE)) {
		if (Tailport && (sym->id == Tailport->id))
		    continue;
		if (Headport && (sym->id == Headport->id))
		    continue;
	    }
	    if (data->str[sym->id] != sym->defval) {
		if (cnt++ == 0) {
		    indent(g, ofile);
		    ioput(g, ofile, " [");
		    Level++;
		} else {
		    ioput(g, ofile, ",\n");
		    indent(g, ofile);
		}
		write_canonstr(g, ofile, sym->name);
		ioput(g, ofile, "=");
		write_canonstr(g, ofile, data->str[sym->id]);
	    }
	}
    if (cnt > 0) {
	ioput(g, ofile, "]");
	Level--;
    }
    AGATTRWF((Agobj_t *) obj) = TRUE;
}

static void write_nodename(Agnode_t * n, iochan_t * ofile)
{
    char *name, buf[20];
    Agraph_t *g;

    name = agnameof(n);
    g = agraphof(n);
    if (name)
	write_canonstr(g, ofile, name);
    else {
	sprintf(buf, "_%ld_SUSPECT", AGID(n));	/* could be deadly wrong */
	ioput(g, ofile, buf);
    }
}

static int attrs_written(void *obj)
{
    return (AGATTRWF((Agobj_t *) obj));
}

static void write_node(Agnode_t * n, iochan_t * ofile, Dict_t * d)
{
    Agraph_t *g;

    g = agraphof(n);
    indent(g, ofile);
    write_nodename(n, ofile);
    if (NOT(attrs_written(n)))
	write_nondefault_attrs(n, ofile, d);
    ioput(g, ofile, ";\n");
}

/* node must be written if it wasn't already emitted because of
 * a subgraph or one of its predecessors, and if it is a singleton
 * or has non-default attributes.
 */
static int write_node_test(Agraph_t * g, Agnode_t * n,
			   unsigned long pred_id)
{
    if (NOT(node_in_subg(g, n)) && has_no_predecessor_below(n, pred_id)) {
	if (has_no_edges(n) || not_default_attrs(g, n))
	    return TRUE;
    }
    return FALSE;
}

static void
write_port(Agedge_t * e, iochan_t * ofile, Agsym_t * port)
{
    char *val;
    Agraph_t *g;

    if (!port)
	return;
    g = agraphof(e);
    val = agxget(e, port);
    if (val[0] == '\0')
	return;

    ioput(g, ofile, ":");
    if (aghtmlstr(val))
	write_canonstr(g, ofile, val);
    else {
	char* s = strchr (val, ':');
	if (s) {
	    *s = '\0';
	    _write_canonstr(g, ofile, val, FALSE);
	    ioput(g, ofile, ":");
	    _write_canonstr(g, ofile, s+1, FALSE);
	    *s = ':';
	}
	else {
	    _write_canonstr(g, ofile, val, FALSE);
	}
    }
}

static int write_edge_test(Agraph_t * g, Agedge_t * e)
{
    Agraph_t *subg;

    /* can use agedge() because we subverted the dict compar_f */
    for (subg = agfstsubg(g); subg; subg = agnxtsubg(subg)) {
	if (irrelevant_subgraph(subg))
	    continue;
	if (agsubedge(subg, e, FALSE))
	    return FALSE;
    }
    return TRUE;
}

static void write_edge(Agedge_t * e, iochan_t * ofile, Dict_t * d)
{
    Agnode_t *t, *h;
    Agraph_t *g;

    t = AGTAIL(e);
    h = AGHEAD(e);
    g = agraphof(t);
    indent(g, ofile);
    write_nodename(t, ofile);
    write_port(e, ofile, Tailport);
    ioput(g, ofile, (agisdirected(agraphof(t)) ? " -> " : " -- "));
    write_nodename(h, ofile);
    write_port(e, ofile, Headport);
    if (NOT(attrs_written(e)))
	write_nondefault_attrs(e, ofile, d);
    else
	write_edge_name(e, ofile, TRUE);
    ioput(g, ofile, ";\n");
}

static void write_body(Agraph_t * g, iochan_t * ofile)
{
    Agnode_t *n;
    Agedge_t *e;
    Agdatadict_t *dd;
    /* int                  has_attr; */

    /* has_attr = (agattrrec(g) != NIL(Agattr_t*)); */
    write_subgs(g, ofile);
    dd = agdatadict(g);
    for (n = agfstnode(g); n; n = agnxtnode(n)) {
	if (write_node_test(g, n, AGSEQ(n)))
	    write_node(n, ofile, dd->dict.n);
	for (e = agfstout(n); e; e = agnxtout(e)) {
	    if (write_node_test(g, e->node, AGSEQ(n)))
		write_node(e->node, ofile, dd->dict.n);
	    if (write_edge_test(g, e))
		write_edge(e, ofile, dd->dict.e);
	}
    }
}

static void set_attrwf(Agraph_t *g, int toplevel, int value)
{
    Agraph_t *subg;
	Agnode_t *n;
	Agedge_t *e;

    AGATTRWF(g) = value;
    for (subg = agfstsubg(g); subg; subg = agnxtsubg(subg)) {
	set_attrwf(subg, FALSE, value);
    }
    if (toplevel) {
	for (n = agfstnode(g); n; n = agnxtnode(n)) {
	AGATTRWF(n) = value;
	for (e = agfstout(n); e; e = agnxtout(e))
	    AGATTRWF(e) = value;
	}
    }
}

int agwrite(Agraph_t * g, void *ofile)
{
    set_attrwf(g,TRUE,FALSE);
    write_hdr(g, ofile, TRUE);
    write_body(g, ofile);
    write_trl(g, ofile);
    return AGDISC(g, io)->flush(ofile);
}
