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
    list of plugins and their capabilities using a tcl-like
    syntax

    Lines beginning with '#' are ignored as comments

    Blank lines are allowed and ignored.

    plugin_path {
	plugin_api {
	    plugin_type plugin_quality
	    ...
	}
	...
    ...

    e.g.

	/usr/lib/graphviz/cairo.so {renderer {x 0 png 10 ps -10}}
	/usr/lib/graphviz/gdgen.so {renderer {png 0 gif 0 jpg 0}}

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

#ifdef DISABLE_LTDL
extern gvplugin_library_t *builtins[];
#endif

/*
  gvconfig - parse a config file and install the identified plugins
 */
void gvconfig(GVC_t * gvc)
{
    gvplugin_api_t *apis;
    gvplugin_type_t *types;
    int i;
#ifdef DISABLE_LTDL
    gvplugin_library_t **libraryp;
#else
    gvplugin_library_t *library;
    char *s, *path, *api, *type;
    api_t gv_api;
    int quality;
    int nest = 0;
    int sz, rc, j;
    struct stat config_st, libdir_st;
    FILE *f;
    char *config_path, *config_glob, *home, *config_text;
    glob_t globbuf;

    char *dot_graphviz = "/.graphviz";
    char *libdir = GVLIBDIR;
    char *plugin_glob = "/libgvplugin*.so.0";

#define MAX_SZ_CONFIG 100000
#endif
    
#ifndef DISABLE_CODEGENS
    config_codegen_builtins(gvc);
#endif

#ifdef DISABLE_LTDL
    for (libraryp = builtins; *libraryp; libraryp++) {
        for (apis = (*libraryp)->apis; (types = apis->types); apis++) {
            for (i = 0; types[i].type; i++) {
                gvplugin_install(gvc, apis->api,
                                 types[i].type, types[i].quality,
                                 (*libraryp)->name, &types[i]);
            }
        }
    }

#else
    /* see if there are any new plugins */

    rc = stat(libdir, &libdir_st);
    if (rc == -1) {	/* if we fail to stat it then it probably doesn't exist
		   so just fail silently */
	return;
    }

    home = getenv ("HOME");
    if (!home) {
	return;
    }

    config_path = malloc(strlen(home) + strlen(dot_graphviz) + 1);
    strcpy(config_path, home);
    strcat(config_path, dot_graphviz);

    rc = stat(config_path, &config_st);

    if (rc == -1 || libdir_st.st_mtime > config_st.st_mtime) {
	f = fopen(config_path,"w");
	if (!f) {
            fprintf(stderr,"failed to open %s for write.\n", config_path);
	    free(config_path);
	    return;
	}

	config_glob = malloc(strlen(libdir) + strlen(plugin_glob) + 1);
	strcpy(config_glob, libdir);
	strcat(config_glob, plugin_glob);

	rc = glob(config_glob, GLOB_NOSORT, NULL, &globbuf);
        if (rc == 0) {
	    for (j = 0; j < globbuf.gl_pathc; j++) {
		library = gvplugin_library_load(globbuf.gl_pathv[j]);
		if (library) {
		    fputs (globbuf.gl_pathv[j], f);
		    fputs (" {\n", f);
		    for (apis = library->apis; (types = apis->types); apis++) {
			fputs ("\t", f);
			fputs (gvplugin_api_name(apis->api), f);
			fputs (" {\n", f);
			for (i = 0; types[i].type; i++) {
			    /* might as well install it since its already loaded */
			    gvplugin_install(gvc, apis->api, types[i].type,
                                types[i].quality, library->name, &types[i]);
			    fprintf(f, "\t\t%s %d\n",
				types[i].type, types[i].quality);
			}
		        fputs ("\t}\n", f);
		    }
		    fputs ("}\n", f);
		}
	    }
	}
	globfree(&globbuf);
        free(config_glob);
	fclose(f);
	return;     /* all plugins have been installed */
    }

    /* load in the cached plugin library data */

    if (config_st.st_size > MAX_SZ_CONFIG) {
        fprintf(stderr,"%s is bigger than I can handle.\n", config_path);
	free(config_path);
	return;
    }
    f = fopen(config_path,"r");
    if (!f) {	/* if we fail to open it then it probably doesn't exists
		   so just fail silently, clean up and return */
	free(config_path);
	return;
    }
    config_text = malloc(config_st.st_size + 1);
    config_text[0] = '\0';
    sz = fread(config_text, 1, config_st.st_size, f);
    if (sz == 0) {
        fprintf(stderr,"%s is zero sized, or other read error.\n", config_path);
	free(config_path);
	free(config_text);
	return;
    }
    fclose(f);
    free(config_path); /* not needed now that we've slurped in the contents */

    config_text[config_st.st_size] = '\0';  /* make input into a null terminated string */

    s = config_text;
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
#endif
}
