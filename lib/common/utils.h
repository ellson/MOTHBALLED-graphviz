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


/* extern char *pathget(char *, char *, char *); */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifndef HAVE_STRCASECMP
    extern int strcasecmp(const char *s1, const char *s2);
#endif
#ifndef HAVE_STRNCASECMP
    extern int strncasecmp(const char *s1, const char *s2, unsigned int n);
#endif

#ifdef __cplusplus
}
#endif
