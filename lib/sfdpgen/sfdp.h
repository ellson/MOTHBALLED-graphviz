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

#ifndef SFDP_H
#define SFDP_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "render.h"

void sfdp_layout (graph_t * g);
void sfdp_cleanup (graph_t * g);
int fdpAdjust (graph_t * g);

#endif
