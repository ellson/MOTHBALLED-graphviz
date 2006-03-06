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

/* Common header used by both clients and plugins */

#ifndef GVCINT_H
#define GVCINT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "gvg.h"

    typedef struct {
	int flags;
    } gvlayout_features_t;

    /* active plugin headers */
    typedef struct gvplugin_active_layout_s {
        gvlayout_engine_t *engine;
        int id;
        gvlayout_features_t *features;
        char *type;
    } gvplugin_active_layout_t;

    typedef struct gvplugin_active_textlayout_s {
        gvtextlayout_engine_t *engine;
        int id;
        char *type;
    } gvplugin_active_textlayout_t;

    typedef struct gvplugin_active_usershape_s {
        gvusershape_engine_t *engine;
        int id;
        char *type;
    } gvplugin_active_usershape_t;

    typedef struct gvplugin_available_s gvplugin_available_t;

    struct gvplugin_available_s {
	gvplugin_available_t *next;       /* next plugin in linked list, or NULL */
	char *typestr;		 /* type string, e.g. "png" or "ps" */
	int quality;             /* programmer assigned quality
					ranking within type (+ve or -ve int)
					- codegens have quality = 0 */
	char *path;		 /* file path to library containing plugin,
					or NULL if builtin */
	char *packagename;	 /* package name */
	gvplugin_installed_t *typeptr;  /* pointer to jumptable for plugin,
					or NULL if not yet loaded */
    };

    struct GVC_s {
	/* gvNEWcontext() */
	char *user;
	char **info;

        void (*errorfn) (char *fmt, ...);

	char *config_path;
	bool config_found;

	/* plugins */
#define ELEM(x) +1
	/* APIS expands to "+1 +1 ... +1" to give the number of APIs */
	gvplugin_available_t *apis[ APIS ]; /* array of linked-list of plugins per api */
	gvplugin_available_t *api[ APIS ];  /* array of current plugins per api */
#undef ELEM

	/* gvrender_begin_job() */
	gvplugin_active_textlayout_t textlayout;
	gvplugin_active_usershape_t usershape;
	gvplugin_active_layout_t layout;

	/* keybindings for keyboard events */
	gvevent_key_binding_t *keybindings;
	int numkeys;
	void *keycodes;

	/* gvrender_config() */
	GVG_t *gvg;
    };

#ifdef __cplusplus
}
#endif
#endif				/* GVCINT_H */
