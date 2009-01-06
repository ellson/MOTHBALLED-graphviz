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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define EXTERN
#include "types.h"
#include "globals.h"

/* Default layout values, possibly set via command line; -1 indicates unset */
fdpParms_t fdp_parms = {
    1,                          /* useGrid */
    1,                          /* useNew */
    -1,                         /* numIters */
    50,                         /* unscaled */
    0.0,                        /* C */
    1.0,                        /* Tfact */
    -1.0,                       /* K - set in initParams; used in init_edge */
    -1.0,                       /* T0 */
};

