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

#ifndef GV_MEMORY_H
#define GV_MEMORY_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef DMALLOC
#define NEW(t)           (t*)calloc(1,sizeof(t))
#define N_NEW(n,t)       (t*)calloc((n),sizeof(t))
#define GNEW(t)          (t*)malloc(sizeof(t))
#define N_GNEW(n,t)      (t*)malloc((n)*sizeof(t))
#define ALLOC(size,ptr,type) (ptr? (type*)realloc(ptr,(size)*sizeof(type)):(type*)malloc((size)*sizeof(type)))
#define RALLOC(size,ptr,type) ((type*)realloc(ptr,(size)*sizeof(type)))
#define ZALLOC(size,ptr,type,osize) (ptr? (type*)recalloc(ptr,(size)*sizeof(type)):(type*)calloc((size),sizeof(type)))
#else
#define NEW(t)           (t*)zmalloc(sizeof(t))
#define N_NEW(n,t)       (t*)zmalloc((n)*sizeof(t))
#define GNEW(t)          (t*)gmalloc(sizeof(t))
#define N_GNEW(n,t)      (t*)gmalloc((n)*sizeof(t))
#define ALLOC(size,ptr,type) (ptr? (type*)grealloc(ptr,(size)*sizeof(type)):(type*)gmalloc((size)*sizeof(type)))
#define RALLOC(size,ptr,type) ((type*)grealloc(ptr,(size)*sizeof(type)))
#define ZALLOC(size,ptr,type,osize) (ptr? (type*)zrealloc(ptr,size,sizeof(type),osize):(type*)zmalloc((size)*sizeof(type)))
#endif


    extern void *zmalloc(size_t);
    extern void *zrealloc(void *, size_t, size_t, size_t);
    extern void *gmalloc(size_t);
    extern void *grealloc(void *, size_t);

#ifdef __cplusplus
}
#endif

#endif
