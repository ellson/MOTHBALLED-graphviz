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

#ifndef _STR_H
#define _STR_H
    void Sinit(void);
    void Sterm(void);
    char *Spath(char *, Tobj);
    char *Sseen(Tobj, char *);
    char *Sabstract(Tobj, Tobj);
    char *Stfull(Tobj);
    char *Ssfull(Tobj, Tobj);
    char *Scfull(Tobj, int, int);
#endif				/* _STR_H */

#ifdef __cplusplus
}
#endif
