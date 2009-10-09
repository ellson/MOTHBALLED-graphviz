/* $Id$Revision: */
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

#ifndef GLMOTION_H
#define GLMOTION_H

#include "viewport.h"

#ifdef __cplusplus
extern "C" {
#endif

    void glmotion_main(ViewInfo * v, GdkEventMotion * event,
		       GtkWidget * widget);
    void glmotion_zoom(ViewInfo * v);
    void glmotion_pan(ViewInfo * v);
    void glmotion_rotate(ViewInfo * v);
    void glmotion_adjust_pan(ViewInfo * v, float panx, float pany);
    void graph_zoom(float real_zoom);
    void glmotion_zoom_inc(int zoomin);

#ifdef __cplusplus
}				/* end extern "C" */
#endif
#endif
