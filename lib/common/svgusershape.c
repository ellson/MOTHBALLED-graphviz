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

#include "render.h"
#include "gvplugin.h"
#include "gvcint.h"
#include "gvcproc.h"

point svg_image_size(graph_t * g, char *shapeimagefile)
{
    point rv;

    rv.x = rv.y = 0;
    return rv;
}
