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

#ifndef GETOPT_H
#define GETOPT_H

/* On most systems, these are defined in unistd.h or stdlib.h,
 * but some systems have no external definitions (UTS, SunOS 4.1),
 * so we provide a declaration if needed.
 */

    extern int getopt(int, char *const *, const char *);
    extern char *optarg;
    extern int opterr, optind, optopt;

#endif

#ifdef __cplusplus
}
#endif
