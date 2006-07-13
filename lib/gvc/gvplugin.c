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

#include	<string.h>
#ifdef ENABLE_LTDL
#include	<ltdl.h>
#endif

#include        "memory.h"
#include        "types.h"
#include        "graph.h"
#include        "gvplugin.h"
#include        "gvcjob.h"
#include        "gvcint.h"
#include        "gvcproc.h"

extern const bool Demand_Loading;

/*
 * Define an apis array of name strings using an enumerated api_t as index.
 * The enumerated type is defined gvplugin.h.  The apis array is
 * inititialized here by redefining ELEM and reinvoking APIS.
 */
#define ELEM(x) #x,
static char *api_names[] = { APIS };	/* "render", "layout", ... */
#undef ELEM

/* translate a string api name to its type, or -1 on error */
api_t gvplugin_api(char *str)
{
    int api;

    for (api = 0; api < ARRAY_SIZE(api_names); api++) {
	if (strcmp(str, api_names[api]) == 0)
	    return (api_t)api;
    }
    return -1;			/* invalid api */
}

/* translate api_t into string name, or NULL */
char *gvplugin_api_name(api_t api)
{
    if (api < 0 || api >= ARRAY_SIZE(api_names))
	return NULL;
    return api_names[api];
}

/* install a plugin description into the list of available plugins */
/* list is alpha sorted by type, then quality sorted within the type,
   then, if qualities are the same, last install wins */
bool gvplugin_install(GVC_t * gvc, api_t api,
		 char *typestr, int quality, char *packagename, char *path,
		 gvplugin_installed_t * typeptr)
{
    gvplugin_available_t *plugin, **pnext;


    if (api < 0)
	return FALSE;

    /* point to the beginning of the linked list of plugins for this api */
    pnext = &(gvc->apis[api]);

    /* keep alpha-sorted and insert new duplicates ahead of old */
    while (*pnext && strcmp(typestr, (*pnext)->typestr) > 0)
	pnext = &((*pnext)->next);

    /* keep quality sorted within type and inster new duplicates ahead of old */
    while (*pnext && strcmp(typestr, (*pnext)->typestr) == 0 && quality < (*pnext)->quality)
	pnext = &((*pnext)->next);

    plugin = GNEW(gvplugin_available_t);
    plugin->next = *pnext;
    *pnext = plugin;
    plugin->typestr = typestr;
    plugin->quality = quality;
    plugin->packagename = packagename;	/* packagename,  all packages */
    plugin->path = path;	/* filepath for .so, or NULL for builtins */
    plugin->typeptr = typeptr;	/* null if not loaded */

    return TRUE;
}

gvplugin_library_t *gvplugin_library_load(GVC_t *gvc, char *path)
{
#ifdef ENABLE_LTDL
    lt_dlhandle hndl;
    lt_ptr ptr;
    char *s, *sym;
    int len;
    static char *p;
    static int lenp;
    char *libdir;
    char *suffix = "_LTX_library";

    if (!Demand_Loading)
	return NULL;

    libdir = gvconfig_libdir();
    len = strlen(libdir) + 1 + strlen(path) + 1;
    if (len > lenp) {
	lenp = len+20;
	if (p)
	    p = grealloc(p, lenp);
	else
	    p = gmalloc(lenp);
    }
	
    if (path[0] == '/') {
	strcpy(p, path);
    } else {
	strcpy(p, libdir);
	strcat(p, "/");
	strcat(p, path);
    }

    if (lt_dlinit()) {
        agerr(AGERR, "failed to init libltdl\n");
        return NULL;
    }
    hndl = lt_dlopen (p);
    if (!hndl) {
        agerr(AGWARN, (char*)lt_dlerror());
        return NULL;
    }
    if (gvc->common.verbose >= 2)
	fprintf(stderr, "Loading %s\n", p);

    s = strrchr(p, '/');
    len = strlen(s); 
    if (len < strlen("/libgvplugin_x")) {
	agerr (AGERR,"invalid plugin path \"%s\"\n", p);
	return NULL;
    }
    sym = gmalloc(len + strlen(suffix) + 1);
    strcpy(sym, s+4);         /* strip leading "/lib" or "/cyg" */
#ifdef __CYGWIN__
    s = strchr(sym, '-');     /* strip trailing "-1.dll" */
#else 
    s = strchr(sym, '.');     /* strip trailing ".so.0" or ".dll" or ".sl" */
#endif
    strcpy(s,suffix);         /* append "_LTX_library" */

    ptr = lt_dlsym (hndl, sym);
    if (!ptr) {
        agerr (AGERR,"failed to resolve %s in %s\n", sym, p);
	free(sym);
        return NULL;
    }
    free(sym);
    return (gvplugin_library_t *)(ptr);
#else
    agerr (AGERR,"dynamic loading not available\n");
    return NULL;
#endif
}


/* load a plugin of type=str
	where str can optionally contain a ":packagename" modifier */
gvplugin_available_t *gvplugin_load(GVC_t * gvc, api_t api, char *str)
{
    gvplugin_available_t **pnext, *rv;
    gvplugin_library_t *library;
    gvplugin_api_t *apis;
    gvplugin_installed_t *types;
    char *s, *p;
    int i;


    /* check for valid apis[] index */
    if (api < 0)
	return NULL;

    /* does str have a :packagename modifier? */
    s = strdup(str);
    p = strchr(s, ':');
    if (p)
	*p++ = '\0';

    /* point to the beginning of the linked list of plugins for this api */
    pnext = &(gvc->apis[api]);

    while (*pnext) {
	if (strcmp(s, (*pnext)->typestr) == 0) {
	    if (p) {
		if (strcmp(p, (*pnext)->packagename) == 0)
		    break;
	    }
	    else
		break;
	}
	pnext = &((*pnext)->next);
    }

    rv = *pnext;
    if ((*pnext) && (*pnext)->typeptr == NULL) {
        rv = NULL;
	library = gvplugin_library_load(gvc, (*pnext)->path);
	if (library) {

	    /*
	     * FIXME - would be cleaner to here remove the entries from the
	     * config data for the uninstalled library - i.e. the entries
	     * without type ptrs.   It works without because the real library
	     * data is inserted ahead of, and so supercedes, the config data.
	     */

            /* Now reinsert the library with real type ptrs */
            for (apis = library->apis; (types = apis->types); apis++) {
		for (i = 0; types[i].type; i++) {
                    gvplugin_install(gvc,
				apis->api,
				types[i].type,
				types[i].quality,
				library->packagename,
				(*pnext)->path,
				&types[i]);
		}
            }
	    
	    /* Now search again for the specific plugin type */
	    pnext = &(gvc->apis[api]);
	    while (*pnext) {
		if (strcmp(s, (*pnext)->typestr) == 0) {
		    if (p) {
			if (strcmp(p, (*pnext)->packagename) == 0)
			    break;
		    }
		    else
			break;
		}
		pnext = &((*pnext)->next);
	    }
	    rv = *pnext;
        }
    }
    /* one last check for succesfull load */
    if ((*pnext) && (*pnext)->typeptr == NULL)
	rv = NULL;
    free(s);
    gvc->api[api] = rv;
    return rv;
}

/* string buffer management
	- FIXME - must have 20 solutions for this same thing */
static const char *append_buf(char sep, char *str, bool new)
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
	buf = grealloc(buf, bufsz);
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
    gvplugin_available_t **pnext, **plugin, **pprev;
    const char *buf = NULL;
    char *s, *p, *typestr_last;
    bool new = TRUE;

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

    if (p) {	/* if str contains a ':', and if we find a match for the type,
		   then just list the alternative paths for the plugin */
	pnext = plugin;
	pprev = NULL;
	while (*pnext) {
	    /* list only the matching type, and only once*/
	    if ((strcmp(s, (*pnext)->typestr) == 0)
		    && ! (pprev && strcmp((*pnext)->packagename, (*pprev)->packagename) == 0)) {
		/* list each member of the matching type as "type:path" */
		append_buf(' ', (*pnext)->typestr, new);
		buf = append_buf(':', (*pnext)->packagename, FALSE);
		new = FALSE;
	    }
	    pprev = pnext;
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
    if (!buf)
	buf = "";
    return buf;
}

void gvplugin_write_status(GVC_t * gvc)
{
    int api;

#ifdef ENABLE_LTDL
    if (Demand_Loading) {
        fprintf(stderr,"The plugin configuration file:\n\t%s\n", gvc->config_path);
        if (gvc->config_found)
	    fprintf(stderr,"\t\twas successfully loaded.\n");
        else
	    fprintf(stderr,"\t\twas not found or not usable. No on-demand plugins.\n");
    }
    else {
        fprintf(stderr,"Demand loading of plugins is disabled.\n");
    }
#endif

    for (api = 0; api < ARRAY_SIZE(api_names); api++) {
	fprintf(stderr,"    %s\t: %s\n", api_names[api], gvplugin_list(gvc, api, ":"));
    }
}
