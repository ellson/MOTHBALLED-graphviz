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


/* Lefteris Koutsofios - AT&T Bell Laboratories */

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

    typedef enum {
	M_GCOFF, M_GCON
    } Mgcstate_t;

    typedef enum {
	M_GCFULL, M_GCINCR
    } Mgctype_t;

#define M_UNITSIZE sizeof (long)
#define M_BYTE2SIZE(l) ((long) (((l + M_UNITSIZE - 1) / M_UNITSIZE)))
#define M_AREAOF(p) ((int) (((Mheader_t *) p)->area))
#define M_TYPEOF(p) ((int) (((Mheader_t *) p)->type))

    extern int Mhaspointers[];
    extern Mgcstate_t Mgcstate;
    extern int Mcouldgc;

    void Minit(void (*)(void));
    void Mterm(void);
    void *Mnew(long, int);
    void *Mallocate(long);
    void Mfree(void *, long);
    void *Marrayalloc(long);
    void *Marraygrow(void *, long);
    void Marrayfree(void *);
    long Mpushmark(void *);
    void Mpopmark(long);
    void Mresetmark(long, void *);
    void Mmkcurr(void *);
    void Mdogc(Mgctype_t);
    void Mreport(void);
#endif				/* _MEM_H */

#ifdef __cplusplus
}
#endif
