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
#ifndef HAVE_STRERROR
#include <errno.h>

extern int sys_nerr;
extern char *sys_errlist[];

char *strerror(int errorNumber)
{
    if (errorNumber > 0 && errorNumber < sys_nerr) {
	return sys_errlist[errorNumber];
    } else {
	return "";
    }
}
#endif
