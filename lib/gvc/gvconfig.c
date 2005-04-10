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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#ifndef DISABLE_LTDL
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<glob.h>
#endif

#include        "types.h"
#include        "macros.h"
#include        "gvc.h"

#ifndef DISABLE_CODEGENS
extern void config_codegen_builtins(GVC_t *gvc);
#endif

/*
    A config for gvrender is a text file containing a
    list of plugin librariess and their capabilities using a tcl-like
    syntax

    Lines beginning with '#' are ignored as comments

    Blank lines are allowed and ignored.

    plugin_library_path {
	plugin_api {
	    plugin_type plugin_quality
	    ...
	}
	...
    ...

    e.g.

	/usr/lib/graphviz/libgvplugin_cairo.so {renderer {x 0 png 10 ps -10}}
	/usr/lib/graphviz/libgvplugin_gdgen.so {renderer {png 0 gif 0 jpg 0}}

    Internally the config is maintained as lists of plugin_types for each plugin_api.
    If multiple plugins of the same type are found then the highest quality wins.
    If equal quality then the last-one-installed wins (thus giving preference to
    external plugins over internal builtins).

 */

/*
  separator - consume all non-token characters until next token.  This includes:
	comments:   '#' ... '\n'
	nesting:    '{'
	unnesting:  '}'
	whitespace: ' ','\t','\n'

	*nest is changed according to nesting/unnesting processed
 */
static void separator(int *nest, char **tokens)
{
    char c, *s;

    s = *tokens;
    while ((c = *s)) {
	/* #->eol = comment */
	if (c == '#') {
	    s++;
	    while ((c = *s)) {
		s++;
		if (c == '\n')
		    break;
	    }
	    continue;
	}
	if (c == '{') {
	    (*nest)++;
	    s++;
	    continue;
	}
	if (c == '}') {
	    (*nest)--;
	    s++;
	    continue;
	}
	if (c == ' ' || c == '\n' || c == '\t') {
	    s++;
	    continue;
	}
	break;
    }
    *tokens = s;
}

/* 
  token - capture all characters until next separator, then consume separator,
	return captured token, leave **tokens pointing to next token.
 */
static char *token(int *nest, char **tokens)
{
    char c, *s, *t;

    s = t = *tokens;
    while ((c = *s)) {
	if (c == '#'
	    || c == ' ' || c == '\t' || c == '\n' || c == '{' || c == '}')
	    break;
	s++;
    }
    *tokens = s;
    separator(nest, tokens);
    *s = '\0';
    return t;
}

static void gvconfig_plugin_install_from_config(GVC_t * gvc, char *s)
{
    char *path, *api, *type;
    api_t gv_api;
    int quality, rc;
    int nest = 0;

    separator(&nest, &s);
    while (*s) {
	path = token(&nest, &s);
	do {
	    api = token(&nest, &s);
	    gv_api = gvplugin_api(api);
	    if (gv_api == -1) {
		fprintf(stderr, "invalid api in config: %s %s\n", path, api);
		return;
	    }
	    do {
		type = token(&nest, &s);
		if (nest == 2)
		    quality = atoi(token(&nest, &s));
		else
		    quality = 0;
		rc = gvplugin_install (gvc, gv_api, type, quality, path, NULL);
		if (!rc) {
		    fprintf(stderr, "config error: %s %s %s\n", path, api, type);
		    return;
		}
	    } while (nest == 2);
	} while (nest == 1);
    }
}

static void gvconfig_plugin_install_from_library(GVC_t * gvc, gvplugin_library_t *library)
{
    gvplugin_api_t *apis;
    gvplugin_type_t *types;
    int i;

    for (apis = library->apis; (types = apis->types); apis++) {
	for (i = 0; types[i].type; i++) {
	    gvplugin_install(gvc, apis->api, types[i].type,
			types[i].quality, library->name, &types[i]);
        }
    }
}

static void gvconfig_write_library_config(char *path, gvplugin_library_t *library, FILE *f)
{
    gvplugin_api_t *apis;
    gvplugin_type_t *types;
    int i;

    fputs (path, f);
    fputs (" {\n", f);
    for (apis = library->apis; (types = apis->types); apis++) {
	fputs ("\t", f);
	fputs (gvplugin_api_name(apis->api), f);
	fputs (" {\n", f);
	for (i = 0; types[i].type; i++) {
	    fprintf(f, "\t\t%s %d\n", types[i].type, types[i].quality);
	}
	fputs ("\t}\n", f);
    }
    fputs ("}\n", f);
}

#ifdef DISABLE_LTDL
extern gvplugin_library_t *builtins[];
#endif

/*
  gvconfig - parse a config file and install the identified plugins
 */
void gvconfig(GVC_t * gvc)
{
#ifdef DISABLE_LTDL
    gvplugin_library_t **libraryp;
#else
    gvplugin_library_t *library;
    int sz, rc, i;
    struct stat config_st, libdir_st;
    FILE *f = NULL;
    char *config_path = NULL, *config_text = NULL;
    char *config_glob, *home;
    glob_t globbuf;

    char *dot_graphviz = "/.graphviz";
    char *libdir = GVLIBDIR;
    char *plugin_glob = "/libgvplugin*.so.?";

#define MAX_SZ_CONFIG 100000
#endif
    
#ifndef DISABLE_CODEGENS
    config_codegen_builtins(gvc);
#endif

#ifdef DISABLE_LTDL
    for (libraryp = builtins; *libraryp; libraryp++) {
	gvconfig_plugin_install_from_library(gvc, *libraryp);
    }
#else
    /* see if there are any new plugins */

    rc = stat(libdir, &libdir_st);
    if (rc == -1) {	/* if we fail to stat it then it probably doesn't exist
		   so just fail silently */
	return;
    }

    home = getenv ("HOME");
    if (home) {
        config_path = malloc(strlen(home) + strlen(dot_graphviz) + 1);
        strcpy(config_path, home);
        strcat(config_path, dot_graphviz);

        rc = stat(config_path, &config_st);
	if (rc == -1) {
	    free(config_path);
	    config_path = NULL;
	}
    }
	
    if (! config_path || libdir_st.st_mtime > config_st.st_mtime) {
	if (config_path) {
		f = fopen(config_path,"w");
		if (!f) {
		    fprintf(stderr,"failed to open %s for write.\n", config_path);
		}
	}
	/* load all libraries even if can't save config */

	config_glob = malloc(strlen(libdir) + strlen(plugin_glob) + 1);
	strcpy(config_glob, libdir);
	strcat(config_glob, plugin_glob);

	rc = glob(config_glob, GLOB_NOSORT, NULL, &globbuf);
        if (rc == 0) {
	    for (i = 0; i < globbuf.gl_pathc; i++) {
		library = gvplugin_library_load(globbuf.gl_pathv[i]);
		if (library) {
		    gvconfig_plugin_install_from_library(gvc, library);
		    if (f) {
			gvconfig_write_library_config(globbuf.gl_pathv[i], library, f);
		    }
		}
	    }
	}
	globfree(&globbuf);
        free(config_glob);
    }
    else {
	/* load in the cached plugin library data */

	if (config_st.st_size > MAX_SZ_CONFIG) {
	    fprintf(stderr,"%s is bigger than I can handle.\n", config_path);
	}
	else {
	    f = fopen(config_path,"r");
	    if (!f) {
	        fprintf(stderr,"failed to open %s for read.\n", config_path);
	    }
	    else {
	        config_text = malloc(config_st.st_size + 1);
	        sz = fread(config_text, 1, config_st.st_size, f);
	        if (sz == 0) {
		    fprintf(stderr,"%s is zero sized, or other read error.\n", config_path);
		    free(config_text);
	        }
		else {
	            config_text[sz] = '\0';  /* make input into a null terminated string */
	            gvconfig_plugin_install_from_config(gvc, config_text);
		    /* NB. config_text not freed becasue we retain char* into it */
		}
	    }
	}
    }
    if (config_path)
	free(config_path);
    if (f)
	fclose(f);
#endif
}
