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

#ifndef _SFDISC_H
#define _SFDISC_H	1

#include	<sfio.h>

    _BEGIN_EXTERNS_
/* functions to create disciplines */
    extern int sfdcdio _ARG_((Sfio_t *, size_t));
    extern int sfdcdos _ARG_((Sfio_t *));
    extern int sfdcfilter _ARG_((Sfio_t *, const char *));
    extern int sfdclzw _ARG_((Sfio_t *));
    extern int sfdcseekable _ARG_((Sfio_t *));
    extern int sfdcslow _ARG_((Sfio_t *));
    extern int sfdcsubstream _ARG_((Sfio_t *, Sfio_t *, Sfoff_t, Sfoff_t));
    extern int sfdctee _ARG_((Sfio_t *, Sfio_t *));
    extern int sfdcunion _ARG_((Sfio_t *, Sfio_t **, int));
     _END_EXTERNS_
#endif
#ifdef __cplusplus
}
#endif
