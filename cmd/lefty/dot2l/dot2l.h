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


#ifndef _DOT2L_H
#define _DOT2L_H
#define SMALLBUF 128

int yyparse(void);
int yylex(void);

typedef struct edgeframe_t {
    struct edgeframe_t *next;
    int type;
    void *obj;
    char *port;
} edgeframe_t;

typedef struct graphframe_t {
    struct graphframe_t *next;
    Tobj g, graphs, nodes, edges;
    Tobj gattr, nattr, eattr, ecopy;
    long emark;
    struct edgeframe_t *estack;
} graphframe_t;

typedef enum {
    NODE, EDGE, GRAPH
} objtype_t;

extern char *gtype, *etype;
extern int yaccdone;
extern int attrclass;
extern int inattrstmt;

#ifndef TRUE
#define FALSE 0
#define TRUE 1
#endif

Tobj D2Lparsegraphlabel (Tobj, Tobj);
Tobj D2Lreadgraph (int, Tobj);
void D2Lwritegraph (int, Tobj, int);
void D2Lbegin (char *);
void D2Lend (void);
void D2Labort (void);
void D2Lpushgraph (char *);
Tobj D2Lpopgraph (void);
Tobj D2Linsertnode (char *);
void D2Linsertedge (Tobj, char *, Tobj, char *);
void D2Lbeginedge (int, Tobj, char *);
void D2Lmidedge (int, Tobj, char *);
void D2Lendedge (void);
void D2Lsetattr (char *, char *);
#endif /* _DOT2L_H */

#ifdef __cplusplus
}
#endif

