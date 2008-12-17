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

/* Header used by plugins */

#ifndef GVCONFIG_H
#define GVCONFIG_H

#include "gvc.h"
#include "gvplugin.h"

#ifdef __cplusplus
extern "C" {
#endif

extern void gvconfig_plugin_install_from_library(GVC_t * gvc, char *path, gvplugin_library_t *library);

#ifdef __cplusplus
}
#endif
#endif				/* GVCONFIG_H */
