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


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

/* avoid compiler warnings with template changes in Tcl8.4 */
/*    specifically just the change to Tcl_CmdProc */
#define USE_NON_CONST

#include "tcldot.h"
#include "tclhandle.h"
#include "neatoprocs.h"
#include "circle.h"
#include "fdp.h"
#include "circo.h"
#include "gvc.h"

#ifdef DMALLOC
#include "dmalloc.h"
#endif

char *Info[] = {
    "tcldot",			/* Program */
    VERSION,			/* Version */
    BUILDDATE			/* Build Date */
};

/* ******* not ready yet
#if (TCL_MAJOR_VERSION > 7)
#define TCLOBJ
#if (TCL_MAJOR_VERSION == 8) && (TCL_MINOR_VERSION == 0)
char *
Tcl_GetString(Tcl_Obj *obj) {
    int len;
	return (Tcl_GetStringFromObj(obj, &len));
}
#else
#define UTF8
#endif
#endif
********* */

#if defined(_BLD_tcldot) && defined(_DLL)
extern codegen_t TK_CodeGen;
#else
extern codegen_t memGD_CodeGen, TK_CodeGen;
#if ENABLE_CODEGENS
extern FILE *Output_file;
extern int Output_lang;
#endif
#endif
extern void *GDHandleTable;
extern int Gdtclft_Init();

static void *graphTblPtr, *nodeTblPtr, *edgeTblPtr;
static tkgendata_t tkgendata;

static void reset_layout(Agraph_t * sg)
{
    Agraph_t *g = sg->root;

    if (GD_drawing(g)) {	/* only cleanup once between layouts */
	switch (GD_drawing(g)->engine) {
	case DOT:
	    dot_cleanup(g);
	    break;
	case NEATO:
	    neato_cleanup(g);
	    break;
	case TWOPI:
	    twopi_cleanup(g);
	    break;
	case FDP:
	    fdp_cleanup(g);
	    break;
	case CIRCULAR:
	    circo_cleanup(g);
	    break;
	default:
	    break;
	}
	GD_drawing(g) = NULL;
    }
}

static void deleteEdges(Tcl_Interp * interp, Agraph_t * g, Agnode_t * n)
{
    Agedge_t **ep, *e, *e1;
    char buf[16];

    e = agfstedge(g, n);
    while (e) {
	tclhandleString(edgeTblPtr, buf, e->handle);
	Tcl_DeleteCommand(interp, buf);
	ep = (Agedge_t **) tclhandleXlateIndex(edgeTblPtr, e->handle);
	if (!ep)
	    fprintf(stderr, "Bad entry in edgeTbl\n");
	tclhandleFreeIndex(edgeTblPtr, e->handle);
	e1 = agnxtedge(g, e, n);
	agdelete(g->root, e);
	e = e1;
    }
}

static void deleteNodes(Tcl_Interp * interp, Agraph_t * g)
{
    Agnode_t **np, *n, *n1;
    char buf[16];

    n = agfstnode(g);
    while (n) {
	tclhandleString(nodeTblPtr, buf, n->handle);
	Tcl_DeleteCommand(interp, buf);
	np = (Agnode_t **) tclhandleXlateIndex(nodeTblPtr, n->handle);
	if (!np)
	    fprintf(stderr, "Bad entry in nodeTbl\n");
	tclhandleFreeIndex(nodeTblPtr, n->handle);
	deleteEdges(interp, g->root, n);
	n1 = agnxtnode(g, n);
	agdelete(g->root, n);
	n = n1;
    }
}

static void deleteGraph(Tcl_Interp * interp, Agraph_t * g)
{
    Agraph_t **sgp;
    Agedge_t *e;
    char buf[16];

    if (g->meta_node) {
	for (e = agfstout(g->meta_node->graph, g->meta_node); e;
	     e = agnxtout(g->meta_node->graph, e)) {
	    deleteGraph(interp, agusergraph(e->head));
	}
	tclhandleString(graphTblPtr, buf, g->handle);
	Tcl_DeleteCommand(interp, buf);
	sgp = (Agraph_t **) tclhandleXlateIndex(graphTblPtr, g->handle);
	if (!sgp)
	    fprintf(stderr, "Bad entry in graphTbl\n");
	tclhandleFreeIndex(graphTblPtr, g->handle);
	if (g == g->root) {
	    agclose(g);
	} else {
	    agdelete(g->meta_node->graph, g->meta_node);
	}
    } else {
	fprintf(stderr, "Subgraph has no meta_node\n");
    }
}

static void setgraphattributes(Agraph_t * g, char *argv[], int argc)
{
    int i;
    Agsym_t *a;

    for (i = 0; i < argc; i++) {
	if (!(a = agfindattr(g->root, argv[i])))
	    a = agraphattr(g->root, argv[i], "");
	agxset(g, a->index, argv[++i]);
    }
}

static void
setedgeattributes(Agraph_t * g, Agedge_t * e, char *argv[], int argc)
{
    int i;
    Agsym_t *a;

    for (i = 0; i < argc; i++) {
	/* silently ignore attempts to modify "key" */
	if (strcmp(argv[i], "key") == 0) {
	    i++;
	    continue;
	}
	if (!(a = agfindattr(g->proto->e, argv[i])))
	    a = agedgeattr(g->root, argv[i], "");
	agxset(e, a->index, argv[++i]);
    }
}

static void
setnodeattributes(Agraph_t * g, Agnode_t * n, char *argv[], int argc)
{
    int i;
    Agsym_t *a;

    for (i = 0; i < argc; i++) {
	if (!(a = agfindattr(g->proto->n, argv[i])))
	    a = agnodeattr(g->root, argv[i], "");
	agxset(n, a->index, argv[++i]);
    }
}

static int edgecmd(ClientData clientData, Tcl_Interp * interp,
#ifndef TCLOBJ
		   int argc, char *argv[]
#else				/* TCLOBJ */
		   int argc, Tcl_Obj * CONST objv[]
#endif				/* TCLOBJ */
    )
{
    char c, buf[16], *s, **argv2;
    int i, j, length, argc2;
    Agraph_t *g;
    Agedge_t **ep, *e;
    Agsym_t *a;
#if 0				/* not used */
    GVC_t *gvc = (GVC_t *) clientData;
#endif

    if (argc < 2) {
	Tcl_AppendResult(interp, "wrong # args: should be \"",
			 argv[0], "\" option ?arg arg ...?",
			 (char *) NULL);
	return TCL_ERROR;
    }
    if (!(ep = (Agedge_t **) tclhandleXlate(edgeTblPtr, argv[0]))) {
	Tcl_AppendResult(interp, " \"", argv[0], "\"", (char *) NULL);
	return TCL_ERROR;
    }
    e = *ep;
    g = e->tail->graph;

    c = argv[1][0];
    length = strlen(argv[1]);

    if ((c == 'd') && (strncmp(argv[1], "delete", length) == 0)) {
	tclhandleFreeIndex(edgeTblPtr, e->handle);
	Tcl_DeleteCommand(interp, argv[0]);
	agdelete(g, e);
	reset_layout(g);
	return TCL_OK;

    } else if ((c == 'l')
	       && (strncmp(argv[1], "listattributes", length) == 0)) {
	for (i = 0; i < dtsize(g->univ->edgeattr->dict); i++) {
	    a = g->univ->edgeattr->list[i];
	    Tcl_AppendElement(interp, a->name);
	}
	return TCL_OK;

    } else if ((c == 'l') && (strncmp(argv[1], "listnodes", length) == 0)) {
	tclhandleString(nodeTblPtr, buf, e->tail->handle);
	Tcl_AppendElement(interp, buf);
	tclhandleString(nodeTblPtr, buf, e->head->handle);
	Tcl_AppendElement(interp, buf);
	return TCL_OK;

    } else if ((c == 'q')
	       && (strncmp(argv[1], "queryattributes", length) == 0)) {
	for (i = 2; i < argc; i++) {
	    if (Tcl_SplitList
		(interp, argv[i], &argc2,
		 (CONST84 char ***) &argv2) != TCL_OK)
		return TCL_ERROR;
	    for (j = 0; j < argc2; j++) {
		if ((a = agfindattr(g->proto->e, argv2[j]))) {
		    Tcl_AppendElement(interp, agxget(e, a->index));
		} else {
		    Tcl_AppendResult(interp, " No attribute named \"",
				     argv2[j], "\"", (char *) 0);
		    return TCL_ERROR;
		}
	    }
	    Tcl_Free((char *) argv2);
	}
	return TCL_OK;

    } else if ((c == 'q')
	       && (strncmp(argv[1], "queryattributevalues", length) ==
		   0)) {
	for (i = 2; i < argc; i++) {
	    if (Tcl_SplitList
		(interp, argv[i], &argc2,
		 (CONST84 char ***) &argv2) != TCL_OK)
		return TCL_ERROR;
	    for (j = 0; j < argc2; j++) {
		if ((a = agfindattr(g->proto->e, argv2[j]))) {
		    Tcl_AppendElement(interp, argv2[j]);
		    Tcl_AppendElement(interp, agxget(e, a->index));
		} else {
		    Tcl_AppendResult(interp, " No attribute named \"",
				     argv2[j], "\"", (char *) 0);
		    return TCL_ERROR;
		}
	    }
	    Tcl_Free((char *) argv2);
	}
	return TCL_OK;

    } else if ((c == 's')
	       && (strncmp(argv[1], "setattributes", length) == 0)) {
	if (argc == 3) {
	    if (Tcl_SplitList
		(interp, argv[2], &argc2,
		 (CONST84 char ***) &argv2) != TCL_OK)
		return TCL_ERROR;
	    if ((argc2 == 0) || (argc2 % 2)) {
		Tcl_AppendResult(interp, "wrong # args: should be \"",
				 argv[0],
				 "\" setattributes attributename attributevalue ?attributename attributevalue? ?...?",
				 (char *) NULL);
		Tcl_Free((char *) argv2);
		return TCL_ERROR;
	    }
	    setedgeattributes(g->root, e, argv2, argc2);
	    Tcl_Free((char *) argv2);
	} else {
	    if ((argc < 4) || (argc % 2)) {
		Tcl_AppendResult(interp, "wrong # args: should be \"",
				 argv[0],
				 "\" setattributes attributename attributevalue ?attributename attributevalue? ?...?",
				 (char *) NULL);
		return TCL_ERROR;
	    }
	    setedgeattributes(g->root, e, &argv[2], argc - 2);
	}
	reset_layout(g);
	return TCL_OK;

    } else if ((c == 's') && (strncmp(argv[1], "showname", length) == 0)) {
	if (g->kind && AGFLAG_DIRECTED)
	    s = "->";
	else
	    s = "--";
	Tcl_AppendResult(interp,
			 e->tail->name, s, e->head->name, (char *) NULL);
	return TCL_OK;

    } else {
	Tcl_AppendResult(interp, "bad option \"", argv[1],
			 "\": must be one of:",
			 "\n\tdelete, listattributes, listnodes,",
			 "\n\tueryattributes, queryattributevalues,",
			 "\n\tsetattributes, showname", (char *) NULL);
	return TCL_ERROR;
    }
}

static int nodecmd(ClientData clientData, Tcl_Interp * interp,
#ifndef TCLOBJ
		   int argc, char *argv[]
#else				/* TCLOBJ */
		   int argc, Tcl_Obj * CONST objv[]
#endif				/* TCLOBJ */
    )
{
    unsigned long id;
    char c, buf[16], **argv2;
    int i, j, length, argc2;
    Agraph_t *g;
    Agnode_t **np, *n, *head;
    Agedge_t **ep, *e;
    Agsym_t *a;
    GVC_t *gvc = (GVC_t *) clientData;

    if (argc < 2) {
	Tcl_AppendResult(interp, "wrong # args: should be \"",
			 argv[0], " option ?arg arg ...?\"",
			 (char *) NULL);
	return TCL_ERROR;
    }
    if (!(np = (Agnode_t **) tclhandleXlate(nodeTblPtr, argv[0]))) {
	Tcl_AppendResult(interp, " \"", argv[0], "\"", (char *) NULL);
	return TCL_ERROR;
    }
    n = *np;
    g = n->graph;

    c = argv[1][0];
    length = strlen(argv[1]);


    if ((c == 'a') && (strncmp(argv[1], "addedge", length) == 0)) {
	if ((argc < 3) || (!(argc % 2))) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
			     argv[0],
			     " addedge head ?attributename attributevalue? ?...?\"",
			     (char *) NULL);
	    return TCL_ERROR;
	}
	if (!(np = (Agnode_t **) tclhandleXlate(nodeTblPtr, argv[2]))) {
	    if (!(head = agfindnode(g, argv[2]))) {
		Tcl_AppendResult(interp, "Head node \"", argv[2],
				 "\" not found.", (char *) NULL);
		return TCL_ERROR;
	    }
	} else {
	    head = *np;
	    if (g->root != head->graph->root) {
		Tcl_AppendResult(interp, "Nodes ", argv[0], " and ",
				 argv[2], " are not in the same graph.",
				 (char *) NULL);
		return TCL_ERROR;
	    }
	}
	e = agedge(g, n, head);
	if (!
	    (ep = (Agedge_t **) tclhandleXlateIndex(edgeTblPtr, e->handle))
	    || *ep != e) {
	    ep = (Agedge_t **) tclhandleAlloc(edgeTblPtr, interp->result,
					      &id);
	    *ep = e;
	    e->handle = id;
#ifndef TCLOBJ
	    Tcl_CreateCommand(interp, interp->result, edgecmd,
			      (ClientData) gvc,
			      (Tcl_CmdDeleteProc *) NULL);
#else				/* TCLOBJ */
	    Tcl_CreateObjCommand(interp, interp->result, edgecmd,
				 (ClientData) gvc,
				 (Tcl_CmdDeleteProc *) NULL);
#endif				/* TCLOBJ */
	} else {
	    tclhandleString(edgeTblPtr, interp->result, e->handle);
	}
	setedgeattributes(g->root, e, &argv[3], argc - 3);
	reset_layout(g);
	return TCL_OK;

    } else if ((c == 'd') && (strncmp(argv[1], "delete", length) == 0)) {
	deleteEdges(interp, g, n);
	tclhandleFreeIndex(nodeTblPtr, n->handle);
	Tcl_DeleteCommand(interp, argv[0]);
	agdelete(g, n);
	reset_layout(g);
	return TCL_OK;

    } else if ((c == 'f') && (strncmp(argv[1], "findedge", length) == 0)) {
	if (argc < 3) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
			     argv[0], " findedge headnodename\"",
			     (char *) NULL);
	    return TCL_ERROR;
	}
	if (!(head = agfindnode(g, argv[2]))) {
	    Tcl_AppendResult(interp, "Head node \"", argv[2],
			     "\" not found.", (char *) NULL);
	    return TCL_ERROR;
	}
	if (!(e = agfindedge(g, n, head))) {
	    tclhandleString(nodeTblPtr, buf, head->handle);
	    Tcl_AppendResult(interp, "Edge \"", argv[0],
			     " - ", buf, "\" not found.", (char *) NULL);
	    return TCL_ERROR;
	}
	tclhandleString(edgeTblPtr, buf, e->handle);
	Tcl_AppendElement(interp, buf);
	return TCL_OK;

    } else if ((c == 'l')
	       && (strncmp(argv[1], "listattributes", length) == 0)) {
	for (i = 0; i < dtsize(g->univ->nodeattr->dict); i++) {
	    a = g->univ->nodeattr->list[i];
	    Tcl_AppendElement(interp, a->name);
	}
	return TCL_OK;

    } else if ((c == 'l') && (strncmp(argv[1], "listedges", length) == 0)) {
	for (e = agfstedge(g, n); e; e = agnxtedge(g, e, n)) {
	    tclhandleString(edgeTblPtr, buf, e->handle);
	    Tcl_AppendElement(interp, buf);
	}
	return TCL_OK;

    } else if ((c == 'l')
	       && (strncmp(argv[1], "listinedges", length) == 0)) {
	for (e = agfstin(g, n); e; e = agnxtin(g, e)) {
	    tclhandleString(edgeTblPtr, buf, e->handle);
	    Tcl_AppendElement(interp, buf);
	}
	return TCL_OK;

    } else if ((c == 'l')
	       && (strncmp(argv[1], "listoutedges", length) == 0)) {
	for (e = agfstout(g, n); e; e = agnxtout(g, e)) {
	    tclhandleString(edgeTblPtr, buf, e->handle);
	    Tcl_AppendElement(interp, buf);
	}
	return TCL_OK;

    } else if ((c == 'q')
	       && (strncmp(argv[1], "queryattributes", length) == 0)) {
	for (i = 2; i < argc; i++) {
	    if (Tcl_SplitList
		(interp, argv[i], &argc2,
		 (CONST84 char ***) &argv2) != TCL_OK)
		return TCL_ERROR;
	    for (j = 0; j < argc2; j++) {
		if ((a = agfindattr(g->proto->n, argv2[j]))) {
		    Tcl_AppendElement(interp, agxget(n, a->index));
		} else {
		    Tcl_AppendResult(interp, " No attribute named \"",
				     argv2[j], "\"", (char *) 0);
		    return TCL_ERROR;
		}
	    }
	    Tcl_Free((char *) argv2);
	}
	return TCL_OK;

    } else if ((c == 'q')
	       && (strncmp(argv[1], "queryattributevalues", length) ==
		   0)) {
	for (i = 2; i < argc; i++) {
	    if (Tcl_SplitList
		(interp, argv[i], &argc2,
		 (CONST84 char ***) &argv2) != TCL_OK)
		return TCL_ERROR;
	    for (j = 0; j < argc2; j++) {
		if ((a = agfindattr(g->proto->n, argv2[j]))) {
		    Tcl_AppendElement(interp, argv2[j]);
		    Tcl_AppendElement(interp, agxget(n, a->index));
		} else {
		    Tcl_AppendResult(interp, " No attribute named \"",
				     argv2[j], "\"", (char *) 0);
		    return TCL_ERROR;
		}
	    }
	    Tcl_Free((char *) argv2);
	}
	return TCL_OK;

    } else if ((c == 's')
	       && (strncmp(argv[1], "setattributes", length) == 0)) {
	g = g->root;
	if (argc == 3) {
	    if (Tcl_SplitList
		(interp, argv[2], &argc2,
		 (CONST84 char ***) &argv2) != TCL_OK)
		return TCL_ERROR;
	    if ((argc2 == 0) || (argc2 % 2)) {
		Tcl_AppendResult(interp, "wrong # args: should be \"",
				 argv[0],
				 "\" setattributes attributename attributevalue ?attributename attributevalue? ?...?",
				 (char *) NULL);
		Tcl_Free((char *) argv2);
		return TCL_ERROR;
	    }
	    setnodeattributes(g, n, argv2, argc2);
	    Tcl_Free((char *) argv2);
	} else {
	    if ((argc < 4) || (argc % 2)) {
		Tcl_AppendResult(interp, "wrong # args: should be \"",
				 argv[0],
				 "\" setattributes attributename attributevalue ?attributename attributevalue? ?...?",
				 (char *) NULL);
		return TCL_ERROR;
	    }
	    setnodeattributes(g, n, &argv[2], argc - 2);
	}
	reset_layout(g);
	return TCL_OK;

    } else if ((c == 's') && (strncmp(argv[1], "showname", length) == 0)) {
	interp->result = n->name;
	return TCL_OK;

    } else {
	Tcl_AppendResult(interp, "bad option \"", argv[1],
			 "\": must be one of:",
			 "\n\taddedge, listattributes, listedges, listinedges,",
			 "\n\tlistoutedges, queryattributes, queryattributevalues,",
			 "\n\tsetattributes, showname.", (char *) NULL);
	return TCL_ERROR;
    }
}

static void tcldot_layout(Agraph_t * g, char *engine)
{
    char buf[256];
    Agsym_t *a;

    reset_layout(g);		/* in case previously drawn */

    if (engine && strcasecmp(engine, "dot") == 0)
	dot_layout(g);
    else if (engine && strcasecmp(engine, "neato") == 0)
	neato_layout(g);
    else if (engine && strcasecmp(engine, "nop") == 0) {
	Nop = 2;
	PSinputscale = POINTS_PER_INCH;
	neato_layout(g);
    } else if (engine && strcasecmp(engine, "twopi") == 0)
	twopi_layout(g);
    else if (engine && strcasecmp(engine, "fdp") == 0)
	fdp_layout(g);
    else if (engine && strcasecmp(engine, "circo") == 0)
	circo_layout(g);
/* support old behaviors if engine isn't specified*/
    else if (AG_IS_DIRECTED(g))
	dot_layout(g);
    else
	neato_layout(g);

/* set bb attribute for basic layout.
 * doesn't yet include margins, scaling or page sizes because
 * those depend on the renderer being used. */
    if (GD_drawing(g)->landscape)
	sprintf(buf, "%d %d %d %d",
		ROUND(GD_bb(g).LL.y), ROUND(GD_bb(g).LL.x),
		ROUND(GD_bb(g).UR.y), ROUND(GD_bb(g).UR.x));
    else
	sprintf(buf, "%d %d %d %d",
		ROUND(GD_bb(g).LL.x), ROUND(GD_bb(g).LL.y),
		ROUND(GD_bb(g).UR.x), ROUND(GD_bb(g).UR.y));
    if (!(a = agfindattr(g, "bb"))) {
	a = agraphattr(g, "bb", "");
    }
    agxset(g, a->index, buf);
}

static int graphcmd(ClientData clientData, Tcl_Interp * interp,
#ifndef TCLOBJ
		    int argc, char *argv[]
#else				/* TCLOBJ */
		    int argc, Tcl_Obj * CONST objv[]
#endif				/* TCLOBJ */
    )
{

    Agraph_t *g, **gp, *sg, **sgp;
    Agnode_t **np, *n, *tail, *head;
    Agedge_t **ep, *e;
    Agsym_t *a;
    char c, buf[256], **argv2;
    int i, j, length, argc2;
    unsigned long id;
    GVC_t *gvc = (GVC_t *) clientData;

    if (argc < 2) {
	Tcl_AppendResult(interp, "wrong # args: should be \"",
			 argv[0], " option ?arg arg ...?\"",
			 (char *) NULL);
	return TCL_ERROR;
    }
    if (!(gp = (Agraph_t **) tclhandleXlate(graphTblPtr, argv[0]))) {
	Tcl_AppendResult(interp, " \"", argv[0], "\"", (char *) NULL);
	return TCL_ERROR;
    }

    g = *gp;

    c = argv[1][0];
    length = strlen(argv[1]);

    if ((c == 'a') && (strncmp(argv[1], "addedge", length) == 0)) {
	if ((argc < 4) || (argc % 2)) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
			     " addedge tail head ?attributename attributevalue? ?...?\"",
			     (char *) NULL);
	    return TCL_ERROR;
	}
	if (!(np = (Agnode_t **) tclhandleXlate(nodeTblPtr, argv[2]))) {
	    if (!(tail = agfindnode(g, argv[2]))) {
		Tcl_AppendResult(interp, "Tail node \"", argv[2],
				 "\" not found.", (char *) NULL);
		return TCL_ERROR;
	    }
	} else {
	    tail = *np;
	    if (g->root != tail->graph->root) {
		Tcl_AppendResult(interp, "Node ", argv[2],
				 " is not in the graph.", (char *) NULL);
		return TCL_ERROR;
	    }
	}
	if (!(np = (Agnode_t **) tclhandleXlate(nodeTblPtr, argv[3]))) {
	    if (!(head = agfindnode(g, argv[3]))) {
		Tcl_AppendResult(interp, "Head node \"", argv[3],
				 "\" not found.", (char *) NULL);
		return TCL_ERROR;
	    }
	} else {
	    head = *np;
	    if (g->root != head->graph->root) {
		Tcl_AppendResult(interp, "Node ", argv[3],
				 " is not in the graph.", (char *) NULL);
		return TCL_ERROR;
	    }
	}
	e = agedge(g, tail, head);
	if (!
	    (ep = (Agedge_t **) tclhandleXlateIndex(edgeTblPtr, e->handle))
	    || *ep != e) {
	    ep = (Agedge_t **) tclhandleAlloc(edgeTblPtr, interp->result,
					      &id);
	    *ep = e;
	    e->handle = id;
#ifndef TCLOBJ
	    Tcl_CreateCommand(interp, interp->result, edgecmd,
			      (ClientData) gvc,
			      (Tcl_CmdDeleteProc *) NULL);
#else				/* TCLOBJ */
	    Tcl_CreateObjCommand(interp, interp->result, edgecmd,
				 (ClientData) gvc,
				 (Tcl_CmdDeleteProc *) NULL);
#endif				/* TCLOBJ */
	} else {
	    tclhandleString(edgeTblPtr, interp->result, e->handle);
	}
	setedgeattributes(g->root, e, &argv[4], argc - 4);
	reset_layout(g);
	return TCL_OK;

    } else if ((c == 'a') && (strncmp(argv[1], "addnode", length) == 0)) {
	if (argc % 2) {
	    /* if odd number of args then argv[2] is name */
	    n = agnode(g, argv[2]);
	    i = 3;
	    if (!
		(np =
		 (Agnode_t **) tclhandleXlateIndex(nodeTblPtr, n->handle))
		|| *np != n) {
		np = (Agnode_t **) tclhandleAlloc(nodeTblPtr,
						  interp->result, &id);
		*np = n;
		n->handle = id;
#ifndef TCLOBJ
		Tcl_CreateCommand(interp, interp->result, nodecmd,
				  (ClientData) gvc,
				  (Tcl_CmdDeleteProc *) NULL);
#else				/* TCLOBJ */
		Tcl_CreateObjCommand(interp, interp->result, nodecmd,
				     (ClientData) gvc,
				     (Tcl_CmdDeleteProc *) NULL);
#endif				/* TCLOBJ */
	    } else {
		tclhandleString(nodeTblPtr, interp->result, n->handle);
	    }
	} else {
	    /* else use handle as name */
	    np = (Agnode_t **) tclhandleAlloc(nodeTblPtr, interp->result,
					      &id);
	    n = agnode(g, interp->result);
	    i = 2;
	    *np = n;
	    n->handle = id;
#ifndef TCLOBJ
	    Tcl_CreateCommand(interp, interp->result, nodecmd,
			      (ClientData) gvc,
			      (Tcl_CmdDeleteProc *) NULL);
#else				/* TCLOBJ */
	    Tcl_CreateObjCommand(interp, interp->result, nodecmd,
				 (ClientData) gvc,
				 (Tcl_CmdDeleteProc *) NULL);
#endif				/* TCLOBJ */
	}
	setnodeattributes(g->root, n, &argv[i], argc - i);
	reset_layout(g);
	return TCL_OK;

    } else if ((c == 'a')
	       && (strncmp(argv[1], "addsubgraph", length) == 0)) {
	if (argc < 2) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
			     "\" addsubgraph ?name? ?attributename attributevalue? ?...?",
			     (char *) NULL);
	}
	if (argc % 2) {
	    /* if odd number of args then argv[2] is name */
	    sg = agsubg(g, argv[2]);
	    i = 3;
	    if (!
		(sgp =
		 (Agraph_t **) tclhandleXlateIndex(graphTblPtr,
						   sg->handle))
		|| *sgp != sg) {
		sgp =
		    (Agraph_t **) tclhandleAlloc(graphTblPtr,
						 interp->result, &id);
		*sgp = sg;
		sg->handle = id;
#ifndef TCLOBJ
		Tcl_CreateCommand(interp, interp->result, graphcmd,
				  (ClientData) gvc,
				  (Tcl_CmdDeleteProc *) NULL);
#else				/* TCLOBJ */
		Tcl_CreateObjCommand(interp, interp->result, graphcmd,
				     (ClientData) gvc,
				     (Tcl_CmdDeleteProc *) NULL);
#endif				/* TCLOBJ */
	    } else {
		tclhandleString(graphTblPtr, interp->result, sg->handle);
	    }
	} else {
	    /* else use handle as name */
	    sgp =
		(Agraph_t **) tclhandleAlloc(graphTblPtr, interp->result,
					     &id);
	    sg = agsubg(g, interp->result);
	    i = 2;
	    *sgp = sg;
	    sg->handle = id;
#ifndef TCLOBJ
	    Tcl_CreateCommand(interp, interp->result, graphcmd,
			      (ClientData) gvc,
			      (Tcl_CmdDeleteProc *) NULL);
#else				/* TCLOBJ */
	    Tcl_CreateObjCommand(interp, interp->result, graphcmd,
				 (ClientData) gvc,
				 (Tcl_CmdDeleteProc *) NULL);
#endif				/* TCLOBJ */
	}
	setgraphattributes(sg, &argv[i], argc - i);
	reset_layout(g);
	return TCL_OK;

    } else if ((c == 'c') && (strncmp(argv[1], "countnodes", length) == 0)) {
	sprintf(buf, "%d", agnnodes(g));
	Tcl_AppendResult(interp, buf, (char *) NULL);
	return TCL_OK;

    } else if ((c == 'c') && (strncmp(argv[1], "countedges", length) == 0)) {
	sprintf(buf, "%d", agnedges(g));
	Tcl_AppendResult(interp, buf, (char *) NULL);
	return TCL_OK;

    } else if ((c == 'd') && (strncmp(argv[1], "delete", length) == 0)) {
	reset_layout(g);
	deleteNodes(interp, g);
	deleteGraph(interp, g);
	return TCL_OK;

    } else if ((c == 'f') && (strncmp(argv[1], "findedge", length) == 0)) {
	if (argc < 4) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
			     argv[0],
			     " findedge tailnodename headnodename\"",
			     (char *) NULL);
	    return TCL_ERROR;
	}
	if (!(tail = agfindnode(g, argv[2]))) {
	    Tcl_AppendResult(interp, "Tail node \"", argv[2],
			     "\" not found.", (char *) NULL);
	    return TCL_ERROR;
	}
	if (!(head = agfindnode(g, argv[3]))) {
	    Tcl_AppendResult(interp, "Head node \"", argv[3],
			     "\" not found.", (char *) NULL);
	    return TCL_ERROR;
	}
	if (!(e = agfindedge(g, tail, head))) {
	    Tcl_AppendResult(interp, "Edge \"", argv[2],
			     " - ", argv[3], "\" not found.",
			     (char *) NULL);
	    return TCL_ERROR;
	}
	tclhandleString(edgeTblPtr, buf, e->handle);
	Tcl_AppendElement(interp, buf);
	return TCL_OK;

    } else if ((c == 'f') && (strncmp(argv[1], "findnode", length) == 0)) {
	if (argc < 3) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
			     argv[0], " findnode nodename\"",
			     (char *) NULL);
	    return TCL_ERROR;
	}
	if (!(n = agfindnode(g, argv[2]))) {
	    Tcl_AppendResult(interp, "Node not found.", (char *) NULL);
	    return TCL_ERROR;
	}
	tclhandleString(nodeTblPtr, buf, n->handle);
	Tcl_AppendResult(interp, buf, (char *) NULL);
	return TCL_OK;

    } else if ((c == 'l')
	       && (strncmp(argv[1], "layoutedges", length) == 0)) {
	g = g->root;
	if (!GD_drawing(g))
	    tcldot_layout(g, (argc > 2) ? argv[2] : (char *) NULL);
	return TCL_OK;

    } else if ((c == 'l')
	       && (strncmp(argv[1], "layoutnodes", length) == 0)) {
	g = g->root;
	if (!GD_drawing(g))
	    tcldot_layout(g, (argc > 2) ? argv[2] : (char *) NULL);
	return TCL_OK;

    } else if ((c == 'l')
	       && (strncmp(argv[1], "listattributes", length) == 0)) {
	for (i = 0; i < dtsize(g->univ->globattr->dict); i++) {
	    a = g->univ->globattr->list[i];
	    Tcl_AppendElement(interp, a->name);
	}
	return TCL_OK;

    } else if ((c == 'l')
	       && (strncmp(argv[1], "listedgeattributes", length) == 0)) {
	for (i = 0; i < dtsize(g->univ->edgeattr->dict); i++) {
	    a = g->univ->edgeattr->list[i];
	    Tcl_AppendElement(interp, a->name);
	}
	return TCL_OK;

    } else if ((c == 'l')
	       && (strncmp(argv[1], "listnodeattributes", length) == 0)) {
	for (i = 0; i < dtsize(g->univ->nodeattr->dict); i++) {
	    a = g->univ->nodeattr->list[i];
	    Tcl_AppendElement(interp, a->name);
	}
	return TCL_OK;

    } else if ((c == 'l') && (strncmp(argv[1], "listedges", length) == 0)) {
	for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	    for (e = agfstout(g, n); e; e = agnxtout(g, e)) {
		tclhandleString(edgeTblPtr, buf, e->handle);
		Tcl_AppendElement(interp, buf);
	    }
	}
	return TCL_OK;

    } else if ((c == 'l') && (strncmp(argv[1], "listnodes", length) == 0)) {
	for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	    tclhandleString(nodeTblPtr, buf, n->handle);
	    Tcl_AppendElement(interp, buf);
	}
	return TCL_OK;

    } else if ((c == 'l')
	       && (strncmp(argv[1], "listnodesrev", length) == 0)) {
	for (n = aglstnode(g); n; n = agprvnode(g, n)) {
	    tclhandleString(nodeTblPtr, buf, n->handle);
	    Tcl_AppendElement(interp, buf);
	}
	return TCL_OK;

    } else if ((c == 'l')
	       && (strncmp(argv[1], "listsubgraphs", length) == 0)) {
	if (g->meta_node) {
	    for (e = agfstout(g->meta_node->graph, g->meta_node); e;
		 e = agnxtout(g->meta_node->graph, e)) {
		sg = agusergraph(e->head);
		tclhandleString(graphTblPtr, buf, sg->handle);
		Tcl_AppendElement(interp, buf);
	    }
	}
	return TCL_OK;

    } else if ((c == 'q')
	       && (strncmp(argv[1], "queryattributes", length) == 0)) {
	for (i = 2; i < argc; i++) {
	    if (Tcl_SplitList
		(interp, argv[i], &argc2,
		 (CONST84 char ***) &argv2) != TCL_OK)
		return TCL_ERROR;
	    for (j = 0; j < argc2; j++) {
		if ((a = agfindattr(g->root, argv2[j]))) {
		    Tcl_AppendElement(interp, agxget(g, a->index));
		} else {
		    Tcl_AppendResult(interp, " No attribute named \"",
				     argv2[j], "\"", (char *) 0);
		    return TCL_ERROR;
		}
	    }
	    Tcl_Free((char *) argv2);
	}
	return TCL_OK;

    } else if ((c == 'q')
	       && (strncmp(argv[1], "queryattributevalues", length) ==
		   0)) {
	for (i = 2; i < argc; i++) {
	    if (Tcl_SplitList
		(interp, argv[i], &argc2,
		 (CONST84 char ***) &argv2) != TCL_OK)
		return TCL_ERROR;
	    for (j = 0; j < argc2; j++) {
		if ((a = agfindattr(g->root, argv2[j]))) {
		    Tcl_AppendElement(interp, argv2[j]);
		    Tcl_AppendElement(interp, agxget(g, a->index));
		} else {
		    Tcl_AppendResult(interp, " No attribute named \"",
				     argv2[j], "\"", (char *) 0);
		    return TCL_ERROR;
		}
	    }
	    Tcl_Free((char *) argv2);
	}
	return TCL_OK;

    } else if ((c == 'q')
	       && (strncmp(argv[1], "queryedgeattributes", length) == 0)) {
	for (i = 2; i < argc; i++) {
	    if (Tcl_SplitList
		(interp, argv[i], &argc2,
		 (CONST84 char ***) &argv2) != TCL_OK)
		return TCL_ERROR;
	    for (j = 0; j < argc2; j++) {
		if ((a = agfindattr(g->proto->e, argv2[j]))) {
		    Tcl_AppendElement(interp,
				      agxget(g->proto->e, a->index));
		} else {
		    Tcl_AppendResult(interp, " No attribute named \"",
				     argv2[j], "\"", (char *) 0);
		    return TCL_ERROR;
		}
	    }
	    Tcl_Free((char *) argv2);
	}
	return TCL_OK;

    } else if ((c == 'q')
	       && (strncmp(argv[1], "queryedgeattributevalues", length) ==
		   0)) {
	for (i = 2; i < argc; i++) {
	    if (Tcl_SplitList
		(interp, argv[i], &argc2,
		 (CONST84 char ***) &argv2) != TCL_OK)
		return TCL_ERROR;
	    for (j = 0; j < argc2; j++) {
		if ((a = agfindattr(g->proto->e, argv2[j]))) {
		    Tcl_AppendElement(interp, argv2[j]);
		    Tcl_AppendElement(interp,
				      agxget(g->proto->e, a->index));
		} else {
		    Tcl_AppendResult(interp, " No attribute named \"",
				     argv2[j], "\"", (char *) 0);
		    return TCL_ERROR;
		}
	    }
	    Tcl_Free((char *) argv2);
	}
	return TCL_OK;

    } else if ((c == 'q')
	       && (strncmp(argv[1], "querynodeattributes", length) == 0)) {
	for (i = 2; i < argc; i++) {
	    if (Tcl_SplitList
		(interp, argv[i], &argc2,
		 (CONST84 char ***) &argv2) != TCL_OK)
		return TCL_ERROR;
	    for (j = 0; j < argc2; j++) {
		if ((a = agfindattr(g->proto->n, argv2[j]))) {
		    Tcl_AppendElement(interp,
				      agxget(g->proto->n, a->index));
		} else {
		    Tcl_AppendResult(interp, " No attribute named \"",
				     argv2[j], "\"", (char *) 0);
		    return TCL_ERROR;
		}
	    }
	    Tcl_Free((char *) argv2);
	}
	return TCL_OK;

    } else if ((c == 'q')
	       && (strncmp(argv[1], "querynodeattributevalues", length) ==
		   0)) {
	for (i = 2; i < argc; i++) {
	    if (Tcl_SplitList
		(interp, argv[i], &argc2,
		 (CONST84 char ***) &argv2) != TCL_OK)
		return TCL_ERROR;
	    for (j = 0; j < argc2; j++) {
		if ((a = agfindattr(g->proto->n, argv2[j]))) {
		    Tcl_AppendElement(interp, argv2[j]);
		    Tcl_AppendElement(interp,
				      agxget(g->proto->n, a->index));
		} else {
		    Tcl_AppendResult(interp, " No attribute named \"",
				     argv2[j], "\"", (char *) 0);
		    return TCL_ERROR;
		}
	    }
	    Tcl_Free((char *) argv2);
	}
	return TCL_OK;

    } else if ((c == 'r') && (strncmp(argv[1], "render", length) == 0)) {
	if (argc < 3) {
	    tkgendata.canvas = "$c";
	} else {
	    tkgendata.canvas = argv[2];
#if 0				/* not implemented */
	    if (argc < 4) {
		tkgendata.eval = FALSE;
	    } else {
		if ((Tcl_GetBoolean(interp, argv[3], &tkgendata.eval)) !=
		    TCL_OK) {
		    Tcl_AppendResult(interp, " Invalid boolean: \"",
				     argv[3], "\"", (char *) 0);
		    return TCL_ERROR;
		}
	    }
#endif
	}
	tkgendata.interp = interp;

	gvrender_output_langname_job(gvc, "TK");
	/* make sure that layout is done */
	gvc->g = g = g->root;
	if (!GD_drawing(g) || argc > 3)
	    tcldot_layout(g, (argc > 3) ? argv[3] : (char *) NULL);

	/* render graph TK canvas commands */
	gvc->job->output_lang =
	    gvrender_select(gvc, gvc->job->output_langname);
	gvc->job->output_file = (FILE *) & tkgendata;
#if ENABLE_CODEGENS
	Output_lang = gvc->job->output_lang;
	Output_file = gvc->job->output_file;
#endif
	gvc->job->codegen = &TK_CodeGen;
	/* emit graph in sorted order, all nodes then all edges */
	emit_graph(gvc, g, EMIT_SORTED);
	gvrender_delete_jobs(gvc);
	return TCL_OK;

    } else if ((c == 'r') && (strncmp(argv[1], "rendergd", length) == 0)) {
	if (argc < 3) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
			     " rendergd gdhandle ?DOT|NEATO|TWOPI|FDP|CIRCO?\"",
			     (char *) NULL);
	    return TCL_ERROR;
	}
	gvrender_output_langname_job(gvc, "memGD");

/* FIXME - nasty hack because memGD is not a language available from command line */
#if 0
	gvc->job->output_lang =
            gvrender_select(gvc, gvc->job->output_langname);

#else
	gvc->job->output_lang = memGD;
#endif

	if (!
	    (gvc->job->output_file =
	     (FILE *) tclhandleXlate(GDHandleTable, argv[2]))) {
	    Tcl_AppendResult(interp, "GD Image not found.", (char *) NULL);
	    return TCL_ERROR;
	}
#if ENABLE_CODEGENS
	Output_lang = gvc->job->output_lang;
	Output_file = gvc->job->output_file;
#endif

	/* make sure that layout is done */
	gvc->g = g = g->root;
	if (!GD_drawing(g) || argc > 4)
	    tcldot_layout(g, (argc > 4) ? argv[4] : (char *) NULL);

	/* set default margins for current output format */
	dotneato_set_margins(gvc, g);
	/* render graph to open GD structure */
	gvc->job->codegen = &memGD_CodeGen;
	/* emit graph in sorted order, all nodes then all edges */
	emit_graph(gvc, g, 1);
	gvrender_delete_jobs(gvc);
	Tcl_AppendResult(interp, argv[2], (char *) NULL);
	return TCL_OK;

    } else if ((c == 's')
	       && (strncmp(argv[1], "setattributes", length) == 0)) {
	if (argc == 3) {
	    if (Tcl_SplitList
		(interp, argv[2], &argc2,
		 (CONST84 char ***) &argv2) != TCL_OK)
		return TCL_ERROR;
	    if ((argc2 == 0) || (argc2 % 2)) {
		Tcl_AppendResult(interp, "wrong # args: should be \"",
				 argv[0],
				 "\" setattributes attributename attributevalue ?attributename attributevalue? ?...?",
				 (char *) NULL);
		Tcl_Free((char *) argv2);
		return TCL_ERROR;
	    }
	    setgraphattributes(g, argv2, argc2);
	    Tcl_Free((char *) argv2);
	    reset_layout(g);
	}
	if (argc == 4 && strcmp(argv[2], "viewport") == 0) {
	    /* special case to allow viewport to be set without resetting layout */
	    setgraphattributes(g, &argv[2], argc - 2);
	} else {
	    if ((argc < 4) || (argc % 2)) {
		Tcl_AppendResult(interp, "wrong # args: should be \"",
				 argv[0],
				 "\" setattributes attributename attributevalue ?attributename attributevalue? ?...?",
				 (char *) NULL);
		return TCL_ERROR;
	    }
	    setgraphattributes(g, &argv[2], argc - 2);
	    reset_layout(g);
	}
	return TCL_OK;

    } else if ((c == 's')
	       && (strncmp(argv[1], "setedgeattributes", length) == 0)) {
	if (argc == 3) {
	    if (Tcl_SplitList
		(interp, argv[2], &argc2,
		 (CONST84 char ***) &argv2) != TCL_OK)
		return TCL_ERROR;
	    if ((argc2 == 0) || (argc2 % 2)) {
		Tcl_AppendResult(interp, "wrong # args: should be \"",
				 argv[0],
				 "\" setedgeattributes attributename attributevalue ?attributename attributevalue? ?...?",
				 (char *) NULL);
		Tcl_Free((char *) argv2);
		return TCL_ERROR;
	    }
	    setedgeattributes(g, g->proto->e, argv2, argc2);
	    Tcl_Free((char *) argv2);
	} else {
	    if ((argc < 4) || (argc % 2)) {
		Tcl_AppendResult(interp, "wrong # args: should be \"",
				 argv[0],
				 "\" setedgeattributes attributename attributevalue ?attributename attributevalue? ?...?",
				 (char *) NULL);
	    }
	    setedgeattributes(g, g->proto->e, &argv[2], argc - 2);
	}
	reset_layout(g);
	return TCL_OK;

    } else if ((c == 's')
	       && (strncmp(argv[1], "setnodeattributes", length) == 0)) {
	if (argc == 3) {
	    if (Tcl_SplitList
		(interp, argv[2], &argc2,
		 (CONST84 char ***) &argv2) != TCL_OK)
		return TCL_ERROR;
	    if ((argc2 == 0) || (argc2 % 2)) {
		Tcl_AppendResult(interp, "wrong # args: should be \"",
				 argv[0],
				 "\" setnodeattributes attributename attributevalue ?attributename attributevalue? ?...?",
				 (char *) NULL);
		Tcl_Free((char *) argv2);
		return TCL_ERROR;
	    }
	    setnodeattributes(g, g->proto->n, argv2, argc2);
	    Tcl_Free((char *) argv2);
	} else {
	    if ((argc < 4) || (argc % 2)) {
		Tcl_AppendResult(interp, "wrong # args: should be \"",
				 argv[0],
				 "\" setnodeattributes attributename attributevalue ?attributename attributevalue? ?...?",
				 (char *) NULL);
	    }
	    setnodeattributes(g, g->proto->n, &argv[2], argc - 2);
	}
	reset_layout(g);
	return TCL_OK;

    } else if ((c == 's') && (strncmp(argv[1], "showname", length) == 0)) {
	interp->result = g->name;
	return TCL_OK;

    } else if ((c == 'w') && (strncmp(argv[1], "write", length) == 0)) {
	g = g->root;
	if (argc < 3) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
			     argv[0],
			     " write fileHandle ?language ?DOT|NEATO|TWOPI|FDP|CIRCO|NOP??\"",
			     (char *) NULL);
	    return TCL_ERROR;
	}
	/* process lang first to create job */
	if (argc < 4) {
	    gvrender_output_langname_job(gvc, "dot");
	} else {
	    gvrender_output_langname_job(gvc, argv[3]);
	}
	/* populate new job struct with output language and output file data */
	gvc->job->output_lang =
            gvrender_select(gvc, gvc->job->output_langname);
	if (Tcl_GetOpenFile
	    (interp, argv[2], 1, 1,
	     (ClientData *) & (gvc->job->output_file)) != TCL_OK)
	    return TCL_ERROR;
#if ENABLE_CODEGENS
	/* old codegens use globals */
	Output_lang = gvc->job->output_lang;
	Output_file = gvc->job->output_file;
#endif
	/* make sure that layout is done  - unless canonical output */
	if ((!GD_drawing(g) || argc > 4)
	    && gvc->job->output_lang != CANONICAL_DOT) {
	    tcldot_layout(g, (argc > 4) ? argv[4] : (char *) NULL);
	}

	emit_reset(gvc, g);	/* reset page numbers in postscript */
	dotneato_write_one(gvc, g);
	gvrender_delete_jobs(gvc);
	return TCL_OK;

    } else {
	Tcl_AppendResult(interp, "bad option \"", argv[1],
			 "\": must be one of:",
			 "\n\taddedge, addnode, addsubgraph, countedges, countnodes,",
			 "\n\tlayout, listattributes, listedgeattributes, listnodeattributes,",
			 "\n\tlistedges, listnodes, listsubgraphs, render, rendergd,",
			 "\n\tqueryattributes, queryedgeattributes, querynodeattributes,",
			 "\n\tqueryattributevalues, queryedgeattributevalues, querynodeattributevalues,",
			 "\n\tsetattributes, setedgeattributes, setnodeattributes,",
			 "\n\tshowname, write.", (char *) NULL);
	return TCL_ERROR;
    }
}				/* graphcmd */

static int dotnew(ClientData clientData, Tcl_Interp * interp,
#ifndef TCLOBJ
		  int argc, char *argv[]
#else				/* TCLOBJ */
		  int argc, Tcl_Obj * CONST objv[]
#endif				/* TCLOBJ */
    )
{
    GVC_t *gvc;
    Agraph_t *g, **gp;
    char c;
    int i, length, kind;
    unsigned long id;

    if ((argc < 2)) {
	Tcl_AppendResult(interp, "wrong # args: should be \"",
			 argv[0],
			 " graphtype ?graphname? ?attributename attributevalue? ?...?\"",
			 (char *) NULL);
	return TCL_ERROR;
    }
    gvc = (GVC_t *) clientData;
    c = argv[1][0];
    length = strlen(argv[1]);
    if ((c == 'd') && (strncmp(argv[1], "digraph", length) == 0)) {
	kind = AGDIGRAPH;
    } else if ((c == 'd')
	       && (strncmp(argv[1], "digraphstrict", length) == 0)) {
	kind = AGDIGRAPHSTRICT;
    } else if ((c == 'g') && (strncmp(argv[1], "graph", length) == 0)) {
	kind = AGRAPH;
    } else if ((c == 'g')
	       && (strncmp(argv[1], "graphstrict", length) == 0)) {
	kind = AGRAPHSTRICT;
    } else {
	Tcl_AppendResult(interp, "bad graphtype \"", argv[1],
			 "\": must be one of:",
			 "\n\tdigraph, digraphstrict, graph, graphstrict.",
			 (char *) NULL);
	return TCL_ERROR;
    }
    gp = (Agraph_t **) tclhandleAlloc(graphTblPtr, interp->result, &id);
    if (argc % 2) {
	/* if odd number of args then argv[2] is name */
	g = agopen(argv[2], kind);
	i = 3;
    } else {
	/* else use handle as name */
	g = agopen(interp->result, kind);
	i = 2;
    }
    if (!g) {
	Tcl_AppendResult(interp, "\nFailure to open graph.",
			 (char *) NULL);
	return TCL_ERROR;
    }
    *gp = g;
    g->handle = id;

    /* link graph to context */
    gvc->g = g;

#ifndef TCLOBJ
    Tcl_CreateCommand(interp, interp->result, graphcmd,
		      (ClientData) gvc, (Tcl_CmdDeleteProc *) NULL);
#else				/* TCLOBJ */
    Tcl_CreateObjCommand(interp, interp->result, graphcmd,
			 (ClientData) gvc, (Tcl_CmdDeleteProc *) NULL);
#endif				/* TCLOBJ */
    setgraphattributes(g, &argv[i], argc - i);
    /* we use GD_drawing(g) as a flag that layout has been done.
     * so we make sure that it is initialized to "not done" */
    GD_drawing(g) = NULL;

    return TCL_OK;
}

/*
 * when a graph is read in from a file or string we need to walk
 * it to create the handles and tcl commands for each 
 * graph, subgraph, node, and edge.
 */
static int tcldot_fixup(Tcl_Interp * interp, GVC_t * gvc)
{
    Agraph_t **gp, *sg, **sgp, *g = gvc->g;
    Agnode_t *n, **np;
    Agedge_t *e, **ep;
    char buf[16];
    unsigned long id;

    if (g->meta_node) {
	for (n = agfstnode(g->meta_node->graph); n;
	     n = agnxtnode(g->meta_node->graph, n)) {
	    sg = agusergraph(n);
	    sgp = (Agraph_t **) tclhandleAlloc(graphTblPtr, buf, &id);
	    *sgp = sg;
	    sg->handle = id;
#ifndef TCLOBJ
	    Tcl_CreateCommand(interp, buf, graphcmd,
			      (ClientData) gvc,
			      (Tcl_CmdDeleteProc *) NULL);
#else				/* TCLOBJ */
	    Tcl_CreateObjCommand(interp, buf, graphcmd,
				 (ClientData) gvc,
				 (Tcl_CmdDeleteProc *) NULL);
#endif				/* TCLOBJ */
	    if (sg == g)
		strcpy(interp->result, buf);
	}
    } else {
	gp = (Agraph_t **) tclhandleAlloc(graphTblPtr, interp->result,
					  &id);
	*gp = g;
	g->handle = id;
#ifndef TCLOBJ
	Tcl_CreateCommand(interp, interp->result, graphcmd,
			  (ClientData) gvc, (Tcl_CmdDeleteProc *) NULL);
#else				/* TCLOBJ */
	Tcl_CreateObjCommand(interp, interp->result, graphcmd,
			     (ClientData) gvc, (Tcl_CmdDeleteProc *) NULL);
#endif				/* TCLOBJ */
    }
    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	np = (Agnode_t **) tclhandleAlloc(nodeTblPtr, buf, &id);
	*np = n;
	n->handle = id;
#ifndef TCLOBJ
	Tcl_CreateCommand(interp, buf, nodecmd,
			  (ClientData) gvc, (Tcl_CmdDeleteProc *) NULL);
#else				/* TCLOBJ */
	Tcl_CreateObjCommand(interp, buf, nodecmd,
			     (ClientData) gvc, (Tcl_CmdDeleteProc *) NULL);
#endif				/* TCLOBJ */
	for (e = agfstout(g, n); e; e = agnxtout(g, e)) {
	    ep = (Agedge_t **) tclhandleAlloc(edgeTblPtr, buf, &id);
	    *ep = e;
	    e->handle = id;
#ifndef TCLOBJ
	    Tcl_CreateCommand(interp, buf, edgecmd,
			      (ClientData) gvc,
			      (Tcl_CmdDeleteProc *) NULL);
#else				/* TCLOBJ */
	    Tcl_CreateObjCommand(interp, buf, edgecmd,
				 (ClientData) gvc,
				 (Tcl_CmdDeleteProc *) NULL);
#endif				/* TCLOBJ */
	}
    }
    return TCL_OK;
}

/*
 * mygets - same api as gets
 *
 * gets one line at a time from a Tcl_Channel and places it in a user buffer
 *    up to a maximum of n characters
 *
 * returns pointer to obtained line in user buffer, or
 * returns NULL when last line read from memory buffer
 *
 * This is probably innefficient because it introduces
 * one more stage of line buffering during reads (at least)
 * but it is needed so that we can take full advantage
 * of the Tcl_Channel mechanism.
 */
static char *mygets(char *ubuf, int n, FILE * channel)
{
    static Tcl_DString dstr;
    static int strpos;

    if (!n) {			/* a call with n==0 (from aglexinit) resets */
	*ubuf = '\0';
	strpos = 0;
	return NULL;
    }

    /* 
     * the user buffer might not be big enough to hold the line.
     */
    if (strpos) {
	if (Tcl_DStringLength(&dstr) > (n + strpos)) {
	    /* chunk between first and last */
	    strncpy(ubuf, (strpos + Tcl_DStringValue(&dstr)), n - 1);
	    strpos += (n - 1);
	    ubuf[n] = '\0';
	} else {
	    /* last chunk */
	    strcpy(ubuf, (strpos + Tcl_DStringValue(&dstr)));
	    strpos = 0;
	}
    } else {
	Tcl_DStringFree(&dstr);
	Tcl_DStringInit(&dstr);
	if (Tcl_Gets((Tcl_Channel) channel, &dstr) < 0) {
	    /* probably EOF, but could be other read errors */
	    *ubuf = '\0';
	    return NULL;
	}
	/* linend char(s) were stripped off by Tcl_Gets,
	 * append a canonical linenend. */
	Tcl_DStringAppend(&dstr, "\n", 1);
	if (Tcl_DStringLength(&dstr) > n) {
	    /* first chunk */
	    strncpy(ubuf, Tcl_DStringValue(&dstr), n - 1);
	    strpos = n - 1;
	    ubuf[n] = '\0';
	} else {
	    /* single chunk */
	    strcpy(ubuf, Tcl_DStringValue(&dstr));
	}
    }
    return ubuf;

#if 0
    if (!n) {			/* a call with n==0 (from aglexinit) resets */
	mempos = (char *) mbuf;	/* cast from FILE* required by API */
    }

    clp = to = ubuf;
    for (i = 0; i < n - 1; i++) {	/* leave room for terminator */
	if (*mempos == '\0') {
	    if (i) {		/* if mbuf doesn't end in \n, provide one */
		*to++ = '\n';
	    } else {		/* all done */
		clp = NULL;
		mempos = NULL;
	    }
	    break;		/* last line or end-of-buffer */
	}
	if (*mempos == '\n') {
	    *to++ = *mempos++;
	    break;		/* all done with this line */
	}
	*to++ = *mempos++;	/* copy character */
    }
    *to++ = '\0';		/* place terminator in ubuf */
    return clp;
#endif
}

static int dotread(ClientData clientData, Tcl_Interp * interp,
#ifndef TCLOBJ
		   int argc, char *argv[]
#else				/* TCLOBJ */
		   int argc, Tcl_Obj * CONST objv[]
#endif				/* TCLOBJ */
    )
{
    GVC_t *gvc;
    Agraph_t *g;
    Tcl_Channel channel;
    int mode;

    if (argc < 2) {
	Tcl_AppendResult(interp, "wrong # args: should be \"",
			 argv[0], " fileHandle\"", (char *) NULL);
	return TCL_ERROR;
    }
    channel = Tcl_GetChannel(interp, argv[1], &mode);
    if (channel == NULL || !(mode & TCL_READABLE)) {
	Tcl_AppendResult(interp, "\nChannel \"",
			 argv[1], "\"", "is unreadable.", (char *) NULL);
	return TCL_ERROR;
    }
    gvc = (GVC_t *) clientData;
    /*
     * read a graph from the channel, the channel is left open
     *   ready to read the first line after the last line of
     *   a properly parsed graph. If the graph doesn't parse
     *   during reading then the channel will be left at EOF
     */
    g = agread_usergets((FILE *) channel, (mygets));
    if (!g) {
	Tcl_AppendResult(interp, "\nFailure to read graph \"",
			 argv[1], "\"", (char *) NULL);
	if (agerrors()) {
	    Tcl_AppendResult(interp, " because of syntax errors.",
			     (char *) NULL);
	}
	return TCL_ERROR;
    }
    if (agerrors()) {
	Tcl_AppendResult(interp, "\nSyntax errors in file \"",
			 argv[1], " \"", (char *) NULL);
	return TCL_ERROR;
    }
    /* we use GD_drawing(g) as a flag that layout has been done.
     * so we make sure that it is initialized to "not done" */
    GD_drawing(g) = NULL;

    /* link graph to context */
    gvc->g = g;

    return (tcldot_fixup(interp, gvc));
}

static int dotstring(ClientData clientData, Tcl_Interp * interp,
#ifndef TCLOBJ
		     int argc, char *argv[]
#else				/* TCLOBJ */
		     int argc, Tcl_Obj * CONST objv[]
#endif				/* TCLOBJ */
    )
{
    GVC_t *gvc;
    Agraph_t *g;

    if (argc < 2) {
	Tcl_AppendResult(interp, "wrong # args: should be \"",
			 argv[0], " string\"", (char *) NULL);
	return TCL_ERROR;
    }
    if (!(g = agmemread(argv[1]))) {
	Tcl_AppendResult(interp, "\nFailure to read string \"",
			 argv[1], "\"", (char *) NULL);
	if (agerrors()) {
	    Tcl_AppendResult(interp, " because of syntax errors.",
			     (char *) NULL);
	}
	return TCL_ERROR;
    }
    if (agerrors()) {
	Tcl_AppendResult(interp, "\nSyntax errors in string \"",
			 argv[1], " \"", (char *) NULL);
	return TCL_ERROR;
    }
    /* we use GD_drawing(g) as a flag that layout has been done.
     * so we make sure that it is initialized to "not done" */
    GD_drawing(g) = NULL;

    /* link graph to context */
    gvc = (GVC_t *) clientData;
    gvc->g = g;

    return (tcldot_fixup(interp, gvc));
}

#if defined(_BLD_tcldot) && defined(_DLL)
__EXPORT__
#endif
int Tcldot_Init(Tcl_Interp * interp)
{
    GVC_t *gvc;
    char *user;

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

    Gdtclft_Init(interp);

    aginit();
    /* set persistent attributes here */
    agnodeattr(NULL, "label", NODENAME_ESC);

    /* create a GraphViz Context and pass a pointer to it in clientdata */
    user = username();
    gvc = gvNEWcontext(Info, user);

#ifndef TCLOBJ
    Tcl_CreateCommand(interp, "dotnew", dotnew,
		      (ClientData) gvc, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateCommand(interp, "dotread", dotread,
		      (ClientData) gvc, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateCommand(interp, "dotstring", dotstring,
		      (ClientData) gvc, (Tcl_CmdDeleteProc *) NULL);
#else				/* TCLOBJ */
    Tcl_CreateObjCommand(interp, "dotnew", dotnew,
			 (ClientData) gvc, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "dotread", dotread,
			 (ClientData) gvc, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "dotstring", dotstring,
			 (ClientData) gvc, (Tcl_CmdDeleteProc *) NULL);
#endif				/* TCLOBJ */

    graphTblPtr = (void *) tclhandleInit("graph", sizeof(Agraph_t *), 10);
    nodeTblPtr = (void *) tclhandleInit("node", sizeof(Agnode_t *), 100);
    edgeTblPtr = (void *) tclhandleInit("edge", sizeof(Agedge_t *), 100);

    return TCL_OK;
}

int Tcldot_SafeInit(Tcl_Interp * interp)
{
    return Tcldot_Init(interp);
}
