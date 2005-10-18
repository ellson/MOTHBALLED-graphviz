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

#ifndef               RENDER_H
#define               RENDER_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_LIMITS_H
#include <limits.h>
#else
#ifdef HAVE_VALUES_H
#include <values.h>
#endif
#endif

#include <signal.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STDDEF_H
#include <stddef.h>
#endif
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include "logic.h"
#include "geom.h"
#include "pathplan.h"
#include "macros.h"
#include "const.h"
#include "types.h"
#include "graph.h"		/* must follow types.h */
#include "globals.h"
#include "renderprocs.h"
#include "utils.h"

typedef struct epsf_s {
    int macro_id;
    point offset;
} epsf_t;

typedef struct {
    Dtlink_t link;
    char *name;
    int macro_id;
    point size;
    point origin;
    char *contents;
    int	must_inline;
} ps_image_t;


#endif				/* RENDER_H */
