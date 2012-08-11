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

typedef struct {
#ifdef WITH_CGRAPH
    Agdisc_t mydisc;    // must be first to allow casting mydisc to mycontext
#endif
    void *graphTblPtr, *nodeTblPtr, *edgeTblPtr;
    Tcl_Interp *interp;
    GVC_t *gvc;
} mycontext_t;

/*  Globals */

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
#else
extern void deleteEdges(mycontext_t * mycontext, Agraph_t * g, Agnode_t * n);
extern void deleteNodes(mycontext_t * mycontext, Agraph_t * g);
extern void deleteGraph(mycontext_t * mycontext, Agraph_t * g);
extern void listGraphAttrs (Tcl_Interp * interp, Agraph_t* g);
extern void listNodeAttrs (Tcl_Interp * interp, Agraph_t* g);
extern void listEdgeAttrs (Tcl_Interp * interp, Agraph_t* g);
#endif

extern void setgraphattributes(Agraph_t * g, char *argv[], int argc);
extern void setedgeattributes(Agraph_t * g, Agedge_t * e, char *argv[], int argc);
extern void setnodeattributes(Agraph_t * g, Agnode_t * n, char *argv[], int argc);

extern size_t Tcldot_string_writer(GVJ_t *job, const char *s, size_t len);
extern size_t Tcldot_channel_writer(GVJ_t *job, const char *s, size_t len);

extern void tcldot_layout(GVC_t *gvc, Agraph_t * g, char *engine);
extern void reset_layout(GVC_t *gvc, Agraph_t * sg);








