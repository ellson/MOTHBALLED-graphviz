/* $Id$ $Revision$ */
/* vim:set shiftwidth=4 ts=8: */

/**********************************************************
 * *      This software is part of the graphviz package      *
 * *                http://www.graphviz.org/                 *
 * *                                                         *
 * *            Copyright (c) 1994-2004 AT&T Corp.           *
 * *                and is licensed under the                *
 * *            Common Public License, Version 1.0           *
 * *                      by AT&T Corp.                      *
 * *                                                         *
 * *        Information and Software Systems Research        *
 * *              AT&T Research, Florham Park NJ             *
 * **********************************************************/

#ifndef			GVC_H
#define			GVC_H

#include "render.h"
#include "gvplugin.h"
#include "gvcint.h"
#include "gvcproc.h"

#ifdef __cplusplus
extern "C" {
#endif

extern GVC_t *gvContext(void);
extern int gvLayout(GVC_t *gvc, graph_t *g, char *engine);
extern int gvRender(GVC_t *gvc, graph_t *g, char *format, FILE *out);
extern void gvCleanup(GVC_t *gvc);

#ifdef __cplusplus
}
#endif
#endif			/* RENDER_H */
