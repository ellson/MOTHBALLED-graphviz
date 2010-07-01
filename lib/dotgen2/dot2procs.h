/* $Id$Revision: */
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

#ifndef DOT2PROCS_H
#define DOT2PROCS_H

#ifdef _BEGIN_EXTERNS_
_BEGIN_EXTERNS_                 /* public data */
#endif
/* tabs at 8, or you're a sorry loser */
#ifdef __cplusplus
extern "C" {
#endif

extern int is_nonconstraint(edge_t * e);

#if defined(_BLD_dot) && defined(_DLL)
#   define extern __EXPORT__
#endif
    extern Agraph_t* dot2_mincross(Agraph_t *);
    /* extern void dot_position(Agraph_t *, aspect_t*); */
    extern void dot2_levels(Agraph_t *);
    /* extern void dot_sameports(Agraph_t *); */
    /* extern void dot_splines(Agraph_t *); */
#undef extern

#ifdef _END_EXTERNS_
     _END_EXTERNS_
#endif
#ifdef __cplusplus
}
#endif

#endif
