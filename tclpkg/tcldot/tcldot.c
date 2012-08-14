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


#include "tcldot.h"

#ifdef WITH_CGRAPH

// Agiddisc functions
static void *myiddisc_open(Agraph_t *g, Agdisc_t *disc) {
    ictx_t *ictx = (ictx_t *)disc;
    gctx_t *gctx;

    gctx = (gctx_t *)malloc(sizeof(gctx_t));
    gctx->g = g;
    gctx->ictx = ictx;
    return (void *)gctx;
}
static long myiddisc_map(void *state, int objtype, char *str, unsigned long *id, int createflag) {
    gctx_t *gctx = (gctx_t *)state;
    ictx_t *ictx = gctx->ictx;
    Tcl_Interp *interp = ictx->interp;
    Tcl_CmdProc *proc = NULL;
    Agraph_t *g = NULL;
    char *s, buf[30] = "";

    if (str) {
        if (createflag)
            s = agstrdup(g, str);
        else
            s = agstrbind(g, str);
        *id = (unsigned long) s;
    } else {
        *id = ictx->ctr;
        ictx->ctr += 2;
    }
    switch (objtype) {
        case AGRAPH: sprintf(buf,"graph%lu",*id); proc=graphcmd; break;
        case AGNODE: sprintf(buf,"node%lu",*id); proc=nodecmd; break;
        case AGINEDGE:
        case AGOUTEDGE: sprintf(buf,"edge%lu",*id); proc=edgecmd; break;
    }
    Tcl_AppendResult(interp, buf, NULL);
    if (createflag) {
#ifndef TCLOBJ
	Tcl_CreateCommand(interp, Tcl_GetStringResult(interp), proc, (ClientData) gctx, (Tcl_CmdDeleteProc *) NULL);
#else
	Tcl_CreateObjCommand(interp, Tcl_GetStringResult(interp), proc, (ClientData) gctx, (Tcl_CmdDeleteProc *) NULL);
#endif           
    }
    return TRUE;
}
/* we don't allow users to explicitly set IDs, either */
static long myiddisc_alloc(void *state, int objtype, unsigned long request_id) {
    NOTUSED(state);
    NOTUSED(objtype);
    NOTUSED(request_id);
    return FALSE;
}
static void myiddisc_free(void *state, int objtype, unsigned long id) {
    gctx_t *gctx = (gctx_t *)state;
    ictx_t *ictx = gctx->ictx;
    char buf[30] = "";

    switch (objtype) {
        case AGRAPH: sprintf(buf,"graph%lu",id); break;
        case AGNODE: sprintf(buf,"node%lu",id); break;
        case AGINEDGE:
        case AGOUTEDGE: sprintf(buf,"edge%lu",id); break;
    }
    Tcl_DeleteCommand(ictx->interp, buf);
    if (id % 2 == 0)
        agstrfree((Agraph_t *) state, (char *) id);
}
static char *myiddisc_print(void *state, int objtype, unsigned long id) {
    NOTUSED(state);
    NOTUSED(objtype);
    if (id % 2 == 0)
        return (char *) id;
    else
        return "";
}
static void myiddisc_close(void *state) {
    free(state);
}
static Agiddisc_t myiddisc = {
    myiddisc_open,
    myiddisc_map,
    myiddisc_alloc,
    myiddisc_free,
    myiddisc_print,
    myiddisc_close
};

#endif // WITH_CGRAPH

static int dotnew(ClientData clientData, Tcl_Interp * interp,
#ifndef TCLOBJ
		  int argc, char *argv[]
#else				/* TCLOBJ */
		  int argc, Tcl_Obj * CONST objv[]
#endif				/* TCLOBJ */
    )
{
    ictx_t *ictx = (ictx_t *)clientData;
    Agraph_t *g;
    char c;
    int i, length;
#ifndef WITH_CGRAPH
    Agraph_t **gp;
    int kind;
    unsigned long id;
#else
    Agdesc_t kind;
#endif

    if ((argc < 2)) {
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
			 " graphtype ?graphname? ?attributename attributevalue? ?...?\"",
			 NULL);
	return TCL_ERROR;
    }
    c = argv[1][0];
    length = strlen(argv[1]);
    if ((c == 'd') && (strncmp(argv[1], "digraph", length) == 0)) {
#ifndef WITH_CGRAPH
	kind = AGDIGRAPH;
#else
	kind = Agdirected;
#endif
    } else if ((c == 'd')
	       && (strncmp(argv[1], "digraphstrict", length) == 0)) {
#ifndef WITH_CGRAPH
	kind = AGDIGRAPHSTRICT;
#else
	kind = Agstrictdirected;
#endif
    } else if ((c == 'g') && (strncmp(argv[1], "graph", length) == 0)) {
#ifndef WITH_CGRAPH
	kind = AGRAPH;
#else
	kind = Agundirected;
#endif
    } else if ((c == 'g')
	       && (strncmp(argv[1], "graphstrict", length) == 0)) {
#ifndef WITH_CGRAPH
	kind = AGRAPHSTRICT;
#else
	kind = Agstrictundirected;
#endif
    } else {
	Tcl_AppendResult(interp, "bad graphtype \"", argv[1], "\": must be one of:",
			 "\n\tdigraph, digraphstrict, graph, graphstrict.", NULL);
	return TCL_ERROR;
    }
#ifndef WITH_CGRAPH
    gp = (Agraph_t **) tclhandleAlloc(ictx->graphTblPtr, Tcl_GetStringResult(interp), &id);
#endif
    if (argc % 2) {
	/* if odd number of args then argv[2] is name */
#ifndef WITH_CGRAPH
	g = agopen(argv[2], kind);
#else
	g = agopen(argv[2], kind, (Agdisc_t*)ictx);
#endif
	i = 3;
    } else {
	/* else use handle as name */
#ifndef WITH_CGRAPH
	g = agopen(Tcl_GetStringResult(interp), kind);
#else
	g = agopen(Tcl_GetStringResult(interp), kind, (Agdisc_t*)ictx);
#endif
	i = 2;
    }
#ifdef WITH_CGRAPH
    agbindrec(g, "Agraphinfo_t", sizeof(Agraphinfo_t), TRUE);
#endif
    if (!g) {
	Tcl_AppendResult(interp, "\nFailure to open graph.", NULL);
	return TCL_ERROR;
    }
#ifndef WITH_CGRAPH
    *gp = g;
    AGID(g) = id;
#ifndef TCLOBJ
    Tcl_CreateCommand(interp, Tcl_GetStringResult(interp), graphcmd,
		      (ClientData) ictx, (Tcl_CmdDeleteProc *) NULL);
#else				/* TCLOBJ */
    Tcl_CreateObjCommand(interp, Tcl_GetStringResult(interp), graphcmd,
			 (ClientData) ictx, (Tcl_CmdDeleteProc *) NULL);
#endif				/* TCLOBJ */
#endif
    setgraphattributes(g, &argv[i], argc - i);
    /* we use GD_drawing(g) as a flag that layout has been done.
     * so we make sure that it is initialized to "not done" */
    GD_drawing(g) = NULL;

    return TCL_OK;
}

#ifdef WITH_CGRAPH
static void
init_graphs (ictx_t *ictx, graph_t* g)
{
    Agraph_t *sg, **sgp;
    unsigned long id;
    char buf[16];
    Tcl_Interp *interp = ictx->interp;

    for (sg = agfstsubg (g); sg; sg = agnxtsubg (sg))
	init_graphs (ictx, sg);

    sgp = (Agraph_t **) tclhandleAlloc(ictx->graphTblPtr, buf, &id);
    *sgp = g;
    AGID(g) = id;
#ifndef TCLOBJ
    Tcl_CreateCommand(interp, buf, graphcmd, (ClientData) ictx, (Tcl_CmdDeleteProc *) NULL);
#else				/* TCLOBJ */
    Tcl_CreateObjCommand(interp, buf, graphcmd, (ClientData) ictx, (Tcl_CmdDeleteProc *) NULL);
#endif				/* TCLOBJ */
    if (agroot(g) == g)
	Tcl_SetResult(interp, buf, TCL_VOLATILE);
}
#endif

/*
 * when a graph is read in from a file or string we need to walk
 * it to create the handles and tcl commands for each 
 * graph, subgraph, node, and edge.
 */
static int tcldot_fixup(ictx_t *ictx, graph_t * g)
{
#ifndef WITH_CGRAPH
    Agraph_t **gp, *sg, **sgp;
#endif
    Agnode_t *n, **np;
    Agedge_t *e, **ep;
    char buf[16];
    unsigned long id;
    Tcl_Interp *interp = ictx->interp;

#ifdef WITH_CGRAPH
    init_graphs (ictx, g);
#else
    if (g->meta_node) {
	for (n = agfstnode(g->meta_node->graph); n;
	     n = agnxtnode(g->meta_node->graph, n)) {
	    sg = agusergraph(n);
	    sgp = (Agraph_t **) tclhandleAlloc(ictx->graphTblPtr, buf, &id);
	    *sgp = sg;
	    AGID(sg) = id;
#ifndef TCLOBJ
	    Tcl_CreateCommand(interp, buf, graphcmd, (ClientData) ictx,
			      (Tcl_CmdDeleteProc *) NULL);
#else				/* TCLOBJ */
	    Tcl_CreateObjCommand(interp, buf, graphcmd, (ClientData) ictx,
				 (Tcl_CmdDeleteProc *) NULL);
#endif				/* TCLOBJ */
	    if (sg == g)
		Tcl_SetResult(interp, buf, TCL_VOLATILE);
	}
    } else {
	gp = (Agraph_t **) tclhandleAlloc(ictx->graphTblPtr, Tcl_GetStringResult(interp), &id);
	*gp = g;
	AGID(g) = id;
#ifndef TCLOBJ
	Tcl_CreateCommand(interp, Tcl_GetStringResult(interp), graphcmd,
			  (ClientData) ictx, (Tcl_CmdDeleteProc *) NULL);
#else				/* TCLOBJ */
	Tcl_CreateObjCommand(interp, Tcl_GetStringResult(interp), graphcmd,
			     (ClientData) ictx, (Tcl_CmdDeleteProc *) NULL);
#endif				/* TCLOBJ */
    }
#endif /* WITH_CGRAPH */
    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	np = (Agnode_t **) tclhandleAlloc(ictx->nodeTblPtr, buf, &id);
	*np = n;
	AGID(n) = id;
#ifndef TCLOBJ
	Tcl_CreateCommand(interp, buf, nodecmd,
			  (ClientData) ictx, (Tcl_CmdDeleteProc *) NULL);
#else				/* TCLOBJ */
	Tcl_CreateObjCommand(interp, buf, nodecmd,
			     (ClientData) gvc, (Tcl_CmdDeleteProc *) NULL);
#endif				/* TCLOBJ */
	for (e = agfstout(g, n); e; e = agnxtout(g, e)) {
	    ep = (Agedge_t **) tclhandleAlloc(ictx->edgeTblPtr, buf, &id);
	    *ep = e;
	    AGID(e) = id;
#ifndef TCLOBJ
	    Tcl_CreateCommand(interp, buf, edgecmd, (ClientData) ictx,
			      (Tcl_CmdDeleteProc *) NULL);
#else				/* TCLOBJ */
	    Tcl_CreateObjCommand(interp, buf, edgecmd, (ClientData) gvc,
				 (Tcl_CmdDeleteProc *) NULL);
#endif				/* TCLOBJ */
	}
    }
    return TCL_OK;
}

#ifdef WITH_CGRAPH
Agraph_t *agread_usergets (FILE * fp, int (*usergets)(void *chan, char *buf, int bufsize))
{
    Agraph_t* g;
    Agdisc_t disc;
    Agiodisc_t ioDisc;

    ioDisc.afread = usergets;
    ioDisc.putstr = AgIoDisc.putstr;
    ioDisc.flush = AgIoDisc.flush;

    disc.mem = &AgMemDisc;
    disc.id = &AgIdDisc;
    disc.io = &ioDisc;
    g = agread (fp, &disc);
    return g;
}
#endif

static int dotread(ClientData clientData, Tcl_Interp * interp,
#ifndef TCLOBJ
		   int argc, char *argv[]
#else				/* TCLOBJ */
		   int argc, Tcl_Obj * CONST objv[]
#endif				/* TCLOBJ */
    )
{
    Agraph_t *g;
    Tcl_Channel channel;
    int mode;
    ictx_t *ictx = (ictx_t *)clientData;

    if (argc < 2) {
	Tcl_AppendResult(interp, "wrong # args: should be \"",
			 argv[0], " fileHandle\"", NULL);
	return TCL_ERROR;
    }
    channel = Tcl_GetChannel(interp, argv[1], &mode);
    if (channel == NULL || !(mode & TCL_READABLE)) {
	Tcl_AppendResult(interp, "\nChannel \"", argv[1], "\"", "is unreadable.", NULL);
	return TCL_ERROR;
    }
    /*
     * read a graph from the channel, the channel is left open
     *   ready to read the first line after the last line of
     *   a properly parsed graph. If the graph doesn't parse
     *   during reading then the channel will be left at EOF
     */
    g = agread_usergets((FILE *) channel, (mygets));
    if (!g) {
	Tcl_AppendResult(interp, "\nFailure to read graph \"", argv[1], "\"", NULL);
	if (agerrors()) {
	    Tcl_AppendResult(interp, " because of syntax errors.", NULL);
	}
	return TCL_ERROR;
    }
    if (agerrors()) {
	Tcl_AppendResult(interp, "\nSyntax errors in file \"", argv[1], " \"", NULL);
	return TCL_ERROR;
    }
    /* we use GD_drawing(g) as a flag that layout has been done.
     * so we make sure that it is initialized to "not done" */
    GD_drawing(g) = NULL;

    return (tcldot_fixup(ictx, g));
}

static int dotstring(ClientData clientData, Tcl_Interp * interp,
#ifndef TCLOBJ
		     int argc, char *argv[]
#else				/* TCLOBJ */
		     int argc, Tcl_Obj * CONST objv[]
#endif				/* TCLOBJ */
    )
{
    Agraph_t *g;
    ictx_t *ictx = (ictx_t *)clientData;

    if (argc < 2) {
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0], " string\"", NULL);
	return TCL_ERROR;
    }
    if (!(g = agmemread(argv[1]))) {
	Tcl_AppendResult(interp, "\nFailure to read string \"", argv[1], "\"", NULL);
	if (agerrors()) {
	    Tcl_AppendResult(interp, " because of syntax errors.", NULL);
	}
	return TCL_ERROR;
    }
    if (agerrors()) {
	Tcl_AppendResult(interp, "\nSyntax errors in string \"", argv[1], " \"", NULL);
	return TCL_ERROR;
    }
    /* we use GD_drawing(g) as a flag that layout has been done.
     * so we make sure that it is initialized to "not done" */
    GD_drawing(g) = NULL;

    return (tcldot_fixup(ictx, g));
}

#if defined(_BLD_tcldot) && defined(_DLL)
__EXPORT__
#endif
int Tcldot_Init(Tcl_Interp * interp)
{
    ictx_t *ictx;
    GVC_t *gvc;

    ictx = calloc(1, sizeof(ictx_t));
    if (!ictx)
	return TCL_ERROR;

    ictx->interp = interp;
#ifdef WITH_CGRAPH    
    ictx->mydisc.id = &myiddisc;
    ictx->ctr = 1;  /* init to first odd number,  increment by 2 */
#endif

#ifdef USE_TCL_STUBS
    if (Tcl_InitStubs(interp, TCL_VERSION, 0) == NULL) {
	return TCL_ERROR;
    }
#else
    if (Tcl_PkgRequire(interp, "Tcl", TCL_VERSION, 0) == NULL) {
	return TCL_ERROR;
    }
#endif
    if (Tcl_PkgProvide(interp, "Tcldot", VERSION) != TCL_OK) {
	return TCL_ERROR;
    }

#if HAVE_LIBGD
    Gdtclft_Init(interp);
#endif

#ifdef WITH_CGRAPH
    /* set persistent attributes here */
    agattr(NULL, AGNODE, "label", NODENAME_ESC);
#else
    aginit();
    agsetiodisc(NULL, gvfwrite, gvferror);
    /* set persistent attributes here */
    agnodeattr(NULL, "label", NODENAME_ESC);
#endif

    /* create a GraphViz Context and pass a pointer to it in clientdata */
    gvc = gvNEWcontext(lt_preloaded_symbols, DEMAND_LOADING);
    ictx->gvc = gvc;

    /* configure for available plugins */
    gvconfig(gvc, FALSE);

#ifndef TCLOBJ
    Tcl_CreateCommand(interp, "dotnew", dotnew,
		      (ClientData) ictx, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateCommand(interp, "dotread", dotread,
		      (ClientData) ictx, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateCommand(interp, "dotstring", dotstring,
		      (ClientData) ictx, (Tcl_CmdDeleteProc *) NULL);
#else				/* TCLOBJ */
    Tcl_CreateObjCommand(interp, "dotnew", dotnew,
			 (ClientData) ictx, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "dotread", dotread,
			 (ClientData) ictx, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "dotstring", dotstring,
			 (ClientData) ictx, (Tcl_CmdDeleteProc *) NULL);
#endif				/* TCLOBJ */

#ifdef WITH_CGRAPH  
    ictx->graphTblPtr = (void *) tclhandleInit("tcldot", sizeof(gctx_t *), 10);
#else
    ictx->graphTblPtr = (void *) tclhandleInit("graph", sizeof(Agraph_t *), 10);
    ictx->nodeTblPtr = (void *) tclhandleInit("node", sizeof(Agnode_t *), 100);
    ictx->edgeTblPtr = (void *) tclhandleInit("edge", sizeof(Agedge_t *), 100);
#endif

    return TCL_OK;
}

int Tcldot_SafeInit(Tcl_Interp * interp)
{
    return Tcldot_Init(interp);
}

int Tcldot_builtin_Init(Tcl_Interp * interp)
{
    return Tcldot_Init(interp);
}
