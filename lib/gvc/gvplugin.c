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

#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<assert.h>

#include        "config.h"
#include        "types.h"
#include        "macros.h"
#include        "gvc.h"

/*
 * Define an apis array of name strings using an enumerated api_t as index.
 * The enumerated type is defined gvplugin.h.  The apis array is
 * inititialized here by redefining ELEM and reinvoking APIS.
 *
 * The last enum is NUM_APIS which is the size of the array.
 */
#define ELEM(x) #x,
static char *apis[] = { APIS };	/* "renderer", "scripter", ... */

#undef ELEM

/* translate a string api name to its type, or -1 on error */
api_t gvplugin_api(char *str)
{
    api_t api;

    for (api = 0; api < NUM_APIS; api++) {
	if (strcmp(str, apis[api]) == 0)
	    return api;
    }
    return -1;			/* invalid api */
}

/* install a plugin description into the list of available plugins */
/* list is alpha sorted by type, the quality sorted within the type,
   then, if qualities are the same, last install wins */
boolean gvplugin_install(GVC_t * gvc, api_t api,
			 char *typestr, int quality, char *path,
			 gvplugin_type_t * typeptr)
{
    gv_plugin_t *plugin, **pnext;


    if (api < 0)
	return FALSE;

    /* point to the beginning of the linked list of plugins for this api */
    pnext = &(gvc->apis[api]);

    /* keep alpha-sorted and insert new duplicates ahead of old */
    while (*pnext && strcmp(typestr, (*pnext)->typestr) > 0)
	pnext = &((*pnext)->next);

    /* keep quality sorted within type and inster new duplicates ahead of old */
    while (*pnext && quality < (*pnext)->quality)
	pnext = &((*pnext)->next);

    plugin = malloc(sizeof(gv_plugin_t));
    plugin->next = *pnext;
    *pnext = plugin;
    plugin->typestr = typestr;
    plugin->quality = quality;
    plugin->path = path;	/* filepath for .so, shortname for builtins */
    plugin->typeptr = typeptr;	/* null if not loaded */

    return TRUE;
}

/* load a plugin of type=str where str can optionally contain a ":path" modifier */
gv_plugin_t *gvplugin_load(GVC_t * gvc, api_t api, char *str)
{
    gv_plugin_t **pnext;
    char *s, *p;

    /* check for valid apis[] index */
    if (api < 0)
	return NULL;

    /* point to the beginning of the linked list of plugins for this api */
    pnext = &(gvc->apis[api]);

    /* does str have a :path modifier? */
    s = strdup(str);
    p = strchr(s, ':');
    if (p)
	*p++ = '\0';

    while (*pnext) {
	if (strcmp(s, (*pnext)->typestr) == 0) {
	    if (p) {
		if (strcmp(p, (*pnext)->path) == 0)
		    break;
	    } else
		break;
	}
	pnext = &((*pnext)->next);
    }
    if ((*pnext)->typeptr == NULL) {
	/* FIXME - load dll here */
    }
    free(s);
    return (gvc->api[api] = *pnext);
}

/* string buffer management - FIXME - must have 20 solutions for this same thing */
static const char *append_buf(char sep, char *str, boolean new)
{
    static char *buf;
    static int bufsz, pos;
    int len;
    char *p;

    if (new)
	pos = 0;
    len = strlen(str) + 1;
    if (bufsz < (pos + len + 1)) {
	bufsz += 4 * len;
	buf = realloc(buf, bufsz);
    }
    p = buf + pos;
    *p++ = sep;
    strcpy(p, str);
    pos += len;
    return buf;
}

/* assemble a string list of available plugins */
const char *gvplugin_list(GVC_t * gvc, api_t api, char *str)
{
    gv_plugin_t **pnext, **plugin;
    const char *buf = NULL;
    char *s, *p, *typestr_last;
    boolean new = TRUE;

    /* check for valid apis[] index */
    if (api < 0)
	return NULL;

    /* does str have a :path modifier? */
    s = strdup(str);
    p = strchr(s, ':');
    if (p)
	*p++ = '\0';

    /* point to the beginning of the linked list of plugins for this api */
    plugin = &(gvc->apis[api]);

    if (p) {			/* if str contains a ':', and if we find a match for the type,
				   then just list teh alternative paths for the plugin */
	pnext = plugin;
	while (*pnext) {
	    /* list only the matching type */
	    if (strcmp(s, (*pnext)->typestr) == 0) {
		/* list each member of the matching type as "type:path" */
		append_buf(' ', (*pnext)->typestr, new);
		buf = append_buf(':', (*pnext)->path, FALSE);
		new = FALSE;
	    }
	    pnext = &((*pnext)->next);
	}
    }
    if (new) {			/* if the type was not found, or if str without ':',
				   then just list available types */
	pnext = plugin;
	typestr_last = NULL;
	while (*pnext) {
	    /* list only one instance of type */
	    if (!typestr_last
		|| strcmp(typestr_last, (*pnext)->typestr) != 0) {
		/* list it as "type"  i.e. w/o ":path" */
		buf = append_buf(' ', (*pnext)->typestr, new);
		new = FALSE;
	    }
	    typestr_last = (*pnext)->typestr;
	    pnext = &((*pnext)->next);
	}
    }
    free(s);
    return buf;
}

extern gvplugin_t *builtins[];

void gvplugin_builtins(GVC_t * gvc)
{
    gvplugin_t **plugin;
    gvplugin_api_t *apis;
    gvplugin_type_t *types;
    int i;

    for (plugin = builtins; *plugin; plugin++) {
	for (apis = (*plugin)->apis; (types = apis->types); apis++) {
	    for (i = 0; types[i].type; i++) {
		gvplugin_install(gvc, apis->api,
				 types[i].type, types[i].quality,
				 (*plugin)->name, &types[i]);
	    }
	}
    }
}
