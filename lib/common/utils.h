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

#include "geom.h"

#ifndef HAVE_STRCASECMP
    extern int strcasecmp(const char *s1, const char *s2);
#endif
#ifndef HAVE_STRNCASECMP
    extern int strncasecmp(const char *s1, const char *s2, size_t n);
#endif

    extern void *zmalloc(size_t);
    extern void *zrealloc(void *, size_t, size_t, size_t);
    extern void *gmalloc(size_t);
    extern void *grealloc(void *, size_t);

    extern nodequeue *new_queue(int);
    extern void free_queue(nodequeue *);
    extern void enqueue(nodequeue *, Agnode_t *);
    extern Agnode_t *dequeue(nodequeue *);

    extern int late_attr(void *, char *);
    extern int late_int(void *, Agsym_t *, int, int);
    extern double late_double(void *, Agsym_t *, double, double);
    extern char *late_nnstring(void *, Agsym_t *, char *);
    extern char *late_string(void *, Agsym_t *, char *);
    extern int late_bool(void *, Agsym_t *, int);

    extern Agnode_t *UF_find(Agnode_t *);
    extern Agnode_t *UF_union(Agnode_t *, Agnode_t *);
    extern void UF_remove(Agnode_t *, Agnode_t *);
    extern void UF_singleton(Agnode_t *);
    extern void UF_setname(Agnode_t *, Agnode_t *);

    extern char *username(void);
    extern char *safefile(char *shapefilename);
    extern void cat_libfile(FILE *, char **, char **);

    extern int mapbool(char *);
    extern int maptoken(char *, char **, int *);

    extern void toggle(int);
    extern int test_toggle(void);

    extern void common_init_node(node_t * n);
    extern int common_init_edge(edge_t * e);

    extern void updateBB(graph_t * g, textlabel_t * lp);
    extern void compute_bb(Agraph_t *);
    extern boolean overlap_node(node_t *n, boxf b);
    extern boolean overlap_label(textlabel_t *lp, boxf b);
    extern boolean overlap_edge(edge_t *e, boxf b);

    extern int processClusterEdges(graph_t * g);
    extern void undoClusterEdges(graph_t * g);
    extern attrsym_t* safe_dcl(graph_t*, void*, char*, char*,
             attrsym_t * (*fun) (Agraph_t *, char *, char *));

    extern char *latin1ToUTF8(char *);
    extern char* utf8ToLatin1 (char* ins);

#ifdef __cplusplus
}
#endif
