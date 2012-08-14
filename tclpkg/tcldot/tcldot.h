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


/* avoid compiler warnings with template changes in Tcl8.4 */
/*    specifically just the change to Tcl_CmdProc */
#define USE_NON_CONST
#include <tcl.h>
#include "render.h"
#include "gvc.h"
#include "gvio.h"
#include "tclhandle.h"

#ifndef CONST84
#define CONST84
#endif

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

/*
 * ictx - one per tcl interpreter, may support multiple graph namespaces
 */
typedef struct {
#ifdef WITH_CGRAPH
    Agdisc_t mydisc;    /* must be first to allow casting mydisc to ictx */
    unsigned long int ctr; /* odd number counter for anon objects over all g's in interp */
#endif
/* **FIXME**  #ifndef WITH_CGRAPH */
    void *graphTblPtr, *nodeTblPtr, *edgeTblPtr;
/*            #endif */
    Tcl_Interp *interp;
    GVC_t *gvc;
} ictx_t;

#ifdef WITH_CGRAPH
/*
 * gctx - one for each graph in a tcl interp
 */
typedef struct {
    Agraph_t *g;        /* the graph */
    ictx_t *ictx;
    unsigned long int idx; 
} gctx_t;
#endif

#if HAVE_LIBGD
extern void *GDHandleTable;
extern int Gdtclft_Init(Tcl_Interp *);
#endif

#ifndef WITH_CGRAPH
#undef AGID
#define AGID(x) ((x)->handle)
#endif

extern int graphcmd(ClientData clientData, Tcl_Interp * interp,
#ifndef TCLOBJ
		    int argc, char *argv[]
#else
		    int argc, Tcl_Obj * CONST objv[]
#endif
    );
extern int nodecmd(ClientData clientData, Tcl_Interp * interp,
#ifndef TCLOBJ
		    int argc, char *argv[]
#else
		    int argc, Tcl_Obj * CONST objv[]
#endif
    );
extern int edgecmd(ClientData clientData, Tcl_Interp * interp,
#ifndef TCLOBJ
		    int argc, char *argv[]
#else
		    int argc, Tcl_Obj * CONST objv[]
#endif
    );

#ifdef WITH_CGRAPH
extern void deleteEdges(Agraph_t * g, Agnode_t * n);
extern void deleteNodes(Agraph_t * g);
extern void deleteGraph(Agraph_t * g);
extern void listGraphAttrs (Tcl_Interp * interp, Agraph_t* g);
extern void listNodeAttrs (Tcl_Interp * interp, Agraph_t* g);
extern void listEdgeAttrs (Tcl_Interp * interp, Agraph_t* g);
extern int mygets(void* channel, char *ubuf, int n);
#else
extern void deleteEdges(ictx_t * ictx, Agraph_t * g, Agnode_t * n);
extern void deleteNodes(ictx_t * ictx, Agraph_t * g);
extern void deleteGraph(ictx_t * ictx, Agraph_t * g);
extern void listGraphAttrs (Tcl_Interp * interp, Agraph_t* g);
extern void listNodeAttrs (Tcl_Interp * interp, Agraph_t* g);
extern void listEdgeAttrs (Tcl_Interp * interp, Agraph_t* g);
extern char *mygets(char *ubuf, int n, FILE * channel);
#endif

extern void setgraphattributes(Agraph_t * g, char *argv[], int argc);
extern void setedgeattributes(Agraph_t * g, Agedge_t * e, char *argv[], int argc);
extern void setnodeattributes(Agraph_t * g, Agnode_t * n, char *argv[], int argc);

extern size_t Tcldot_string_writer(GVJ_t *job, const char *s, size_t len);
extern size_t Tcldot_channel_writer(GVJ_t *job, const char *s, size_t len);

extern void tcldot_layout(GVC_t *gvc, Agraph_t * g, char *engine);
extern void reset_layout(GVC_t *gvc, Agraph_t * sg);


