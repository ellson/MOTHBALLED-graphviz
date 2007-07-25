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

#ifdef __cplusplus
extern "C" {
#endif

#ifndef TLAYOUT_H
#define TLAYOUT_H

#include "fdp.h"
#include "xlayout.h"

typedef enum {
  seed_unset, seed_val, seed_time, seed_regular
} seedMode;

    extern void fdp_initParams(graph_t *);
    extern void fdp_tLayout(graph_t *, xparams *);

#endif

#ifdef __cplusplus
}
#endif
