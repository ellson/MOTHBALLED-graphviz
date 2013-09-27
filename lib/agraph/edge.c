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

#define IN_SET FALSE
#define OUT_SET TRUE
#define ID_ORDER TRUE
#define SEQ_ORDER FALSE

static Agtag_t Tag;		/* to silence warnings about initialization */

Agedge_t *agfstout(Agnode_t * n)
{
    Agraph_t *g;
    Agedge_t *e = NILedge;

    g = agraphof(n);
    if (agisflattened(g))
	e = AGFSTOUT(n);
    else {
	dtrestore(g->e_seq, &(n->out->base.seq_link));
	e = (Agedge_t *) dtfirst(g->e_seq);
	n->out = (Agedge_t *) dtextract(g->e_seq);
    }
    return e;
}

Agedge_t *agnxtout(Agedge_t * e)
{
    Agraph_t *g;
    Agnode_t *n;
    Agedge_t *f;

    g = agraphof(e);
    if (agisflattened(g))
	f = AGNXTE(e);
    else {
	n = AGTAIL(e);
	dtrestore(g->e_seq, &(n->out->base.seq_link));
	f = (Agedge_t *) dtnext(g->e_seq, e);
	n->out = (Agedge_t *) dtextract(g->e_seq);
    }
    return f;
}

Agedge_t *agfstin(Agnode_t * n)
{
    Agraph_t *g;
    Agedge_t *e = NILedge;

    g = agraphof(n);
    if (agisflattened(g))
	e = AGFSTIN(n);
    else {
	dtrestore(g->e_seq, &(n->in->base.seq_link));
	e = (Agedge_t *) dtfirst(g->e_seq);
	n->in = (Agedge_t *) dtextract(g->e_seq);
    }
    return e;
}

Agedge_t *agnxtin(Agedge_t * e)
{
    Agraph_t *g;
    Agnode_t *n;
    Agedge_t *f;

    g = agraphof(e);
    if (agisflattened(g))
	f = AGNXTE(e);
    else {
	n = AGHEAD(e);
	dtrestore(g->e_seq, &(n->in->base.seq_link));
	f = (Agedge_t *) dtnext(g->e_seq, e);
	n->in = (Agedge_t *) dtextract(g->e_seq);
    }
    return f;
}

Agedge_t *agfstedge(Agnode_t * n)
{
    Agedge_t *rv;
    rv = agfstout(n);
    if (rv == NILedge)
	rv = agfstin(n);
    return rv;
}

Agedge_t *agnxtedge(Agedge_t * e, Agnode_t * n)
{
    Agedge_t *rv;
    if (AGID(n) == AGID(agtail(e))) {
	rv = agnxtout(e);
	if (rv == NILedge)
	    rv = agfstin(n);
    } else
	rv = agnxtin(e);
    return rv;
}

static Agedge_t *agfindedge(Agnode_t * t, Agnode_t * h, Agtag_t key)
{
    Agraph_t *g;
    Agedge_t *e, template;

    assert(agraphof(t) == agraphof(h));

    g = agraphof(t);
    template.base.tag = key;	/* guess that fan-in < fan-out */
    template.node = t;
    if (t != h) {		/* guess that fan-in < fan-out */
	dtrestore(g->e_id, h->inid);
	e = (Agedge_t *) dtsearch(g->e_id, &template);
	h->inid = dtextract(g->e_id);
    } else {
	dtrestore(g->e_id, t->outid);
	e = (Agedge_t *) dtsearch(g->e_id, &template);
	t->outid = dtextract(g->e_id);
    }
    return e;
}

static Agedge_t *agfindedge_by_id(Agnode_t * t, Agnode_t * h,
				  unsigned long id)
{
    Agtag_t tag;

    assert(agraphof(t) == agraphof(h));
    tag = Tag;
    tag.objtype = AGEDGE;
    tag.id = id;
    return agfindedge(t, h, tag);
}

void agedgesetop(Agraph_t * g, Agedge_t * e, int ins)
{
    union {
	Dtlink_t **dtlink;
	Agedge_t **agedge;
    } seq_set;
    Dtlink_t **id_set;
    Agnode_t *n;		/* node where <e> is referenced */

    if (AGTYPE(e) == AGOUTEDGE) {
	n = AGOUT2IN(e)->node;
	seq_set.agedge = &(n->out);
	id_set = &(n->outid);
    } else {
	n = AGIN2OUT(e)->node;
	seq_set.agedge = &(n->in);
	id_set = &(n->inid);
    }

    dtrestore(g->e_seq, *seq_set.dtlink);
    if (ins)
	dtinsert(g->e_seq, e);
    else
	dtdelete(g->e_seq, e);
    *seq_set.dtlink = dtextract(g->e_seq);

    dtrestore(g->e_id, *id_set);
    if (ins)
	dtinsert(g->e_id, e);
    else
	dtdelete(g->e_id, e);
    *id_set = dtextract(g->e_id);
}

/* creates new edge pair and returns outedge */
static Agedgepair_t *newedgepair(Agraph_t * g, Agnode_t * t, Agnode_t * h,
				 unsigned long id, unsigned long seq)
{
    Agedgepair_t *e2;
    Agedge_t *in, *out;

    e2 = (Agedgepair_t *) agalloc(g, sizeof(Agedgepair_t));
    in = &(e2->in);
    out = &(e2->out);
    AGTYPE(in) = AGINEDGE;
    AGTYPE(out) = AGOUTEDGE;
    AGID(in) = AGID(out) = id;
    AGSEQ(in) = AGSEQ(out) = seq;
    in->node = t;
    out->node = h;
    agedgesetop(g, out, TRUE);
    if (t != h)
	agedgesetop(g, in, TRUE);
    return e2;
}

static Agedge_t *mklocaledge(Agraph_t * g, Agnode_t * arg_t,
			     Agnode_t * arg_h, unsigned long id,
			     int *isnew)
{
    Agedge_t *e, *epar;
    Agraph_t *par;
    Agnode_t *t, *h;
    Agtag_t key;
    Agedgepair_t *e2;

    agnotflat(g);

    t = agsubnode(g, arg_t, TRUE);
    h = agsubnode(g, arg_h, TRUE);
    /* protect against multi-edges */
    key = Tag;
    if (agisstrict(g))
	key.objtype = 0;
    else
	key.objtype = AGEDGE;
    key.id = id;
    if ((e = agfindedge(t, h, key)))
	return e;

    if ((par = agparent(g))) {
	epar = mklocaledge(par, t, h, id, isnew);
    } else {
	epar = NILedge;
	*isnew = TRUE;
    }

    e2 = newedgepair(g, t, h, id,
		     epar ? AGSEQ(epar) : agnextseq(g, AGEDGE));
    e = &(e2->out);
    if (epar)
	AGDATA(&(e2->in)) = AGDATA(&(e2->out)) = AGDATA(epar);
    else {
	if (g->desc.has_attrs)
	    (void) agrealbindrec(e, AgDataRecName, sizeof(Agattr_t), FALSE,
				 TRUE);
	/* edgeattr_init and method_init will be called later */
    }
    return e;
}

static Agedge_t *localedge(Agraph_t * g, Agnode_t * arg_t,
			   Agnode_t * arg_h, unsigned long id)
{
    int isnew = FALSE;
    Agedge_t *e;
    e = mklocaledge(g, arg_t, arg_h, id, &isnew);
    if (isnew) {
	if (g->desc.has_attrs)
	    agedgeattr_init(e, TRUE);
	agmethod_init(g, e);
    }
    return e;
}

static int ok_to_make_edge(Agnode_t * t, Agnode_t * h)
{
    Agraph_t *g;
    Agtag_t key;

    g = agraphof(t);

    /* protect against endpoints in different graphs */
    g = agraphof(t);
    if (g != agraphof(h))
	return FALSE;

    /* protect against self, multi-edges in strict graphs */
    if (agisstrict(g)) {
	if (AGID(t) == AGID(h))
	    return FALSE;
	key = Tag;
	key.objtype = 0;	/* wild card */
	if (agfindedge(t, h, key))
	    return FALSE;
    }
    return TRUE;
}

Agedge_t *agidedge(Agnode_t * t, Agnode_t * h, unsigned long id, int cflag)
{
    Agraph_t *g, *root;
    Agnode_t *tr, *hr;
    Agedge_t *e;

    if ((g = agraphof(t)) != agraphof(h))
	return NILedge;
    e = agfindedge_by_id(t, h, id);
    if ((e == NILedge) && agisundirected(g))
	e = agfindedge_by_id(h, t, id);
    if ((e == NILedge) && cflag && ok_to_make_edge(t, h)) {
	root = agroot(g);
	if (((g != root) && ((tr = agsubnode(root, t, FALSE)))	/* old */
	     &&((hr = agsubnode(root, h, FALSE)))
	     && agfindedge_by_id(tr, hr, id))
	    || agallocid(g, AGEDGE, id))	/* new */
	    e = localedge(g, t, h, id);
    }
    return e;
}

Agedge_t *agedge(Agnode_t * t, Agnode_t * h, char *name, int cflag)
{
    Agraph_t *g;
    Agedge_t *e;
    unsigned long id;
    int have_id;

    if ((g = agraphof(t)) != agraphof(h))
	return NILedge;
    have_id = agmapnametoid(g, AGEDGE, name, &id, FALSE);
    if (have_id || ((name == NILstr) && (NOT(cflag) || agisstrict(g)))) {
	/* probe for pre-existing edge */
	Agtag_t key;
	key = Tag;
	if (have_id) {
	    key.id = id;
	    key.objtype = AGEDGE;
	} else {
	    key.id = key.objtype = 0;
	}

	/* might already exist locally */
	e = agfindedge(t, h, key);
	if ((e == NILedge) && agisundirected(g))
	    e = agfindedge(h, t, key);
	if (e)
	    return e;
    }

    if (cflag && ok_to_make_edge(t, h)
	&& agmapnametoid(g, AGEDGE, name, &id, TRUE))	/* reserve id */
	e = localedge(g, t, h, id);
    else
	e = NILedge;
    return e;
}

void agdeledgepair(Agedge_t * e, void *ignored)
{
    Agraph_t *g;
    Agedge_t *in, *out;
    Agnode_t *t, *h;

    NOTUSED(ignored);
    g = agraphof(e);
    agnotflat(g);
    if (AGTYPE(e) == AGINEDGE) {
	in = e;
	out = AGIN2OUT(e);
    } else {
	out = e;
	in = AGOUT2IN(e);
    }
    t = in->node;
    h = out->node;
    agedgesetop(g, out, FALSE);
    if (t != h)
	agedgesetop(g, in, FALSE);
    agfree(g, out);
    for (e = agfstin(h); e; e = agnxtin(e))
	assert(e != in);
    for (e = agfstout(t); e; e = agnxtout(e))
	assert(e != out);
}

int agdeledge(Agedge_t * e)
{
    Agraph_t *g;

    g = agraphof(e);
    e = AGMKOUT(e);

    if (agfindedge(agtail(e), aghead(e), AGTAG(e)) == NILedge)
	return FAILURE;

    if (agisarootobj(e)) {
	if (g->desc.has_attrs)
	    agedgeattr_delete(e);
	agmethod_delete(g, e);
	agrecclose((Agobj_t *) e);
	agfreeid(g, AGEDGE, AGID(e));
    }
    return agapply(g, (Agobj_t *) e, (agobjfn_t) agdeledgepair, NILedge,
		   FALSE);
}


Agedge_t *agsubedge(Agraph_t * g, Agedge_t * arg_e, int cflag)
{
    Agnode_t *t, *h;
    Agedge_t *e;

    if (agraphof(arg_e) == g)
	return arg_e;

    agnotflat(g);
    t = agsubnode(g, AGTAIL(arg_e), cflag);
    h = agsubnode(g, AGHEAD(arg_e), cflag);
    if ((t == NILnode) || (h == NILnode))
	e = NILedge;
    else {
	e = agfindedge(t, h, AGTAG(arg_e));
	if (cflag && (e == NILedge)) {
	    e = localedge(g, t, h, AGID(arg_e));
	}
    }
    if (e && (AGTYPE(e) != AGTYPE(arg_e)))
	e = AGOPP(e);
    return e;
}


/* edge comparison.  OBJTYPE(e) == 0 means ID is a wildcard. */
int agedgecmpf(Dict_t * d, void *arg_e0, void *arg_e1, Dtdisc_t * disc)
{
    int rv;
    Agedge_t *e0, *e1;

    NOTUSED(d);
    e0 = arg_e0;
    e1 = arg_e1;
    NOTUSED(disc);
    rv = AGID(e0->node) - AGID(e1->node);
    if (rv == 0) {		/* same node */
	if ((AGTYPE(e0) == 0) || (AGTYPE(e1) == 0))
	    rv = 0;
	else
	    rv = AGID(e0) - AGID(e1);
    }
    return rv;
}

	/* discipline for edges that compares endpoints + ID */
Dtdisc_t Ag_edge_disc = {
    0,				/* obj is passed as key */
    0,				/* key size (ignored) */
    offsetof(Agedge_t, base.id_link),	/* link offset */
    NIL(Dtmake_f),
    NIL(Dtfree_f),
    agedgecmpf,
    NIL(Dthash_f)
};

/* debug functions */
#ifdef agtail
#undef agtail
#endif
Agnode_t *agtail(Agedge_t * e)
{
    return AGTAIL(e);
}

#ifdef aghead
#undef aghead
#endif
Agnode_t *aghead(Agedge_t * e)
{
    return AGHEAD(e);
}

#ifdef agopp
#undef agopp
#endif
Agedge_t *agopp(Agedge_t * e)
{
    return AGOPP(e);
}

#ifdef NOTDEF
	/* could be useful if we write relabel_edge */
static Agedge_t *agfindedge_by_name(Agnode_t * t, Agnode_t * h, char *name)
{
    unsigned long id;

    if (agmapnametoid(agraphof(t), AGEDGE, name, &id, FALSE))
	return agfindedge_by_id(t, h, id);
    else
	return NILedge;
}
#endif
