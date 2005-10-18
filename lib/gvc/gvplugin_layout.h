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

#ifndef GVLAYOUT_PLUGIN_H
#define GVLAYOUT_PLUGIN_H

#include "gvplugin.h"
#include "gvcint.h"

#ifdef __cplusplus
extern "C" {
#endif

    struct gvlayout_engine_s {
	void (*layout) (graph_t * g);
	void (*cleanup) (graph_t * g);
    };

#ifdef __cplusplus
}
#endif
#endif				/* GVLAYOUT_PLUGIN_H */
