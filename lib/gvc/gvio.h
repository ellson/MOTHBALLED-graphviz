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

#ifndef GVDEVICE_H
#define GVDEVICE_H

#include "gvcjob.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef GVDLL
#define extern __declspec(dllexport)
#else
#define extern
#endif

/*visual studio*/
#ifdef WIN32_DLL
#ifndef GVC_EXPORTS
#define extern __declspec(dllimport)
#endif
#endif
/*end visual studio*/

    extern size_t gvwrite (GVJ_t * job, const char *s, size_t len);
    extern size_t gvfwrite (FILE * job, const char *s, size_t len);
    extern int gvferror (FILE * job);
    extern int gvputc(GVJ_t * job, int c);
    extern int gvputs(GVJ_t * job, const char *s);
    extern void gvprintf(GVJ_t * job, const char *format, ...);
    extern void gvprintdouble(GVJ_t * job, double num); 
    extern void gvprintpointf(GVJ_t * job, pointf p);
    extern void gvprintpointflist(GVJ_t * job, pointf *p, int n);

#undef extern

#ifdef __cplusplus
}
#endif

#endif				/* GVDEVICE_H */
