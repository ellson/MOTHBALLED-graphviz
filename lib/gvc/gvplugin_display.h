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

#ifndef GVDISPLAY_PLUGIN_H
#define GVDISPLAY_PLUGIN_H

#include "gvplugin.h"
#include "gvcint.h"

#ifdef __cplusplus
extern "C" {
#endif

    struct gvdisplay_engine_s {
	void (*init) (GVC_t * gvc);
	void (*process) (GVC_t * gvc);
	void (*deinit) (GVC_t * gvc);
    };

#ifdef __cplusplus
}
#endif
#endif				/* GVDISPLAY_PLUGIN_H */
