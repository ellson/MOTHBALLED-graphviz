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

size_t Tcldot_string_writer(GVJ_t *job, const char *s, size_t len)
{
    Tcl_AppendResult((Tcl_Interp*)(job->context), s, NULL);
    return len;
}

size_t Tcldot_channel_writer(GVJ_t *job, const char *s, size_t len)
{
    return Tcl_Write((Tcl_Channel)(job->output_file), s, len);
}

/* handles (tcl commands) to obj* */

Agraph_t *cmd2g(char *cmd) {
    Agraph_t *g = NULL;

    if (sscanf(cmd, "graph%p", &g) != 1 || !g)
        return NULL;
    return g;
}
Agnode_t *cmd2n(char *cmd) {
    Agnode_t *n = NULL;

    if (sscanf(cmd, "node%p", &n) != 1 || !n)
        return NULL;
    return n;
}
Agedge_t *cmd2e(char *cmd) {
    Agedge_t *e = NULL;

    if (sscanf(cmd, "edge%p", &e) != 1 || !e)
        return NULL;
    return e;
}


/* obj* to handles (tcl commands) */

char *obj2cmd (void *obj) {
    static char buf[32];

    switch (AGTYPE(obj)) {
        case AGRAPH: sprintf(buf,"graph%p",obj); break;
        case AGNODE: sprintf(buf,"node%p",obj); break;
        case AGINEDGE: 
        case AGOUTEDGE: sprintf(buf,"edge%p",obj); break;
    }
    return buf;
}


void deleteEdge(gctx_t *gctx, Agraph_t * g, Agedge_t *e)
{
    char *hndl;

    hndl = obj2cmd(e);
    agdelete(gctx->g, e);  /* delete edge from root graph */
    Tcl_DeleteCommand(gctx->ictx->interp, hndl);
}
static void deleteNodeEdges(gctx_t *gctx, Agraph_t *g, Agnode_t *n)
{
    Agedge_t *e, *e1;

    e = agfstedge(g, n);
    while (e) {
	e1 = agnxtedge(g, e, n);
	deleteEdge(gctx, g, e);
	e = e1;
    }
}
void deleteNode(gctx_t * gctx, Agraph_t *g, Agnode_t *n)
{
    char *hndl;

    deleteNodeEdges(gctx, gctx->g, n); /* delete all edges to/from node in root graph */

    hndl = obj2cmd(n);
    agdelete(gctx->g, n); /* delete node from root graph */
    Tcl_DeleteCommand(gctx->ictx->interp, hndl);
}
static void deleteGraphNodes(gctx_t * gctx, Agraph_t *g)
{
    Agnode_t *n, *n1;

    n = agfstnode(g);
    while (n) {
	n1 = agnxtnode(g, n);
	deleteNode(gctx, g, n);
	n = n1;
    }
}
void deleteGraph(gctx_t * gctx, Agraph_t *g)
{
    Agraph_t *sg;
    char *hndl;

    for (sg = agfstsubg (g); sg; sg = agnxtsubg (sg)) {
	deleteGraph(gctx, sg);
    }
    deleteGraphNodes(gctx, g);

    hndl = obj2cmd(g);
    if (g == agroot(g)) {
	agclose(g);
    } else {
	agdelsubg(agroot(g), g);
    }
    Tcl_DeleteCommand(gctx->ictx->interp, hndl);
}

static void myagxset(void *obj, Agsym_t *a, char *val)
{
    int len;
    char *hs;

    if (a->name[0] == 'l' && val[0] == '<' && strcmp(a->name, "label") == 0) {
        len = strlen(val);
        if (val[len-1] == '>') {
            hs = strdup(val+1);
                *(hs+len-2) = '\0';
            val = agstrdup_html(agraphof(obj),hs);
            free(hs);
        }
    }
    agxset(obj, a, val);
}
void setgraphattributes(Agraph_t * g, char *argv[], int argc)
{
    int i;
    Agsym_t *a;

    for (i = 0; i < argc; i++) {
	if (!(a = agfindgraphattr(agroot(g), argv[i])))
	    a = agattr(agroot(g), AGRAPH, argv[i], "");
	myagxset(g, a, argv[++i]);
    }
}

void setedgeattributes(Agraph_t * g, Agedge_t * e, char *argv[], int argc)
{
    int i;
    Agsym_t *a;

    for (i = 0; i < argc; i++) {
	/* silently ignore attempts to modify "key" */
	if (strcmp(argv[i], "key") == 0) {
	    i++;
	    continue;
	}
	if (e) {
	    if (!(a = agfindedgeattr(g, argv[i])))
		a = agattr(agroot(g), AGEDGE, argv[i], "");
	    myagxset(e, a, argv[++i]);
	}
	else {
	    agattr(g, AGEDGE, argv[i], argv[i+1]);
	    i++;
	}
    }
}

void setnodeattributes(Agraph_t * g, Agnode_t * n, char *argv[], int argc)
{
    int i;
    Agsym_t *a;

    for (i = 0; i < argc; i++) {
	if (n) {
	    if (!(a = agfindnodeattr(g, argv[i])))
		a = agattr(agroot(g), AGNODE, argv[i], "");
	    myagxset(n, a, argv[++i]);
	}
	else {
	    agattr(g, AGNODE, argv[i], argv[i+1]);
	    i++;
	}
    }
}

void listGraphAttrs (Tcl_Interp * interp, Agraph_t* g)
{
    Agsym_t *a = NULL;
    while ((a = agnxtattr(g, AGRAPH, a))) {
	Tcl_AppendElement(interp, a->name);
    }
}
void listNodeAttrs (Tcl_Interp * interp, Agraph_t* g)
{
    Agsym_t *a = NULL;
    while ((a = agnxtattr(g, AGNODE, a))) {
	Tcl_AppendElement(interp, a->name);
    }
}
void listEdgeAttrs (Tcl_Interp * interp, Agraph_t* g)
{
    Agsym_t *a = NULL;
    while ((a = agnxtattr(g, AGEDGE, a))) {
	Tcl_AppendElement(interp, a->name);
    }
}

void tcldot_layout(GVC_t *gvc, Agraph_t * g, char *engine)
{
    char buf[256];
    Agsym_t *a;
    int rc;

    gvFreeLayout(gvc, g);               /* in case previously drawn */

/* support old behaviors if engine isn't specified*/
    if (!engine || *engine == '\0') {
	if (agisdirected(g))
	    rc = gvlayout_select(gvc, "dot");
	else
	    rc = gvlayout_select(gvc, "neato");
    }
    else {
	if (strcasecmp(engine, "nop") == 0) {
	    Nop = 2;
	    PSinputscale = POINTS_PER_INCH;
	    rc = gvlayout_select(gvc, "neato");
	}
	else {
	    rc = gvlayout_select(gvc, engine);
	}
	if (rc == NO_SUPPORT)
	    rc = gvlayout_select(gvc, "dot");
    }
    if (rc == NO_SUPPORT) {
        fprintf(stderr, "Layout type: \"%s\" not recognized. Use one of:%s\n",
                engine, gvplugin_list(gvc, API_layout, engine));
        return;
    }
    gvLayoutJobs(gvc, g);

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
    if (!(a = agattr(g, AGRAPH, "bb", NULL))) 
	a = agattr(g, AGRAPH, "bb", "");
    agxset(g, a, buf);
}
