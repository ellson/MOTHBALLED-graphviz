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

int nodecmd(ClientData clientData, Tcl_Interp * interp,
#ifndef TCLOBJ
		   int argc, char *argv[]
#else				/* TCLOBJ */
		   int argc, Tcl_Obj * CONST objv[]
#endif				/* TCLOBJ */
    )
{
    char c, **argv2;
    int i, j, length, argc2;
    Agraph_t *g;
    Agnode_t *n, *head;
    Agedge_t *e;
    Agsym_t *a;
    gctx_t *gctx = (gctx_t *)clientData;

    if (argc < 2) {
	Tcl_AppendResult(interp, "Wrong # args: should be \"", argv[0], " option ?arg arg ...?\"", NULL);
	return TCL_ERROR;
    }
    n = cmd2n(argv[0]);
    if (!n) {
	Tcl_AppendResult(interp, "Node \"", argv[0], "\" not found", NULL);
	return TCL_ERROR;
    }
    g = agraphof(n);

    c = argv[1][0];
    length = strlen(argv[1]);


    if ((c == 'a') && (strncmp(argv[1], "addedge", length) == 0)) {
	if ((argc < 3) || (!(argc % 2))) {
	    Tcl_AppendResult(interp, "Wrong # args: should be \"", argv[0], " addedge head ?attributename attributevalue? ?...?\"", NULL);
	    return TCL_ERROR;
	}
	head = cmd2n(argv[2]);
	if (!head) {
	    if (!(head = agfindnode(g, argv[2]))) {
		Tcl_AppendResult(interp, "Head node \"", argv[2], "\" not found.", NULL);
		return TCL_ERROR;
	    }
	}
	if (agroot(g) != agroot(agraphof(head))) {
	    Tcl_AppendResult(interp, "Nodes ", argv[0], " and ", argv[2], " are not in the same graph.", NULL);
	    return TCL_ERROR;
	}
        e = agedge(g, n, head, NULL, 1);
        Tcl_AppendResult(interp, obj2cmd(e), NULL);
	setedgeattributes(agroot(g), e, &argv[3], argc - 3);
	return TCL_OK;

    } else if ((c == 'd') && (strncmp(argv[1], "delete", length) == 0)) {
	deleteNode(gctx, g, n);
	return TCL_OK;

    } else if ((c == 'f') && (strncmp(argv[1], "findedge", length) == 0)) {
	if (argc < 3) {
	    Tcl_AppendResult(interp, "Wrong # args: should be \"", argv[0], " findedge headnodename\"", NULL);
	    return TCL_ERROR;
	}
	if (!(head = agfindnode(g, argv[2]))) {
	    Tcl_AppendResult(interp, "Head node \"", argv[2], "\" not found.", NULL);
	    return TCL_ERROR;
	}
	if (!(e = agfindedge(g, n, head))) {
	    Tcl_AppendResult(interp, "Edge \"", argv[0], " - ", obj2cmd(head), "\" not found.", NULL);
	    return TCL_ERROR;
	}
	Tcl_AppendElement(interp, obj2cmd(head));
	return TCL_OK;

    } else if ((c == 'l')
	       && (strncmp(argv[1], "listattributes", length) == 0)) {
	listNodeAttrs (interp, g);
	return TCL_OK;

    } else if ((c == 'l') && (strncmp(argv[1], "listedges", length) == 0)) {
	for (e = agfstedge(g, n); e; e = agnxtedge(g, e, n)) {
	    Tcl_AppendElement(interp, obj2cmd(e));
	}
	return TCL_OK;

    } else if ((c == 'l')
	       && (strncmp(argv[1], "listinedges", length) == 0)) {
	for (e = agfstin(g, n); e; e = agnxtin(g, e)) {
	    Tcl_AppendElement(interp, obj2cmd(e));
	}
	return TCL_OK;

    } else if ((c == 'l')
	       && (strncmp(argv[1], "listoutedges", length) == 0)) {
	for (e = agfstout(g, n); e; e = agnxtout(g, e)) {
	    Tcl_AppendElement(interp, obj2cmd(e));
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
		if ((a = agfindnodeattr(g, argv2[j]))) {
		    Tcl_AppendElement(interp, agxget(n, a));
		} else {
		    Tcl_AppendResult(interp, "No attribute named \"", argv2[j], "\"", NULL);
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
		if ((a = agfindnodeattr(g, argv2[j]))) {
		    Tcl_AppendElement(interp, argv2[j]);
		    Tcl_AppendElement(interp, agxget(n, a));
		} else {
		    Tcl_AppendResult(interp, "No attribute named \"", argv2[j], "\"", NULL);
		    return TCL_ERROR;
		}
	    }
	    Tcl_Free((char *) argv2);
	}
	return TCL_OK;

    } else if ((c == 's')
	       && (strncmp(argv[1], "setattributes", length) == 0)) {
	g = agroot(g);
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
	    setnodeattributes(g, n, argv2, argc2);
	    Tcl_Free((char *) argv2);
	} else {
	    if ((argc < 4) || (argc % 2)) {
		Tcl_AppendResult(interp, "Wrong # args: should be \"", argv[0],
				 "\" setattributes attributename attributevalue ?attributename attributevalue? ?...?",
				 NULL);
		return TCL_ERROR;
	    }
	    setnodeattributes(g, n, &argv[2], argc - 2);
	}
	return TCL_OK;

    } else if ((c == 's') && (strncmp(argv[1], "showname", length) == 0)) {
	Tcl_SetResult(interp, agnameof(n), TCL_STATIC);
	return TCL_OK;

    } else {
	Tcl_AppendResult(interp, "Bad option \"", argv[1],
			 "\": must be one of:",
			 "\n\taddedge, listattributes, listedges, listinedges,",
			 "\n\tlistoutedges, queryattributes, queryattributevalues,",
			 "\n\tsetattributes, showname.", NULL);
	return TCL_ERROR;
    }
}
