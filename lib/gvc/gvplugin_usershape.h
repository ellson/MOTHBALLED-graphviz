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

#ifndef GVUSERSHAPE_PLUGIN_H
#define GVUSERSHAPE_PLUGIN_H

#include "gvplugin.h"
#include "geom.h"
#include "color.h"
#include "gvcint.h"

#ifdef __cplusplus
extern "C" {
#endif

    struct gvusershape_engine_s {
	void *(*getshape) (char *name);
	void (*freeshapes) (void);
	point (*image_size) (graph_t * g, char *shapefile);
    };

#ifdef __cplusplus
}
#endif
#endif				/* GVUSERSHAPE_PLUGIN_H */
