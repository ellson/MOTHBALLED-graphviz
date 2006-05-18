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

#include "gvcext.h"

#ifndef NULL
#define NULL (void *)0
#endif

/* 
 * Terminology:
 *
 *    package         - e.g. libgvplugin_cairo.so, or table of codegen builtins.
 *       api	      - e.g. render
 *          type      - e.g. "png", "ps"
 */

/*
 * Define an apis array of name strings using an enumerated api_t as index.
 * The enumerated type is defined here.  The apis array is
 * inititialized in gvplugin.c by redefining ELEM and reinvoking APIS.
 */
#define APIS ELEM(render) ELEM(layout) ELEM(device)

/*
 * Define api_t using names based on the plugin names with API_ prefixed.
 */
#define ELEM(x) API_##x,
    typedef enum { APIS _DUMMY_ELEM_=0 } api_t; /* API_render, API_layout, ... */
    			/* Stupid but true: The sole purpose of "_DUMMY_ELEM_=0"
			 * is to avoid a "," after the last element of the enum
			 * because some compilers when using "-pedantic"
			 * generate an error for about the dangling ","
			 * but only if this header is used from a .cpp file!
			 * Setting it to 0 makes sure that the enumeration
			 * does not define an extra value.  (It does however
			 * define _DUMMY_ELEM_ as an enumeration symbol,
			 * but its value duplicates that of the first
			 * symbol in the enumeration - in this case "render".)
			 */

    			/* One could wonder why trailing "," in:
			 * 	int nums[]={1,2,3,};
			 * is OK, but in:
			 * 	typedef enum {a,b,c,} abc_t; 
			 * is not!!!
			 */
#undef ELEM

    typedef struct {
	int id;         /* an id that is only unique within a package 
			of plugins of the same api.
			A codegen id is unique in in the set of codegens.
			A renderer-type such as "png" in the cairo package
			has an id that is different from the "ps" type
			in the same package */
	char *type;	/* a string name, such as "png" or "ps" that
			distinguishes different types withing the same
			api (renderer in this case) */
	int quality;    /* an arbitrary integer used for ordering plugins of
			the same type from different packages */
	void *engine;   /* pointer to the jump table for the plugin */
	void *features; /* pointer to the feature description 
				void* because type varies by api */
    } gvplugin_installed_t;

    typedef struct {
	api_t api;
	gvplugin_installed_t *types;
    } gvplugin_api_t;

    typedef struct {
	char *packagename;    /* used when this plugin is builtin and has
					no pathname */
	gvplugin_api_t *apis;
    } gvplugin_library_t;

#ifdef __cplusplus
}
#endif
#endif				/* GVPLUGIN_H */
