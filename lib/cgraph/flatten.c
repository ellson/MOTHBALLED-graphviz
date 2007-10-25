#include "cghdr.h"

static void agflatten_elist(Dict_t * d, Dtlink_t ** lptr)
{
    dtrestore(d, *lptr);
    (void) dtflatten(d);
    *lptr = dtextract(d);
}

void agflatten_edges(Agraph_t * g, Agnode_t * n)
{
    Agsubnode_t *sn;
    Dtlink_t **tmp;

	sn = agsubrep(g,n);
    tmp = &(sn->out_seq); /* avoiding - "dereferencing type-punned pointer will break strict-aliasing rules" */
    agflatten_elist(g->e_seq, tmp);
    tmp = &(sn->in_seq);
    agflatten_elist(g->e_seq, tmp);
}

void agflatten(Agraph_t * g, int flag)
{
    Agnode_t *n;

    if (flag) {
	if (g->desc.flatlock == FALSE) {
	    dtflatten(g->n_seq);
	    g->desc.flatlock = TRUE;
	    for (n = agfstnode(g); n; n = agnxtnode(g,n))
		agflatten_edges(g, n);
	}
    } else {
	if (g->desc.flatlock) {
	    g->desc.flatlock = FALSE;
	}
    }
}

void agnotflat(Agraph_t * g)
{
    if (g->desc.flatlock)
	agerr(AGERR, "flat lock broken");
}
