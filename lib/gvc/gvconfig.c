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

#ifndef DISABLE_LTDL
static char *libdir = GVLIBDIR;
#endif

/*
    A config for gvrender is a text file containing a
    list of plugin librariess and their capabilities using a tcl-like
    syntax

    Lines beginning with '#' are ignored as comments

    Blank lines are allowed and ignored.

    plugin_library_path packagename {
	plugin_api {
	    plugin_type plugin_quality
	    ...
	}
	...
    ...

    e.g.

	/usr/lib/graphviz/libgvplugin_cairo.so cairo {renderer {x 0 png 10 ps -10}}
	/usr/lib/graphviz/libgvplugin_gd.so gd {renderer {png 0 gif 0 jpg 0}}

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

#ifndef DISABLE_LTDL
static int gvconfig_plugin_install_from_config(GVC_t * gvc, char *s)
{
    char *path, *packagename, *api, *type;
    api_t gv_api;
    int quality, rc;
    int nest = 0;

    separator(&nest, &s);
    while (*s) {
	path = token(&nest, &s);
	rc = strncmp(path, libdir, strlen(libdir));
        if(rc) {
	    agerr (AGERR, "config contains invalid path\n");
	    return 0;
	}
	if (nest == 0)
	    packagename = token(&nest, &s);
        else
	    packagename = "x";
	do {
	    api = token(&nest, &s);
	    gv_api = gvplugin_api(api);
	    if (gv_api == -1) {
		agerr(AGERR, "invalid api in config: %s %s\n", path, api);
		return 0;
	    }
	    do {
		if (nest == 2)
		    type = token(&nest, &s);
		if (nest == 2)
		    quality = atoi(token(&nest, &s));
		else
		    quality = 0;
		rc = gvplugin_install (gvc, gv_api,
				type, quality, packagename, path, NULL);
		if (!rc) {
		    agerr(AGERR, "config error: %s %s %s\n", path, api, type);
		    return 0;
		}
	    } while (nest == 2);
	} while (nest == 1);
    }
    return 1;
}
#endif

static void gvconfig_plugin_install_from_library(GVC_t * gvc, char *path, gvplugin_library_t *library)
{
    gvplugin_api_t *apis;
    gvplugin_type_t *types;
    int i;

    for (apis = library->apis; (types = apis->types); apis++) {
	for (i = 0; types[i].type; i++) {
	    gvplugin_install(gvc, apis->api, types[i].type,
			types[i].quality, library->packagename, path, &types[i]);
        }
    }
}

static void gvconfig_write_library_config(char *path, gvplugin_library_t *library, FILE *f)
{
    gvplugin_api_t *apis;
    gvplugin_type_t *types;
    int i;

    fprintf(f, "%s %s {\n", path, library->packagename);
    for (apis = library->apis; (types = apis->types); apis++) {
        fprintf(f, "\t%s {\n", gvplugin_api_name(apis->api));
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

#ifndef DISABLE_LTDL
static void config_rescan(GVC_t *gvc, char *config_path)
{
    FILE *f = NULL;
    glob_t globbuf;
    char *config_glob;
    int i, rc;
    gvplugin_library_t *library;
    char *plugin_glob = "libgvplugin*.so.?";

    if (config_path) {
	f = fopen(config_path,"w");
	if (!f) {
	    agerr(AGERR,"failed to open %s for write.\n", config_path);
	}
    }

    /* load all libraries even if can't save config */
    config_glob = malloc(strlen(libdir)
			    + 1
			    + strlen(plugin_glob)
			    + 1);
    strcpy(config_glob, libdir);
    strcat(config_glob, "/");
    strcat(config_glob, plugin_glob);

    rc = glob(config_glob, GLOB_NOSORT, NULL, &globbuf);
    if (rc == 0) {
	for (i = 0; i < globbuf.gl_pathc; i++) {
	    library = gvplugin_library_load(globbuf.gl_pathv[i]);
	    if (library) {
		gvconfig_plugin_install_from_library(gvc, globbuf.gl_pathv[i], library);
		if (f) {
		    gvconfig_write_library_config(globbuf.gl_pathv[i], library, f);
		}
	    }
	}
    }
    globfree(&globbuf);
    free(config_glob);
    if (f)
	fclose(f);
}
#endif

/*
  gvconfig - parse a config file and install the identified plugins
 */
void gvconfig(GVC_t * gvc)
{
#ifdef DISABLE_LTDL
    gvplugin_library_t **libraryp;
#else
    int sz, rc;
    struct stat config_st, libdir_st;
    FILE *f = NULL;
    char *config_path = NULL, *config_text = NULL;
    char *home;

    char *config_dir_name = ".graphviz";
    char *config_file_name = "config";
    char *s;

#define MAX_SZ_CONFIG 100000
#endif
    
#ifndef DISABLE_CODEGENS
    config_codegen_builtins(gvc);
#endif

#ifdef DISABLE_LTDL
    for (libraryp = builtins; *libraryp; libraryp++) {
	gvconfig_plugin_install_from_library(gvc, NULL, *libraryp);
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
	rc = -1;
    }
    else {
	s = gvhostname();
        config_path = malloc(strlen(home) 
				+ 1
				+ strlen(config_dir_name)
				+ 1
				+ strlen(s)
				+ 1
				+ strlen(config_file_name)
				+ 1);
        strcpy(config_path, home);
        strcat(config_path, "/");
        strcat(config_path, config_dir_name);
        rc = mkdir(config_path, 0700);

        strcat(config_path, "/");
	if (s[0]) {
	    strcat(config_path, s);
	    strcat(config_path, "_");
	}
        strcat(config_path, config_file_name);

        rc = stat(config_path, &config_st);
    }
	
    if (rc == -1 || libdir_st.st_mtime > config_st.st_mtime) {
	config_rescan(gvc, config_path);
    }
    else {
	/* load in the cached plugin library data */

	rc = 0;
	if (config_st.st_size > MAX_SZ_CONFIG) {
	    agerr(AGERR,"%s is bigger than I can handle.\n", config_path);
	}
	else {
	    f = fopen(config_path,"r");
	    if (!f) {
	        agerr (AGERR,"failed to open %s for read.\n", config_path);
	    }
	    else {
	        config_text = malloc(config_st.st_size + 1);
	        sz = fread(config_text, 1, config_st.st_size, f);
	        if (sz == 0) {
		    agerr(AGERR,"%s is zero sized, or other read error.\n", config_path);
		    free(config_text);
	        }
		else {
	            config_text[sz] = '\0';  /* make input into a null terminated string */
	            rc = gvconfig_plugin_install_from_config(gvc, config_text);
		    /* NB. config_text not freed because we retain char* into it */
		}
	    }
	    if (f)
		fclose(f);
	}
	if (!rc) {
	    agerr(AGERR,"rescanning for plugins\n");
	    config_rescan(gvc, config_path);
	}
    }
    if (config_path)
	free(config_path);
#endif
}
