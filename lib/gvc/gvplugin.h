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

/* Header used by plugins */

#ifndef GVPLUGIN_H
#define GVPLUGIN_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NULL
#define NULL (void *)0
#endif

/*
 * Define an apis array of name strings using an enumerated api_t as index.
 * The enumerated type is defined here.  The apis array is
 * inititialized in gvplugin.c by redefining ELEM and reinvoking APIS.
 */
#define APIS ELEM(render) ELEM(layout) ELEM(display) ELEM(text)

#define ELEM(x) API_##x,
    typedef enum { APIS } api_t; /* API_render, API_layout, ... */
#undef ELEM

    typedef struct {
	int id;
	char *type;
	int quality;
	void *engine;
	void *features;
    } gvplugin_type_t;

    typedef struct {
	api_t api;
	gvplugin_type_t *types;
    } gvplugin_api_t;

    typedef struct {
	char *name;		/* used instead of a file pathname
					when this plugin is loaded */
	gvplugin_api_t *apis;
    } gvplugin_library_t;

#ifdef __cplusplus
}
#endif
#endif				/* GVPLUGIN_H */
