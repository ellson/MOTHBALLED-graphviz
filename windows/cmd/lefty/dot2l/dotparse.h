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

    typedef union {
	long i;
	char *s;
	void *o;
    } YYSTYPE;
#define	T_graph	257
#define	T_digraph	258
#define	T_strict	259
#define	T_node	260
#define	T_edge	261
#define	T_edgeop	262
#define	T_id	263
#define	T_subgraph	264


    extern YYSTYPE yylval;

#ifdef __cplusplus
}
#endif
