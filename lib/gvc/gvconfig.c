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

#include        "config.h"
#include        "types.h"
#include        "macros.h"
#include        "gvc.h"

/*
    A config for gvrender is a text file containing a
    tcl-like-syntax list of plugins and their capabilities.

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

/*
  gvconfig - parse a config file and install the identified plugins
 */
void gvconfig(GVC_t * gvc, char *config)
{
    char *s, *path, *api, *type;
    int quality;
    int nest = 0;

    s = strdup(config);
    /* this copy is never free'd because the config uses pointers
       into it for string values */

    separator(&nest, &s);
    while (*s) {
	path = token(&nest, &s);
	do {
	    api = token(&nest, &s);
	    do {
		type = token(&nest, &s);
		if (nest == 2)
		    quality = atoi(token(&nest, &s));
		else
		    quality = 0;
		if (!
		    (gvplugin_install
		     (gvc, gvplugin_api(api), type, quality, path,
		      NULL))) {
		    fprintf(stderr, "config error: %s %s %s\n", api, type,
			    path);
		    return;
		}
	    } while (nest == 2);
	} while (nest == 1);
    }
}
