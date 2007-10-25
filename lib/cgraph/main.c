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

#include <stdio.h>
#include <cgraph.h>

static void prstats(Agraph_t * g, int verbose);
static void do_it(Agraph_t * g, int dostat);


static void my_ins(Agraph_t * g, Agobj_t * obj, void *context)
{
    Agnode_t *n;

    if (AGTYPE(obj) == AGNODE) {
	n = (Agnode_t *) obj;
	fprintf(stderr, "%s initialized with label %s\n", agnameof(n),
		agget(n, "label"));
    }
}

static Agcbdisc_t mydisc = { {0, 0, 0}, {my_ins, 0, 0}, {0, 0, 0} };

main(int argc, char **argv)
{
    Agraph_t *g, *prev;
    int dostat;

    if (argc > 1)
	dostat = atoi(argv[1]);
    else
	dostat = 0;

    prev = agopen("some_name", Agdirected, NIL(Agdisc_t *));
    agcallbacks(prev, FALSE);
    agpushdisc(prev, &mydisc, NIL(void *));
    while (g = agconcat(prev, stdin, NIL(Agdisc_t *))) {
	/*do_it(g, dostat); */
    }
    /*agwrite(prev,stdout); */
    fprintf(stderr, "ready to go, computer fans\n");
    agcallbacks(prev, TRUE);
    agclose(prev);
    return 1;
}

static void prstats(Agraph_t * g, int verbose)
{
#ifdef HAVE_VMALLOC
    Vmstat_t ss, *s;
    vmstat(g->cmn->heap, &ss);
    s = &ss;
    if (verbose)
	fprintf(stderr,
		"n_busy %d n_free %d s_busy %d s_free %d m_busy %d m_free %d n_seg %d extent %d\n",
		s->n_busy, s->n_free, s->s_busy, s->s_free, s->m_busy,
		s->m_free, s->n_seg, s->extent);
    else
	fprintf(stderr, "%d (%d,%d)\n", s->extent, s->s_busy, s->s_free);
#endif
}

static void do_it(Agraph_t * g, int dostat)
{
    agwrite(g, stdout);
    if (dostat)
	prstats(g, dostat > 1);
}
