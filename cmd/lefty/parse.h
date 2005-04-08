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

#ifndef _PARSE_H
#define _PARSE_H
typedef struct Psrc_t {
    int flag;
    char *s;
    FILE *fp;
    int tok;
    int lnum;
} Psrc_t;

void Pinit (void);
void Pterm (void);
Tobj Punit (Psrc_t *);
Tobj Pfcall (Tobj, Tobj);
Tobj Pfunction (char *, int);
#endif /* _PARSE_H */

#ifdef __cplusplus
}
#endif
