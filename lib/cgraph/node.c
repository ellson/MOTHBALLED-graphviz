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

#include <cghdr.h>

Agnode_t *agfindnode_by_id(Agraph_t * g, unsigned long id)
{
    Agsubnode_t *sn;
    static Agsubnode_t template;
    static Agnode_t dummy;

    dummy.base.tag.id = id;
    template.node = &dummy;
    sn = (Agsubnode_t *) dtsearch(g->n_id, &template);
    return sn ? sn->node : NILnode;
}

Agnode_t *agfindnode_by_name(Agraph_t * g, char *name)
{
    unsigned long id;

    if (agmapnametoid(g, AGNODE, name, &id, FALSE))
	return agfindnode_by_id(g, id);
    else
	return NILnode;
}

Agnode_t *agfstnode(Agraph_t * g)
{
    Agsubnode_t *sn;
    sn = (Agsubnode_t *) dtfirst(g->n_seq);
    return sn ? sn->node : NILnode;
}

Agnode_t *agnxtnode(Agraph_t * g, Agnode_t * n)
{
    Agsubnode_t *sn;
    sn = agsubrep(g, n);
    sn = ((Agsubnode_t *) dtnext(g->n_seq, sn));
    return sn ? sn->node : NILnode;
}

Agnode_t *aglstnode(Agraph_t * g)
{
    Agsubnode_t *sn;
    sn = (Agsubnode_t *) dtlast(g->n_seq);
    return sn ? sn->node : NILnode;
}

Agnode_t *agprvnode(Agraph_t * g, Agnode_t * n)
{
    Agsubnode_t *sn;
    sn = agsubrep(g, n);
    sn = ((Agsubnode_t *) dtprev(g->n_seq, sn));
    return sn ? sn->node : NILnode;
}


/* internal node constructor */
static Agnode_t *newnode(Agraph_t * g, unsigned long id, unsigned long seq)
{
    Agnode_t *n;

    n = agalloc(g, sizeof(Agnode_t));
    AGTYPE(n) = AGNODE;
    AGID(n) = id;
    AGSEQ(n) = seq;
    n->root = agroot(g);
    if (agroot(g)->desc.has_attrs)
	(void) agbindrec(n, AgDataRecName, sizeof(Agattr_t), FALSE);
    /* nodeattr_init and method_init will be called later, from the
     * subgraph where the node was actually created, but first it has
     * to be installed in all the (sub)graphs up to root. */
    return n;
}

static void installnode(Agraph_t * g, Agnode_t * n)
{
    Agsubnode_t *sn;
    if (g == agroot(g)) sn = &(n->mainsub);
    else sn = agalloc(g, sizeof(Agsubnode_t));
    sn->node = n;
    dtinsert(g->n_id, sn);
    dtinsert(g->n_seq, sn);
}

static void installnodetoroot(Agraph_t * g, Agnode_t * n)
{
    Agraph_t *par;
    installnode(g, n);
    if ((par = agparent(g)))
	installnodetoroot(par, n);
}

static void initnode(Agraph_t * g, Agnode_t * n)
{
    if (agroot(g)->desc.has_attrs)
	agnodeattr_init(g,n);
    agmethod_init(g, n);
}

/* external node constructor - create by id */
Agnode_t *agidnode(Agraph_t * g, unsigned long id, int cflag)
{
    Agraph_t *root;
    Agnode_t *n;

    n = agfindnode_by_id(g, id);
    if ((n == NILnode) && cflag) {
	root = agroot(g);
	if ((g != root) && ((n = agfindnode_by_id(root, id))))	/*old */
	    agsubnode(g, n, TRUE);	/* insert locally */
	else {
	    if (agallocid(g, AGNODE, id)) {	/* new */
		n = newnode(g, id, agnextseq(g, AGNODE));
		installnodetoroot(g, n);
		initnode(g, n);
	    } else
		n = NILnode;	/* allocid for new node failed */
	}
    }
    /* else return probe result */
    return n;
}

Agnode_t *agnode(Agraph_t * g, char *name, int cflag)
{
    Agraph_t *root;
    Agnode_t *n;
    unsigned long id;

    root = agroot(g);
    /* probe for existing node */
    if (agmapnametoid(g, AGNODE, name, &id, FALSE)) {
	if ((n = agfindnode_by_id(g, id)))
	    return n;

	/* might already exist globally, but need to insert locally */
	if (cflag && (g != root) && ((n = agfindnode_by_id(root, id)))) {
	    return agsubnode(g, n, TRUE);
	}
    }

    if (cflag && agmapnametoid(g, AGNODE, name, &id, TRUE)) {	/* reserve id */
	n = newnode(g, id, agnextseq(g, AGNODE));
	installnodetoroot(g, n);
	initnode(g, n);
	return n;
    }

    return NILnode;
}

/* removes image of node and its edges from graph.
   caller must ensure n belongs to g. */
void agdelnodeimage(Agraph_t * g, Agnode_t * n, void *ignored)
{
    Agedge_t *e, *f;
    static Agsubnode_t template;
    template.node = n;

    NOTUSED(ignored);
    for (e = agfstedge(g, n); e; e = f) {
	f = agnxtedge(g, e, n);
	agdeledgeimage(g, e, 0);
    }
    dtdelete(g->n_id, &template);
    dtdelete(g->n_seq, &template);
}

int agdelnode(Agraph_t * g, Agnode_t * n)
{
    Agedge_t *e, *f;

    if (!agfindnode_by_id(g, AGID(n)))
	return FAILURE;		/* bad arg */
    if (g == agroot(g)) {
	for (e = agfstedge(g, n); e; e = f) {
	    f = agnxtedge(g, e, n);
	    agdeledge(g, e);
	}
	if (g->desc.has_attrs)
	    agnodeattr_delete(n);
	agmethod_delete(g, n);
	agrecclose((Agobj_t *) n);
	agfreeid(g, AGNODE, AGID(n));
    }
    return agapply(g, (Agobj_t *) n, (agobjfn_t) agdelnodeimage, NILnode,
		   FALSE);
    if (g == agroot(g))
	agfree(g, n);
}

static void dict_relabel(Agnode_t * n, void *arg)
{
    Agraph_t *g;
    unsigned long new_id;

    g = agraphof(n);
    new_id = *(unsigned long *) arg;
    dtdelete(g->n_id, n);	/* wrong, should be subrep */
    AGID(n) = new_id;
    dtinsert(g->n_id, n);	/* also wrong */
    /* because all the subgraphs share the same node now, this
       now requires a separate deletion and insertion phase */
}

int agrelabel_node(Agnode_t * n, char *newname)
{
    Agraph_t *g;
    unsigned long new_id;

    g = agroot(agraphof(n));
    if (agfindnode_by_name(g, newname))
	return FAILURE;
    if (agmapnametoid(g, AGNODE, newname, &new_id, TRUE)) {
	if (agfindnode_by_id(agroot(g), new_id) == NILnode) {
	    agfreeid(g, AGNODE, AGID(n));
	    agapply(g, (Agobj_t *) n, (agobjfn_t) dict_relabel,
		    (void *) &new_id, FALSE);
	    return SUCCESS;
	} else {
	    agfreeid(g, AGNODE, new_id);	/* couldn't use it after all */
	}
    }
    return FAILURE;
}

/* lookup or insert <n> in <g> */
Agnode_t *agsubnode(Agraph_t * g, Agnode_t * n0, int cflag)
{
    Agraph_t *par;
    Agnode_t *n;

    if (agroot(g) != n0->root)
	return NILnode;
    n = agfindnode_by_id(g, AGID(n0));
    if ((n == NILnode) && cflag) {
	if ((par = agparent(g))) {
	    n = agsubnode(par, n0, cflag);
	    installnode(g, n);
	    /* no callback for existing node insertion in subgraph (?) */
	}
	/* else impossible that <n> doesn't belong to <g> */
    }
    /* else lookup succeeded */
    return n;
}

int agsubnodeidcmpf(Dict_t * d, void *arg0, void *arg1, Dtdisc_t * disc)
{
    Agsubnode_t *sn0, *sn1;
    sn0 = (Agsubnode_t *) arg0;
    sn1 = (Agsubnode_t *) arg1;
    return (AGID(sn0->node) - AGID(sn1->node));
}

int agsubnodeseqcmpf(Dict_t * d, void *arg0, void *arg1, Dtdisc_t * disc)
{
    Agsubnode_t *sn0, *sn1;
    sn0 = (Agsubnode_t *) arg0;
    sn1 = (Agsubnode_t *) arg1;
    return (AGSEQ(sn0->node) - AGSEQ(sn1->node));
}

Dtdisc_t Ag_subnode_id_disc = {
    0,				/* pass object ptr  */
    0,				/* size (ignored)   */
    offsetof(Agsubnode_t, id_link),	/* link offset */
    NIL(Dtmake_f),
    NIL(Dtfree_f),
    agsubnodeidcmpf,
    NIL(Dthash_f),
    agdictobjmem,
    NIL(Dtevent_f)
};

Dtdisc_t Ag_subnode_seq_disc = {
    0,				/* pass object ptr  */
    0,				/* size (ignored)   */
    offsetof(Agsubnode_t, seq_link),	/* link offset */
    NIL(Dtmake_f),
    NIL(Dtfree_f),
    agsubnodeseqcmpf,
    NIL(Dthash_f),
    agdictobjmem,
    NIL(Dtevent_f)
};
