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



#ifndef _CG_H_
#define _CG_H_

#include "defs.h"

/*************************
 * C.G. method - SPARSE  *
 ************************/

    extern void conjugate_gradient(vtx_data *, double *, double *, int,
				   double, int);

/*************************
 * C.G. method - DENSE   *
 ************************/

    extern void conjugate_gradient_f(float **, double *, double *, int,
				     double, int, boolean);

    extern void conjugate_gradient_mkernel(float *, float *, float *, int,
					   double, int);

#endif

#ifdef __cplusplus
}
#endif
