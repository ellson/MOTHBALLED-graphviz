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

#ifndef TEXTPARA_H
#define TEXTPARA_H

#ifdef __cplusplus
extern "C" {
#endif

#include "geom.h"

    typedef struct textpara_t {
	char *str;      /* stored in utf-8 */
	char *xshow;
	void *layout;
	void (*free_layout) (void *layout);   /* FIXME - this is ugly */
	double width, height;
	char just;
    } textpara_t;

#ifdef __cplusplus
}
#endif
#endif
