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
 * gpr: graph pattern recognizer
 *
 * Written by Emden Gansner
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#ifdef HAVE_UNISTD_H
#include	<unistd.h>
#endif
#include <gprstate.h>
#include <agraph.h>
#include <ingraphs.h>
#include <compile.h>
#include <queue.h>
#include <sfstr.h>
#include <error.h>
#include <string.h>
#include <ctype.h>
#ifdef HAVE_GETOPT_H
#include <getopt.h>
#else
#include "compat_getopt.h"
#endif

char *Info[] = {
    "dot",			/* Program */
    VERSION,			/* Version */
    BUILDDATE			/* Build Date */
};

#define DFLT_GPRPATH    "."

static const char *usage =
    ": gvpr [-o <ofile>] [-a <args>] ([-f <prog>] | 'prog') [files]\n\
   -c         - use source graph for output\n\
   -f <pfile> - find program in file <pfile>\n\
   -i         - create node induced subgraph\n\
   -a <args>  - string arguments available as ARGV[0..]\n\
   -o <ofile> - write output to <ofile>; stdout by default\n\
   -V         - print version info\n\
   -?         - print usage info\n\
If no files are specified, stdin is used\n";

struct {
    char *cmdName;
    Sfio_t *outFile;		/* output stream; stdout default */
    char *program;
    int useFile;		/* true if program comes from a file */
    int compflags;
    char **inFiles;
    int argc;
    char **argv;
} options;

static Sfio_t *openOut(char *name)
{
    Sfio_t *outs;

    outs = sfopen(0, name, "w");
    if (outs == 0) {
	error(ERROR_FATAL, "could not open %s for writing", name);
    }
    return outs;
}

/* gettok:
 * Return next argument token, returning NULL if none.
 * sp is updated to point to next character to be processed.
 */
static char*
gettok (char** sp)
{
    char* s = *sp;
    char* ws = s;
    char* rs = s;
    char  c;
    char  q = '\0';  /* if non-0, in quote mode with quote char q */

    while (isspace(*rs)) rs++;
    if ((c = *rs) == '\0') return NULL;
    while ((c = *rs)) {
        if (q && (q == c)) {  /* end quote */
	    q = '\0';
	}
	else if (!q && ((c == '"') || (c == '\''))) { 
	    q = c;
	}
        else if (c == '\\') {
	    rs++;
	    c = *rs;
	    if (c) *ws++ = c;
	    else {
		error(ERROR_WARNING, 
                  "backslash in argument followed by no character - ignored");
		rs--;
	    }
	} 
	else if (q || !isspace(c))
	    *ws++ = c;
	else break;
        rs++;
    }
    *ws = '\0';
    if (*rs) *rs++;
    else if (q)
	error(ERROR_WARNING, "no closing quote for argument %s", s);
    *sp = rs;
    return s;
}

#define NUM_ARGS 10

/* parseArgs:
 * Tokenize a string. Tokens consist of either a non-empty string
 * of non-space characters, or all characters between a pair of
 * single or double quotes. As usual, we map 
 *   \c -> c
 * for all c
 */
static void parseArgs(char *s, int *argc, char ***argv)
{
    int i, cnt = 0;
    char *args[NUM_ARGS];
    char *t;
    char **av;

    while ((t = gettok(&s))) {
	if (cnt == NUM_ARGS) {
	    error(ERROR_WARNING,
		  "at most %d arguments allowed in -a flag - ignoring rest",
		  NUM_ARGS);
	    break;
	}
	args[cnt++] = t;
    }

    if (cnt) {
	av = oldof(0, char *, cnt, 0);
	for (i = 0; i < cnt; i++)
	    av[i] = strdup(args[i]);
	*argv = av;
    }
    *argc = cnt;
}

/* resolve:
 * Translate -f arg parameter into a pathname.
 * If arg contains '/', return arg.
 * Else search directories in GPRPATH for arg.
 */
static char *resolve(char *arg)
{
    char *path;
    char *s;
    char *cp;
    char *fname = 0;
    Sfio_t *fp;
    size_t sz;

    if (strchr(arg, '/'))
	return arg;

    path = getenv("GPRPATH");
    if (!path)
	path = DFLT_GPRPATH;

    if (!(fp = sfstropen()))
	error(ERROR_FATAL, "Could not open buffer");

    while (*path && !fname) {
	if (*path == ':') {	/* skip colons */
	    path++;
	    continue;
	}
	cp = strchr(path, ':');
	if (cp) {
	    sz = (size_t) (cp - path);
	    sfwrite(fp, path, sz);
	    path = cp + 1;	/* skip past current colon */
	} else {
	    sz = sfprintf(fp, path);
	    path += sz;
	}
	sfputc(fp, '/');
	sfprintf(fp, arg);
	s = sfstruse(fp);

	if (access(s, R_OK) == 0) {
	    fname = strdup(s);
	}
    }

    if (!fname)
	error(ERROR_FATAL, "Could not find file \"%s\" in GPRPATH", arg);

    sfclose(fp);
    return fname;
}

/* scanArgs:
 * Parse command line options.
 */
static void scanArgs(int argc, char **argv)
{
    int c;
    char *outname = 0;

    options.cmdName = argv[0];
    options.outFile = 0;
    options.useFile = 0;
    error_info.id = options.cmdName;

    while ((c = getopt(argc, argv, ":?Vcia:f:o:")) != -1) {
	switch (c) {
	case 'c':
	    options.compflags |= SRCOUT;
	    break;
	case 'f':
	    options.useFile = 1;
	    options.program = resolve(optarg);
	    break;
	case 'i':
	    options.compflags |= INDUCE;
	    break;
	case 'a':
	    parseArgs(optarg, &options.argc, &options.argv);
	    break;
	case 'o':
	    outname = optarg;
	    break;
	case 'V':
	    fprintf(stderr, "%s version %s (%s)\n",
		    Info[0], Info[1], Info[2]);
	    exit(0);
	    break;
	case '?':
	    if (optopt == '?') {
		error(ERROR_USAGE, "%s", usage);
		exit(0);
	    } else {
		error(2, "option -%c unrecognized", optopt);
	    }
	    break;
	case ':':
	    error(2, "missing argument for option -%c", optopt);
	    break;
	}
    }
    argv += optind;
    argc -= optind;

    /* Handle additional semantics */
    if (options.useFile == 0) {
	if (argc == 0) {
	    error(2, "No program supplied via argument or -f option");
	    error_info.errors = 1;
	} else {
	    options.program = *argv++;
	    argc--;
	}
    }
    if (argc == 0)
	options.inFiles = 0;
    else
	options.inFiles = argv;
    if (outname)
	options.outFile = openOut(outname);
    else
	options.outFile = sfstdout;

    if (error_info.errors)
	error(ERROR_USAGE | 4, "%s", usage);
}

static void evalEdge(Gpr_t * state, comp_prog * xprog, Agedge_t * e)
{
    int i;
    case_stmt *cs;
    int okay;

    state->curobj = (Agobj_t *) e;
    for (i = 0; i < xprog->n_estmts; i++) {
	cs = xprog->edge_stmts + i;
	if (cs->guard)
	    okay = (exeval(xprog->prog, cs->guard, state)).integer;
	else
	    okay = 1;
	if (okay) {
	    if (cs->action)
		exeval(xprog->prog, cs->action, state);
	    else
		agsubedge(state->target, e, TRUE);
	}
    }
}

static void evalNode(Gpr_t * state, comp_prog * xprog, Agnode_t * n)
{
    int i;
    case_stmt *cs;
    int okay;

    state->curobj = (Agobj_t *) n;
    for (i = 0; i < xprog->n_nstmts; i++) {
	cs = xprog->node_stmts + i;
	if (cs->guard)
	    okay = (exeval(xprog->prog, cs->guard, state)).integer;
	else
	    okay = 1;
	if (okay) {
	    if (cs->action)
		exeval(xprog->prog, cs->action, state);
	    else
		agsubnode(state->target, n, TRUE);
	}
    }
}

typedef struct {
    Agnode_t *oldroot;
    Agnode_t *prev;
} nodestream;

static Agnode_t *nextNode(Gpr_t * state, nodestream * nodes)
{
    Agnode_t *np;

    if (state->tvroot != nodes->oldroot) {
	np = nodes->oldroot = state->tvroot;
    } else if (nodes->prev) {
	np = nodes->prev = agnxtnode(nodes->prev);
    } else {
	np = nodes->prev = agfstnode(state->curgraph);
    }
    return np;
}

#define MARKED(x)  (((x)->iu.integer)&1)
#define MARK(x)  (((x)->iu.integer) = 1)
#define ONSTACK(x)  (((x)->iu.integer)&2)
#define PUSH(x)  (((x)->iu.integer)|=2)
#define POP(x)  (((x)->iu.integer)&=(~2))

typedef Agedge_t *(*fstedgefn_t) (Agnode_t *);
typedef Agedge_t *(*nxttedgefn_t) (Agedge_t *, Agnode_t *);

typedef struct {
    fstedgefn_t fstedge;
    nxttedgefn_t nxtedge;
} trav_fns;

static trav_fns DFSfns = { agfstedge, agnxtedge };
static trav_fns FWDfns = { agfstout, (nxttedgefn_t) agnxtout };
static trav_fns REVfns = { agfstin, (nxttedgefn_t) agnxtin };

static void travBFS(Gpr_t * state, comp_prog * xprog)
{
    nodestream nodes;
    queue *q;
    ndata *nd;
    Agnode_t *n;
    Agedge_t *cure;

    q = mkQueue();
    nodes.oldroot = 0;
    nodes.prev = 0;
    while ((n = nextNode(state, &nodes))) {
	nd = nData(n);
	if (MARKED(nd))
	    continue;
	PUSH(nd);
	push (q,n);
	while ((n = pull(q))) {
	    nd = nData(n);
	    MARK(nd);
 	    POP(nd);
	    evalNode(state, xprog, n);
	    for (cure = agfstedge(n); cure; cure = agnxtedge(cure, n)) {
		nd = nData(cure->node);
		if (MARKED(nd)) continue;
		evalEdge(state, xprog, cure);
		if (!ONSTACK(nd)) {
		    push(q, cure->node);
		    PUSH(nd);
		}
	    }
	}
    }
    freeQ (q);
}

static void travDFS(Gpr_t * state, comp_prog * xprog, trav_fns * fns)
{
    Agnode_t *n;
    queue *stk;
    Agnode_t *curn;
    Agedge_t *cure;
    Agedge_t *entry;
    int more;
    ndata *nd;
    nodestream nodes;
    Agedgepair_t seed;

    stk = mkStack();
    nodes.oldroot = 0;
    nodes.prev = 0;
    while ((n = nextNode(state, &nodes))) {
	nd = nData(n);
	if (MARKED(nd))
	    continue;
	seed.out.node = n;
	seed.in.node = 0;
	curn = n;
	entry = &(seed.out);
	cure = 0;
	MARK(nd);
	PUSH(nd);
	evalNode(state, xprog, n);
	more = 1;
	while (more) {
	    if (cure)
		cure = fns->nxtedge(cure, curn);
	    else
		cure = fns->fstedge(curn);
	    if (cure) {
		if (entry == agopp(cure))
		    continue;
		nd = nData(cure->node);
		if (MARKED(nd)) {
		    if (ONSTACK(nd))
			evalEdge(state, xprog, cure);
		} else {
		    evalEdge(state, xprog, cure);
		    push(stk, entry);
		    entry = cure;
		    curn = cure->node;
		    cure = 0;
		    evalNode(state, xprog, curn);
		    MARK(nd);
		    PUSH(nd);
		}
	    } else {
		nd = nData(curn);
		POP(nd);
		cure = entry;
		entry = (Agedge_t *) pull(stk);
		if (entry)
		    curn = entry->node;
		else
		    more = 0;
	    }
	}
    }
    freeQ (stk);
}

static void travNodes(Gpr_t * state, comp_prog * xprog)
{
    Agnode_t *n;

    for (n = agfstnode(state->curgraph); n; n = agnxtnode(n)) {
	evalNode(state, xprog, n);
    }
}

static void travEdges(Gpr_t * state, comp_prog * xprog)
{
    Agnode_t *n;
    Agedge_t *e;

    for (n = agfstnode(state->curgraph); n; n = agnxtnode(n)) {
	for (e = agfstout(n); e; e = agnxtout(e)) {
	    evalEdge(state, xprog, e);
	}
    }
}

static void travFlat(Gpr_t * state, comp_prog * xprog)
{
    Agnode_t *n;
    Agedge_t *e;

    for (n = agfstnode(state->curgraph); n; n = agnxtnode(n)) {
	evalNode(state, xprog, n);
	if (xprog->n_estmts > 0) {
	    for (e = agfstout(n); e; e = agnxtout(e)) {
		evalEdge(state, xprog, e);
	    }
	}
    }
}

/* traverse:
 */
static void traverse(Gpr_t * state, comp_prog * xprog)
{
    char *target;

    if (state->name_used) {
	sfprintf(state->tmp, "%s%d", state->tgtname, state->name_used);
	target = sfstruse(state->tmp);
    } else
	target = state->tgtname;
    state->name_used++;
    state->target = openSubg(state->curgraph, target);
    if (!state->outgraph)
	state->outgraph = state->target;

    switch (state->tvt) {
    case TV_flat:
	travFlat(state, xprog);
	break;
    case TV_bfs:
	travBFS(state, xprog);
	break;
    case TV_dfs:
	travDFS(state, xprog, &DFSfns);
	break;
    case TV_fwd:
	travDFS(state, xprog, &FWDfns);
	break;
    case TV_rev:
	travDFS(state, xprog, &REVfns);
	break;
    case TV_ne:
	travNodes(state, xprog);
	travEdges(state, xprog);
	break;
    case TV_en:
	travEdges(state, xprog);
	travNodes(state, xprog);
	break;
    }
}

static void chkClose(Agraph_t * g)
{
    gdata *data;

    data = gData(g);
    if (data->lock & 1)
	data->lock |= 2;
    else
	agclose(g);
}

static void *ing_open(char *f)
{
    return sfopen(0, f, "r");
}

static Agraph_t *ing_read(void *fp)
{
    return readG((Sfio_t *) fp);
}

static int ing_close(void *fp)
{
    return sfclose((Sfio_t *) fp);
}

static ingdisc ingDisc = { ing_open, ing_read, ing_close, &_Sfstdin };

int main(int argc, char *argv[])
{
    parse_prog *prog;
    ingraph_state *ing;
    comp_prog *xprog;
    Gpr_t *state;
    gpr_info info;

    scanArgs(argc, argv);

    prog = parseProg(options.program, options.useFile);
    state = openGPRState();
    xprog = compileProg(prog, state, options.compflags);
    info.outFile = options.outFile;
    info.argc = options.argc;
    info.argv = options.argv;
    initGPRState(state, xprog->prog->vm, &info);

    /* do begin */
    if (xprog->begin_stmt)
	exeval(xprog->prog, xprog->begin_stmt, state);

    /* if program is not null */
    if (usesGraph(xprog)) {
	ing = newIng(0, options.inFiles, &ingDisc);
	while ((state->curgraph = nextGraph(ing))) {
	    state->infname = fileName(ing);
	    /* begin graph */
	    state->curobj = (Agobj_t *) state->curgraph;
	    state->tvroot = 0;
	    if (xprog->begg_stmt)
		exeval(xprog->prog, xprog->begg_stmt, state);

	    /* walk graph */
	    if (walksGraph(xprog))
		traverse(state, xprog);

	    /* end graph */
	    state->curobj = (Agobj_t *) state->curgraph;
	    if (xprog->endg_stmt)
		exeval(xprog->prog, xprog->endg_stmt, state);

	    /* if $O == $G and $T is empty, delete $T */
	    if ((state->outgraph == state->curgraph) &&
		(state->target) && !agnnodes(state->target))
		agdelete(state->curgraph, state->target);

	    /* output graph, if necessary */
	    if (state->outgraph && agnnodes(state->outgraph))
		agwrite(state->outgraph, options.outFile);

	    chkClose(state->curgraph);
	    state->target = 0;
	    state->outgraph = 0;
	}
    }

    /* do end */
    state->curgraph = 0;
    state->curobj = 0;
    if (xprog->end_stmt)
	exeval(xprog->prog, xprog->end_stmt, state);

    exit(0);
}
