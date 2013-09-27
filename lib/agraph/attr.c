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


#include "aghdr.h"

/*
 * dynamic attributes
 */

/* to create a graph's data dictionary */

#define MINATTR	4		/* minimum allocation */

static void freesym(Dict_t * d, Void_t * obj, Dtdisc_t * disc);

Dtdisc_t AgDataDictDisc = {
    offsetof(Agsym_t, name),	/* use symbol name as key */
    -1,
    offsetof(Agsym_t, link),
    NIL(Dtmake_f),
    freesym,
    NIL(Dtcompar_f),
    NIL(Dthash_f)
};

static char DataDictName[] = "_AG_datadict";
static void init_all_attrs(Agraph_t * g);

Agdatadict_t *agdatadict(Agraph_t * g)
{
    Agdatadict_t *rv;
    while ((rv = (Agdatadict_t *) aggetrec(g, DataDictName, FALSE))
	   == NIL(Agdatadict_t *))
	init_all_attrs(g);
    return rv;
}

Dict_t *agdictof(Agraph_t * g, int kind)
{
    Agdatadict_t *dd;
    Dict_t *dict;

    dd = agdatadict(g);
    switch (kind) {
    case AGRAPH:
	dict = dd->dict.g;
	break;
    case AGNODE:
	dict = dd->dict.n;
	break;
    case AGINEDGE:
    case AGOUTEDGE:
	dict = dd->dict.e;
	break;
    default:
	abort();
    }
    return dict;
}

static Agdatadict_t *agmakedatadict(Agraph_t * g)
{
    Agraph_t *par;
    Agdatadict_t *parent_dd, *dd;

    dd = (Agdatadict_t *) agbindrec(g, DataDictName, sizeof(Agdatadict_t),
				    FALSE);
    dd->dict.n = agdtopen(g, &AgDataDictDisc, Dttree);
    dd->dict.e = agdtopen(g, &AgDataDictDisc, Dttree);
    dd->dict.g = agdtopen(g, &AgDataDictDisc, Dttree);
    if ((par = agparent(g))) {
	parent_dd = agdatadict(par);
	assert(dd != parent_dd);
	dtview(dd->dict.n, parent_dd->dict.n);
	dtview(dd->dict.e, parent_dd->dict.e);
	dtview(dd->dict.g, parent_dd->dict.g);
    }
    return dd;
}

Agsym_t *agnewsym(Agraph_t * g, char *name, char *value, int id, int kind)
{
    Agsym_t *sym;
    sym = agalloc(g, sizeof(Agsym_t));
    sym->kind = kind;
    sym->name = agstrdup(g, name);
    sym->defval = agstrdup(g, value);
    sym->id = id;
    return sym;
}

	/* look up or update a value associated with an object. */
Agsym_t *agdictsym(Dict_t * dict, char *name)
{
    Agsym_t key;
    key.name = (char *) name;
    return (Agsym_t *) dtsearch(dict, &key);
}

/* look up attribute in local dictionary with no view pathing */
Agsym_t *aglocaldictsym(Dict_t * dict, char *name)
{
    Agsym_t *rv;
    Dict_t *view;

    view = dtview(dict, NIL(Dict_t *));
    rv = agdictsym(dict, name);
    dtview(dict, view);
    return rv;
}

Agsym_t *agattrsym(void *obj, char *name)
{
    Agattr_t *data;

    data = agattrrec((Agobj_t *) obj);
    return (data ? agdictsym(data->dict, name) : NILsym);
}

/* to create a graph's, node's edge's string attributes */

char *AgDataRecName = "_AG_strdata";

static int topdictsize(Agobj_t * obj)
{
    return dtsize(agdictof(agroot(agraphof(obj)), AGTYPE(obj)));
}

static Agrec_t *agmakeattrs(void *obj, int norecur)
{
    int sz;
    Agattr_t *rec;
    Agsym_t *sym;
    Agraph_t *g;
    Dict_t *datadict;

    g = agraphof(obj);
    rec =
	agrealbindrec(obj, AgDataRecName, sizeof(Agattr_t), FALSE,
		      norecur);
    datadict = agdictof(g, AGTYPE(obj));
    if (rec->dict == NIL(Dict_t *)) {
	rec->dict = datadict;
	/* don't malloc(0) */
	sz = topdictsize(obj);
	if (sz < MINATTR)
	    sz = MINATTR;
	rec->str = agalloc(g, sz * sizeof(char *));
	/* doesn't call agxset() so no obj-modified callbacks occur */
	for (sym = (Agsym_t *) dtfirst(datadict); sym;
	     sym = (Agsym_t *) dtnext(datadict, sym))
	    rec->str[sym->id] = agstrdup(g, sym->defval);
    } else {
	assert(rec->dict == datadict);
    }
    return (Agrec_t *) rec;
}


static void freeattr(Agobj_t * obj, Agattr_t * attr)
{
    int i, sz;
    Agraph_t *g;

    g = agraphof(obj);
    sz = topdictsize(obj);
    for (i = 0; i < sz; i++)
	agstrfree(g, attr->str[i]);
    agfree(g, attr->str);
}

static void freesym(Dict_t * d, Void_t * obj, Dtdisc_t * disc)
{
    Agsym_t *sym;

    NOTUSED(d);
    sym = (Agsym_t *) obj;
    NOTUSED(disc);
    agstrfree(Ag_G_global, sym->name);
    agstrfree(Ag_G_global, sym->defval);
    agfree(Ag_G_global, sym);
}

Agattr_t *agattrrec(void *obj)
{
    return (Agattr_t *) aggetrec(obj, AgDataRecName, FALSE);
}


static void addattr(Agobj_t * obj, Agsym_t * sym)
{
    Agattr_t *attr;
    Agraph_t *g;

    g = agraphof(obj);
    attr = (Agattr_t *) agattrrec(obj);
    assert(attr != NIL(Agattr_t *));
    if (sym->id >= MINATTR)
	attr->str = (char **) AGDISC(g, mem)->resize(AGCLOS(g, mem),
						     attr->str,
						     sym->id *
						     sizeof(char *),
						     (sym->id +
						      1) * sizeof(char *));
    attr->str[sym->id] = agstrdup(g, sym->defval);
    /* agmethod_upd(g,obj,sym);  JCE and GN didn't like this. */
}


/*
 * create or update an existing attribute and return its descriptor.
 * if the new value is NIL(char*), this is only a search, no update.
 * when a new attribute is created, existing graphs/nodes/edges
 * receive its default value.
 */
Agsym_t *agattr(Agraph_t * g, int kind, char *name, char *value)
{
    Agraph_t *root;
    Agsym_t *lsym, *rsym;
    Dict_t *ldict, *rdict;
    Agnode_t *n;
    Agedge_t *e;

    ldict = agdictof(g, kind);
    lsym = aglocaldictsym(ldict, name);
    root = agroot(g);
    if (lsym) {
	/* this attr was previously defined in this graph */
	if (value) {
	    agstrfree(g, lsym->defval);
	    lsym->defval = agstrdup(g, value);
	}
    } else {			/* not previously defined here */
	rsym = agdictsym(ldict, name);	/* viewpath up to root */
	if (value) {		/* need to define */
	    if (rsym) {
		lsym = agnewsym(g, name, value, rsym->id, kind);
		dtinsert(ldict, lsym);
	    } else {		/* just define globally */
		rdict = agdictof(root, kind);
		lsym = rsym =
		    agnewsym(g, name, value, dtsize(rdict), kind);
		dtinsert(rdict, rsym);

		switch (kind) {
		case AGRAPH:
		    agapply(root, (Agobj_t *) root, (agobjfn_t) addattr,
			    lsym, TRUE);
		    break;
		case AGNODE:
		    for (n = agfstnode(root); n; n = agnxtnode(n))
			addattr((Agobj_t *) n, lsym);
		    break;
		case AGINEDGE:
		case AGOUTEDGE:
		    for (n = agfstnode(root); n; n = agnxtnode(n))
			for (e = agfstout(n); e; e = agnxtout(e))
			    addattr((Agobj_t *) e, lsym);
		    break;
		}
	    }
	    agmethod_upd(g, g, lsym);	/* JCE and GN wanted this instead */
	} else
	    return rsym;
    }
    /* in graphs, defaults and actual values are always the same.
     * see also agxset() where graph attrs are handled.  therefore
     * agxset() must not call agattr() in its implementation 
     */
    if (lsym && value && (kind == AGRAPH))
	agxset(g, lsym, value);
    return lsym;
}

Agsym_t *agnxtattr(Agraph_t * g, int kind, Agsym_t * attr)
{
    Dict_t *d;
    Agsym_t *rv;

    d = agdictof(g, kind);
    if (attr)
	rv = (Agsym_t *) dtnext(d, attr);
    else
	rv = (Agsym_t *) dtfirst(d);
    return rv;
}

/* Create or delete attributes associated with an object */

void agraphattr_init(Agraph_t * g, int norecur)
{
    /* Agdatadict_t *dd; */
    /* Agrec_t                      *attr; */

    g->desc.has_attrs = 1;
    /* dd = */ agmakedatadict(g);
    /* attr = */ agmakeattrs(g, norecur);
}

void agraphattr_delete(Agraph_t * g)
{
    Agdatadict_t *dd;
    Agattr_t *attr;

    Ag_G_global = g;
    if ((attr = agattrrec(g))) {
	freeattr((Agobj_t *) g, attr);
	agdelrec(g, attr->h.name);
    }

    if ((dd = agdatadict(g))) {
	agdtclose(g, dd->dict.n);
	agdtclose(g, dd->dict.e);
	agdtclose(g, dd->dict.g);
	agdelrec(g, dd->h.name);
    }
}

void agnodeattr_init(Agnode_t * n, int norecur)
{
    Agattr_t *data;

    data = agattrrec(n);
    if ((!data) || (!data->dict))
	(void) agmakeattrs(n, norecur);
}

void agnodeattr_delete(Agnode_t * n)
{
    Agattr_t *rec;

    if ((rec = agattrrec(n))) {
	freeattr((Agobj_t *) n, rec);
	agdelrec(n, AgDataRecName);
    }
}

void agedgeattr_init(Agedge_t * e, int norecur)
{
    Agattr_t *data;

    data = agattrrec(e);
    if ((!data) || (!data->dict))
	(void) agmakeattrs(e, norecur);
}

void agedgeattr_delete(Agedge_t * e)
{
    Agattr_t *rec;

    if ((rec = agattrrec(e))) {
	freeattr((Agobj_t *) e, rec);
	agdelrec(e, AgDataRecName);
    }
}

char *agget(void *obj, char *name)
{
    Agsym_t *sym;
    Agattr_t *data;
    char *rv;

    sym = agattrsym(obj, name);
    if (sym == NILsym)
	rv = "";
    else {
	data = agattrrec((Agobj_t *) obj);
	rv = (char *) (data->str[sym->id]);
    }
    return rv;
}

char *agxget(void *obj, Agsym_t * sym)
{
    Agattr_t *data;
    char *rv;

    data = agattrrec((Agobj_t *) obj);
    assert((sym->id >= 0) && (sym->id < topdictsize(obj)));
    rv = (char *) (data->str[sym->id]);
    return rv;
}

int agset(void *obj, char *name, char *value)
{
    Agsym_t *sym;
    int rv;

    sym = agattrsym(obj, name);
    if (sym == NILsym)
	rv = FAILURE;
    else
	rv = agxset(obj, sym, value);
    return rv;
}

int agxset(void *obj, Agsym_t * sym, char *value)
{
    Agraph_t *g;
    Agobj_t *hdr;
    Agattr_t *data;
    Agsym_t *lsym;

    g = agraphof(obj);
    hdr = (Agobj_t *) obj;
    data = agattrrec(hdr);
    assert((sym->id >= 0) && (sym->id < topdictsize(obj)));
    agstrfree(g, data->str[sym->id]);
    data->str[sym->id] = agstrdup(g, value);
    if (hdr->tag.objtype == AGRAPH) {
	/* also update dict default */
	Dict_t *dict;
	dict = agdatadict(g)->dict.g;
	if ((lsym = aglocaldictsym(dict, sym->name))) {
	    agstrfree(g, lsym->defval);
	    lsym->defval = agstrdup(g, value);
	} else {
	    lsym = agnewsym(g, sym->name, value, sym->id, AGTYPE(hdr));
	    dtinsert(dict, lsym);
	}
    }
    agmethod_upd(g, obj, sym);
    return SUCCESS;
}

/*
 * here we are attaching attributes to the already created graph objs.
 * presumably they were already initialized, so we don't invoke any
 * of the old methods.
 */
static void init_all_attrs(Agraph_t * g)
{
    Agraph_t *root;
    Agnode_t *n;
    Agedge_t *e;

    root = agroot(g);
    agapply(root, (Agobj_t *) root, (agobjfn_t) agraphattr_init,
	    NIL(Agdisc_t *), TRUE);
    for (n = agfstnode(root); n; n = agnxtnode(n)) {
	agnodeattr_init(n, FALSE);
	for (e = agfstout(n); e; e = agnxtout(e)) {
	    agedgeattr_init(e, FALSE);
	}
    }
}
