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

int edgecmd(ClientData clientData, Tcl_Interp * interp,
#ifndef TCLOBJ
		   int argc, char *argv[]
#else				/* TCLOBJ */
		   int argc, Tcl_Obj * CONST objv[]
#endif				/* TCLOBJ */
    )
{
    char c, *s, **argv2;
    int i, j, length, argc2;
    Agraph_t *g;
    Agedge_t *e;
    Agsym_t *a;
    gctx_t *gctx = (gctx_t *)clientData;

    if (argc < 2) {
	Tcl_AppendResult(interp, "Wrong # args: should be \"", argv[0], "\" option ?arg arg ...?", NULL);
	return TCL_ERROR;
    }
    e = cmd2e(argv[0]);
    if (!e) {
        Tcl_AppendResult(interp, "Edge \"", argv[0], "\" not found", NULL);
        return TCL_ERROR;
    }
    g = agraphof(agtail(e));

    c = argv[1][0];
    length = strlen(argv[1]);

    if ((c == 'd') && (strncmp(argv[1], "delete", length) == 0)) {
	deleteEdge(gctx, g, e);
	return TCL_OK;

    } else if ((c == 'l')
	       && (strncmp(argv[1], "listattributes", length) == 0)) {
	listEdgeAttrs (interp, g);
	return TCL_OK;

    } else if ((c == 'l') && (strncmp(argv[1], "listnodes", length) == 0)) {
	Tcl_AppendElement(interp, obj2cmd(agtail(e)));
	Tcl_AppendElement(interp, obj2cmd(aghead(e)));
	return TCL_OK;

    } else if ((c == 'q')
	       && (strncmp(argv[1], "queryattributes", length) == 0)) {
	for (i = 2; i < argc; i++) {
	    if (Tcl_SplitList
		(interp, argv[i], &argc2,
		 (CONST84 char ***) &argv2) != TCL_OK)
		return TCL_ERROR;
	    for (j = 0; j < argc2; j++) {
		if ((a = agfindedgeattr(g, argv2[j]))) {
		    Tcl_AppendElement(interp, agxget(e, a));
		} else {
		    Tcl_AppendResult(interp, "No attribute named \"", argv2[j], "\"", NULL);
		    return TCL_ERROR;
		}
	    }
	    Tcl_Free((char *) argv2);
	}
	return TCL_OK;

    } else if ((c == 'q') && (strncmp(argv[1], "queryattributevalues", length) == 0)) {
	for (i = 2; i < argc; i++) {
	    if (Tcl_SplitList
		(interp, argv[i], &argc2,
		 (CONST84 char ***) &argv2) != TCL_OK)
		return TCL_ERROR;
	    for (j = 0; j < argc2; j++) {
		if ((a = agfindedgeattr(g, argv2[j]))) {
		    Tcl_AppendElement(interp, argv2[j]);
		    Tcl_AppendElement(interp, agxget(e, a));
		} else {
		    Tcl_AppendResult(interp, "No attribute named \"", argv2[j], "\"", NULL);
		    return TCL_ERROR;
		}
	    }
	    Tcl_Free((char *) argv2);
	}
	return TCL_OK;

    } else if ((c == 's') && (strncmp(argv[1], "setattributes", length) == 0)) {
	if (argc == 3) {
	    if (Tcl_SplitList
		(interp, argv[2], &argc2,
		 (CONST84 char ***) &argv2) != TCL_OK)
		return TCL_ERROR;
	    if ((argc2 == 0) || (argc2 % 2)) {
		Tcl_AppendResult(interp, "Wrong # args: should be \"", argv[0],
				 "\" setattributes attributename attributevalue ?attributename attributevalue? ?...?",
				 NULL);
		Tcl_Free((char *) argv2);
		return TCL_ERROR;
	    }
	    setedgeattributes(agroot(g), e, argv2, argc2);
	    Tcl_Free((char *) argv2);
	} else {
	    if ((argc < 4) || (argc % 2)) {
		Tcl_AppendResult(interp, "Wrong # args: should be \"", argv[0],
				 "\" setattributes attributename attributevalue ?attributename attributevalue? ?...?",
				 NULL);
		return TCL_ERROR;
	    }
	    setedgeattributes(agroot(g), e, &argv[2], argc - 2);
	}
	return TCL_OK;

    } else if ((c == 's') && (strncmp(argv[1], "showname", length) == 0)) {
	if (agisdirected(g))
	    s = "->";
	else
	    s = "--";
	Tcl_AppendResult(interp, agnameof(agtail(e)), s, agnameof(aghead(e)), NULL);
	return TCL_OK;

    } else {
	Tcl_AppendResult(interp, "Bad option \"", argv[1],
			 "\": must be one of:",
			 "\n\tdelete, listattributes, listnodes,",
			 "\n\tueryattributes, queryattributevalues,",
			 "\n\tsetattributes, showname", NULL);
	return TCL_ERROR;
    }
}
