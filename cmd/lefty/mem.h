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

/* Lefteris Koutsofios - AT&T Labs Research */

#ifndef _MEM_H
#define _MEM_H

#ifdef FEATURE_MINTSIZE
typedef int Msize_t;
#define M_SIZEMAX INT_MAX
#else
typedef short Msize_t;
#define M_SIZEMAX SHRT_MAX
#endif
typedef struct Mheader_t {
    char type;
    char area;
    Msize_t size;
} Mheader_t;
#define M_HEADERSIZE sizeof (Mheader_t)

#define M_GCOFF 0
#define M_GCON  1

#define M_GCFULL 0
#define M_GCINCR 1

#define M_UNITSIZE sizeof (long)
#define M_BYTE2SIZE(l) ((long) (((l + M_UNITSIZE - 1) / M_UNITSIZE)))
#define M_AREAOF(p) ((int) (((Mheader_t *) p)->area))
#define M_TYPEOF(p) ((int) (((Mheader_t *) p)->type))

extern int Mhaspointers[];
extern int Mgcstate;
extern int Mcouldgc;

void Minit (void (*) (void));
void Mterm (void);
void *Mnew (long, int);
void *Mallocate (long);
void Mfree (void *, long);
void *Marrayalloc (long);
void *Marraygrow (void *, long);
void Marrayfree (void *);
long Mpushmark (void *);
void Mpopmark (long);
void Mresetmark (long, void *);
void Mmkcurr (void *);
void Mdogc (int);
void Mreport (void);
#endif /* _MEM_H */

#ifdef __cplusplus
}
#endif

