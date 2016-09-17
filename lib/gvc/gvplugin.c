/* $Id$ $Revision$ */
/* vim:set shiftwidth=4 ts=8: */

/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: See CVS logs. Details at http://www.graphviz.org/
 *************************************************************************/

#include "config.h"

#include	<string.h>
#ifdef ENABLE_LTDL
#include	<ltdl.h>
#endif

#include	<agxbuf.h>
#include        "memory.h"
#include        "types.h"
#include        "gvplugin.h"
#include        "gvcjob.h"
#include        "gvcint.h"
#include        "gvcproc.h"
#include        "gvio.h"

#include	"const.h"

#ifndef HAVE_STRCASECMP
extern int strcasecmp(const char *s1, const char *s2);
#endif

#ifdef WIN32
#define strdup(x) _strdup(x)
#endif

/*
 * Define an apis array of name strings using an enumerated api_t as index.
 * The enumerated type is defined gvplugin.h.  The apis array is
 * inititialized here by redefining ELEM and reinvoking APIS.
 */
#define ELEM(x) #x,
static char *api_names[] = { APIS };    /* "render", "layout", ... */

#undef ELEM

/* translate a string api name to its type, or -1 on error */
api_t gvplugin_api(char *str)
{
    int api;

    for (api = 0; api < ARRAY_SIZE(api_names); api++) {
        if (strcmp(str, api_names[api]) == 0)
            return (api_t) api;
    }
    return -1;                  /* invalid api */
}

/* translate api_t into string name, or NULL */
char *gvplugin_api_name(api_t api)
{
    if (api >= ARRAY_SIZE(api_names))
        return NULL;
    return api_names[api];
}

/* install a plugin description into the list of available plugins
 * list is alpha sorted by type (not including :dependency), then
 * quality sorted within the type, then, if qualities are the same,
 * last install wins.
 */
boolean gvplugin_install(GVC_t * gvc, api_t api, const char *typestr,
                         int quality, gvplugin_package_t * package, gvplugin_installed_t * typeptr)
{
    gvplugin_available_t *plugin, **pnext;
#define TYPSIZ 63
    char *p, pins[TYPSIZ + 1], pnxt[TYPSIZ + 1];

    strncpy(pins, typestr, TYPSIZ);
    if ((p = strchr(pins, ':')))
        *p = '\0';

    /* point to the beginning of the linked list of plugins for this api */
    pnext = &(gvc->apis[api]);

    /* keep alpha-sorted and insert new duplicates ahead of old */
    while (*pnext) {
        strncpy(pnxt, (*pnext)->typestr, TYPSIZ);
        if ((p = strchr(pnxt, ':')))
            *p = '\0';
        if (strcmp(pins, pnxt) <= 0)
            break;
        pnext = &((*pnext)->next);
    }

    /* keep quality sorted within type and insert new duplicates ahead of old */
    while (*pnext) {
        strncpy(pnxt, (*pnext)->typestr, TYPSIZ);
        if ((p = strchr(pnxt, ':')))
            *p = '\0';
        if (strcmp(pins, pnxt) != 0)
            break;
        if (quality >= (*pnext)->quality)
            break;
        pnext = &((*pnext)->next);
    }

    plugin = GNEW(gvplugin_available_t);
    plugin->next = *pnext;
    *pnext = plugin;
    plugin->typestr = typestr;
    plugin->quality = quality;
    plugin->package = package;
    plugin->typeptr = typeptr;  /* null if not loaded */

    return TRUE;
}

/* Activate a plugin description in the list of available plugins.
 * This is used when a plugin-library loaded because of demand for
 * one of its plugins. It updates the available plugin data with
 * pointers into the loaded library.
 * NB the quality value is not replaced as it might have been
 * manually changed in the config file.
 */
static boolean gvplugin_activate(GVC_t * gvc, api_t api,
                                 const char *typestr, char *name, char *path, gvplugin_installed_t * typeptr)
{
    gvplugin_available_t **pnext;

    /* point to the beginning of the linked list of plugins for this api */
    pnext = &(gvc->apis[api]);

    while (*pnext) {
        if ((strcasecmp(typestr, (*pnext)->typestr) == 0)
            && (strcasecmp(name, (*pnext)->package->name) == 0)
            && ((*pnext)->package->path != 0)
            && (strcasecmp(path, (*pnext)->package->path) == 0)) {
            (*pnext)->typeptr = typeptr;
            return TRUE;
        }
        pnext = &((*pnext)->next);
    }
    return FALSE;
}

gvplugin_library_t *gvplugin_library_load(GVC_t * gvc, char *path)
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

    if (!gvc->common.demand_loading)
        return NULL;

    libdir = gvconfig_libdir(gvc);
    len = strlen(libdir) + 1 + strlen(path) + 1;
    if (len > lenp) {
        lenp = len + 20;
        if (p)
            p = grealloc(p, lenp);
        else
            p = gmalloc(lenp);
    }
#ifdef WIN32
    if (path[1] == ':') {
#else
    if (path[0] == '/') {
#endif
        strcpy(p, path);
    } else {
        strcpy(p, libdir);
        strcat(p, DIRSEP);
        strcat(p, path);
    }

    if (lt_dlinit()) {
        agerr(AGERR, "failed to init libltdl\n");
        return NULL;
    }
    hndl = lt_dlopen(p);
    if (!hndl) {
        agerr(AGWARN, "Could not load \"%s\" - %s\n", p, (char *) lt_dlerror());
        return NULL;
    }
    if (gvc->common.verbose >= 2)
        fprintf(stderr, "Loading %s\n", p);

    s = strrchr(p, DIRSEP[0]);
    len = strlen(s);
#if defined(WIN32) && !defined(__MINGW32__) && !defined(__CYGWIN__)
    if (len < strlen("/gvplugin_x")) {
#else
    if (len < strlen("/libgvplugin_x")) {
#endif
        agerr(AGERR, "invalid plugin path \"%s\"\n", p);
        return NULL;
    }
    sym = gmalloc(len + strlen(suffix) + 1);
#if defined(WIN32) && !defined(__MINGW32__) && !defined(__CYGWIN__)
    strcpy(sym, s + 1);         /* strip leading "/"  */
#else
    strcpy(sym, s + 4);         /* strip leading "/lib" or "/cyg" */
#endif
#if defined(__CYGWIN__) || defined(__MINGW32__)
    s = strchr(sym, '-');       /* strip trailing "-1.dll" */
#else
    s = strchr(sym, '.');       /* strip trailing ".so.0" or ".dll" or ".sl" */
#endif
    strcpy(s, suffix);          /* append "_LTX_library" */

    ptr = lt_dlsym(hndl, sym);
    if (!ptr) {
        agerr(AGERR, "failed to resolve %s in %s\n", sym, p);
        free(sym);
        return NULL;
    }
    free(sym);
    return (gvplugin_library_t *) (ptr);
#else
    agerr(AGERR, "dynamic loading not available\n");
    return NULL;
#endif
}


/* load a plugin of type=str
	the str can optionally contain one or more ":dependencies" 

	examples:
	        png
		png:cairo
        fully qualified:
		png:cairo:cairo
		png:cairo:gd
		png:gd:gd
      
*/
gvplugin_available_t *gvplugin_load(GVC_t * gvc, api_t api, const char *str)
{
    gvplugin_available_t **pnext, *rv;
    gvplugin_library_t *library;
    gvplugin_api_t *apis;
    gvplugin_installed_t *types;
#define TYPBUFSIZ 64
    char reqtyp[TYPBUFSIZ], typ[TYPBUFSIZ];
    char *reqdep, *dep = NULL, *reqpkg;
    int i;
    api_t apidep;

    if (api == API_device || api == API_loadimage)
        /* api dependencies - FIXME - find better way to code these *s */
        apidep = API_render;
    else
        apidep = api;

    strncpy(reqtyp, str, TYPBUFSIZ - 1);
    reqdep = strchr(reqtyp, ':');
    if (reqdep) {
        *reqdep++ = '\0';
        reqpkg = strchr(reqdep, ':');
        if (reqpkg)
            *reqpkg++ = '\0';
    } else
        reqpkg = NULL;

    /* iterate the linked list of plugins for this api */
    for (pnext = &(gvc->apis[api]); *pnext; pnext = &((*pnext)->next)) {
        strncpy(typ, (*pnext)->typestr, TYPBUFSIZ - 1);
        dep = strchr(typ, ':');
        if (dep)
            *dep++ = '\0';
        if (strcmp(typ, reqtyp))
            continue;           /* types empty or mismatched */
        if (dep && reqdep && strcmp(dep, reqdep))
            continue;           /* dependencies not empty, but mismatched */
        if (!reqpkg || strcmp(reqpkg, (*pnext)->package->name) == 0) {
            /* found with no packagename constraints, or with required matching packagname */

            if (dep && (apidep != api)) /* load dependency if needed, continue if can't find */
                if (!(gvplugin_load(gvc, apidep, dep)))
                    continue;
            break;
        }
    }
    rv = *pnext;

    if (rv && rv->typeptr == NULL) {
        library = gvplugin_library_load(gvc, rv->package->path);
        if (library) {

            /* Now activate the library with real type ptrs */
            for (apis = library->apis; (types = apis->types); apis++) {
                for (i = 0; types[i].type; i++) {
                    /* NB. quality is not checked or replaced
                     *   in case user has manually edited quality in config */
                    gvplugin_activate(gvc, apis->api, types[i].type, library->packagename, rv->package->path, &types[i]);
                }
            }
            if (gvc->common.verbose >= 1)
                fprintf(stderr, "Activated plugin library: %s\n", rv->package->path ? rv->package->path : "<builtin>");
        }
    }

    /* one last check for successfull load */
    if (rv && rv->typeptr == NULL)
        rv = NULL;

    if (rv && gvc->common.verbose >= 1)
        fprintf(stderr, "Using %s: %s:%s\n", api_names[api], rv->typestr, rv->package->name);

    gvc->api[api] = rv;
    return rv;
}

/* assemble a string list of available plugins 
 * non-re-entrant as character store is shared
 */
char *gvplugin_list(GVC_t * gvc, api_t api, const char *str)
{
    static int first = 1;
    gvplugin_available_t **pnext, **plugin;
    char *bp;
    char *s, *p, *q, *typestr_last;
    boolean new = TRUE;
    static agxbuf xb;

    /* check for valid str */
    if (!str)
        return NULL;

    if (first) {
        agxbinit(&xb, 0, 0);
        first = 0;
    }

    /* does str have a :path modifier? */
    s = strdup(str);
    p = strchr(s, ':');
    if (p)
        *p++ = '\0';

    /* point to the beginning of the linked list of plugins for this api */
    plugin = &(gvc->apis[api]);

    if (p) {                    /* if str contains a ':', and if we find a match for the type,
                                   then just list the alternative paths for the plugin */
        for (pnext = plugin; *pnext; pnext = &((*pnext)->next)) {
            q = strdup((*pnext)->typestr);
            if ((p = strchr(q, ':')))
                *p++ = '\0';
            /* list only the matching type, or all types if s is an empty string */
            if (!s[0] || strcasecmp(s, q) == 0) {
                /* list each member of the matching type as "type:path" */
                agxbputc(&xb, ' ');
                agxbput(&xb, (*pnext)->typestr);
                agxbputc(&xb, ':');
                agxbput(&xb, (*pnext)->package->name);
                new = FALSE;
            }
            free(q);
        }
    }
    free(s);
    if (new) {                  /* if the type was not found, or if str without ':',
                                   then just list available types */
        typestr_last = NULL;
        for (pnext = plugin; *pnext; pnext = &((*pnext)->next)) {
            /* list only one instance of type */
            q = strdup((*pnext)->typestr);
            if ((p = strchr(q, ':')))
                *p++ = '\0';
            if (!typestr_last || strcasecmp(typestr_last, q) != 0) {
                /* list it as "type"  i.e. w/o ":path" */
                agxbputc(&xb, ' ');
                agxbput(&xb, q);
                new = FALSE;
            }
            if (!typestr_last)
                free(typestr_last);
            typestr_last = q;
        }
        if (!typestr_last)
            free(typestr_last);
    }
    if (new)
        bp = "";
    else
        bp = agxbuse(&xb);
    return bp;
}

/* gvPluginList:
 * Return list of plugins of type kind.
 * The size of the list is stored in sz.
 * The caller is responsible for freeing the storage. This involves
 * freeing each item, then the list.
 * Returns NULL on error, or if there are no plugins.
 * In the former case, sz is unchanged; in the latter, sz = 0.
 *
 * At present, the str argument is unused, but may be used to modify
 * the search as in gvplugin_list above.
 */
char **gvPluginList(GVC_t * gvc, const char *kind, int *sz, const char *str)
{
    int api;
    gvplugin_available_t **pnext, **plugin;
    int cnt = 0;
    char **list = NULL;
    char *p, *q, *typestr_last;

    if (!kind)
        return NULL;
    for (api = 0; api < ARRAY_SIZE(api_names); api++) {
        if (!strcasecmp(kind, api_names[api]))
            break;
    }
    if (api == ARRAY_SIZE(api_names)) {
        agerr(AGERR, "unrecognized api name \"%s\"\n", kind);
        return NULL;
    }

    /* point to the beginning of the linked list of plugins for this api */
    plugin = &(gvc->apis[api]);
    typestr_last = NULL;
    for (pnext = plugin; *pnext; pnext = &((*pnext)->next)) {
        /* list only one instance of type */
        q = strdup((*pnext)->typestr);
        if ((p = strchr(q, ':')))
            *p++ = '\0';
        if (!typestr_last || strcasecmp(typestr_last, q) != 0) {
            list = RALLOC(cnt + 1, list, char *);
            list[cnt++] = q;
        }
        typestr_last = q;
    }

    *sz = cnt;
    return list;
}

void gvplugin_write_status(GVC_t * gvc)
{
    int api;

#ifdef ENABLE_LTDL
    if (gvc->common.demand_loading) {
        fprintf(stderr, "The plugin configuration file:\n\t%s\n", gvc->config_path);
        if (gvc->config_found)
            fprintf(stderr, "\t\twas successfully loaded.\n");
        else
            fprintf(stderr, "\t\twas not found or not usable. No on-demand plugins.\n");
    } else {
        fprintf(stderr, "Demand loading of plugins is disabled.\n");
    }
#endif

    for (api = 0; api < ARRAY_SIZE(api_names); api++) {
        if (gvc->common.verbose >= 2)
            fprintf(stderr, "    %s\t: %s\n", api_names[api], gvplugin_list(gvc, api, ":"));
        else
            fprintf(stderr, "    %s\t: %s\n", api_names[api], gvplugin_list(gvc, api, "?"));
    }

}

Agraph_t *gvplugin_graph(GVC_t * gvc)
{
    Agraph_t *g, *sg, *ssg;
    Agnode_t *n, *m, *loadimage_n, *renderer_n, *device_n, *textlayout_n, *layout_n;
    Agedge_t *e;
    Agsym_t *a;
    gvplugin_package_t *package;
    gvplugin_available_t **pnext;
    char bufa[100], *buf1, *buf2, bufb[100], *p, *q, *lq, *t;
    int api, neededge_loadimage, neededge_device;

    g = agopen("G", Agdirected, NIL(Agdisc_t *));
    agattr(g, AGRAPH, "label", "");
    agattr(g, AGRAPH, "rankdir", "");
    agattr(g, AGRAPH, "rank", "");
    agattr(g, AGRAPH, "ranksep", "");
    agattr(g, AGNODE, "label", NODENAME_ESC);
    agattr(g, AGNODE, "shape", "");
    agattr(g, AGNODE, "style", "");
    agattr(g, AGNODE, "width", "");
    agattr(g, AGEDGE, "style", "");

    a = agfindgraphattr(g, "rankdir");
    agxset(g, a, "LR");

    a = agfindgraphattr(g, "ranksep");
    agxset(g, a, "2.5");

    a = agfindgraphattr(g, "label");
    agxset(g, a, "Plugins");

    for (package = gvc->packages; package; package = package->next) {
        loadimage_n = renderer_n = device_n = textlayout_n = layout_n = NULL;
        neededge_loadimage = neededge_device = 0;
        strcpy(bufa, "cluster_");
        strcat(bufa, package->name);
        sg = agsubg(g, bufa, 1);
        a = agfindgraphattr(sg, "label");
        agxset(sg, a, package->name);
        strcpy(bufa, package->name);
        strcat(bufa, "_");
        buf1 = bufa + strlen(bufa);
        for (api = 0; api < ARRAY_SIZE(api_names); api++) {
            strcpy(buf1, api_names[api]);
            ssg = agsubg(sg, bufa, 1);
            a = agfindgraphattr(ssg, "rank");
            agxset(ssg, a, "same");
            strcat(buf1, "_");
            buf2 = bufa + strlen(bufa);
            for (pnext = &(gvc->apis[api]); *pnext; pnext = &((*pnext)->next)) {
                if ((*pnext)->package == package) {
                    t = q = strdup((*pnext)->typestr);
                    if ((p = strchr(q, ':')))
                        *p++ = '\0';
                    /* Now p = renderer, e.g. "gd"
                     * and q = device, e.g. "png"
                     * or  q = loadimage, e.g. "png" */
                    switch (api) {
                    case API_device:
                    case API_loadimage:
			/* draw device as box - record last device in plugin  (if any) in device_n */
			/* draw loadimage as box - record last loadimage in plugin  (if any) in loadimage_n */

                        /* hack for aliases */
			lq = q;
                        if (!strncmp(q, "jp", 2)) {
                            q = "jpg";                /* canonical - for node name */
			    lq = "jpeg\\njpe\\njpg";  /* list - for label */
			}
                        else if (!strncmp(q, "tif", 3)) {
                            q = "tif";
			    lq = "tiff\\ntif";
			}
                        else if (!strcmp(q, "x11") || !strcmp(q, "xlib")) {
                            q = "x11";
                            lq = "x11\\nxlib";
			}
                        else if (!strcmp(q, "dot") || !strcmp(q, "gv")) {
                            q = "gv";
                            lq = "gv\\ndot";
			}

                        strcpy(buf2, q);
                        n = agnode(ssg, bufa, 1);
                        a = agfindnodeattr(g, "label");
                        agxset(n, a, lq);
                        a = agfindnodeattr(g, "width");
                        agxset(n, a, "1.0");
                        a = agfindnodeattr(g, "shape");
			if (api == API_device) {
                            agxset(n, a, "box");
                            device_n = n;
			}
                        else {
                            agxset(n, a, "box");
                            loadimage_n = n;
			}
                        if (!(p && *p)) {
                            strcpy(bufb, "render_cg");
                            m = agfindnode(sg, bufb);
                            if (!m) {
                                m = agnode(sg, bufb, 1);
                                a = agfindgraphattr(g, "label");
                                agxset(m, a, "cg");
                            }
                            agedge(sg, m, n, NULL, 1);
                        }
                        break;
                    case API_render:
			/* draw renderers as ellipses - record last renderer in plugin (if any) in renderer_n */
                        strcpy(bufb, api_names[api]);
                        strcat(bufb, "_");
                        strcat(bufb, q);
                        renderer_n = n = agnode(ssg, bufb, 1);
                        a = agfindnodeattr(g, "label");
                        agxset(n, a, q);
                        break;
                    case API_textlayout:
			/* draw textlayout  as invtriangle - record last textlayout in plugin  (if any) in textlayout_n */
			/* FIXME? only one textlayout is loaded. Why? */
                        strcpy(bufb, api_names[api]);
                        strcat(bufb, "_");
                        strcat(bufb, q);
                        textlayout_n = n = agnode(ssg, bufb, 1);
                        a = agfindnodeattr(g, "shape");
                        agxset(n, a, "invtriangle");
                        a = agfindnodeattr(g, "label");
                        agxset(n, a, "T");
                        break;
                    case API_layout:
			/* draw textlayout  as hexagon - record last layout in plugin  (if any) in layout_n */
                        strcpy(bufb, api_names[api]);
                        strcat(bufb, "_");
                        strcat(bufb, q);
                        layout_n = n = agnode(ssg, bufb, 1);
                        a = agfindnodeattr(g, "shape");
                        agxset(n, a, "hexagon");
                        a = agfindnodeattr(g, "label");
                        agxset(n, a, q);
                        break;
                    default:
                        break;
                    }
                    free(t);
                }
            }
            // add some invisible nodes (if needed) and invisible edges to 
            //    improve layout of cluster
            if (api == API_loadimage && !loadimage_n) {
		neededge_loadimage = 1;
                strcpy(buf2, "invis");
                loadimage_n = n = agnode(ssg, bufa, 1);
                a = agfindnodeattr(g, "style");
                agxset(n, a, "invis");
                a = agfindnodeattr(g, "label");
                agxset(n, a, "");
                a = agfindnodeattr(g, "width");
                agxset(n, a, "1.0");

                strcpy(buf2, "invis_src");
                n = agnode(g, bufa, 1);
                a = agfindnodeattr(g, "style");
                agxset(n, a, "invis");
                a = agfindnodeattr(g, "label");
                agxset(n, a, "");

                e = agedge(g, n, loadimage_n, NULL, 1);
                a = agfindedgeattr(g, "style");
                agxset(e, a, "invis");
	    }
            if (api == API_render && !renderer_n) {
		neededge_loadimage = 1;
		neededge_device = 1;
                strcpy(buf2, "invis");
                renderer_n = n = agnode(ssg, bufa, 1);
                a = agfindnodeattr(g, "style");
                agxset(n, a, "invis");
                a = agfindnodeattr(g, "label");
                agxset(n, a, "");
	    }
            if (api == API_device && !device_n) {
		neededge_device = 1;
                strcpy(buf2, "invis");
                device_n = n = agnode(ssg, bufa, 1);
                a = agfindnodeattr(g, "style");
                agxset(n, a, "invis");
                a = agfindnodeattr(g, "label");
                agxset(n, a, "");
                a = agfindnodeattr(g, "width");
                agxset(n, a, "1.0");
	    }
        }
        if (neededge_loadimage) {
            e = agedge(sg, loadimage_n, renderer_n, NULL, 1);
            a = agfindedgeattr(g, "style");
            agxset(e, a, "invis");
        }
        if (neededge_device) {
            e = agedge(sg, renderer_n, device_n, NULL, 1);
            a = agfindedgeattr(g, "style");
            agxset(e, a, "invis");
        }
        if (textlayout_n) {
            e = agedge(sg, loadimage_n, textlayout_n, NULL, 1);
            a = agfindedgeattr(g, "style");
            agxset(e, a, "invis");
        }
        if (layout_n) {
            e = agedge(sg, loadimage_n, layout_n, NULL, 1);
            a = agfindedgeattr(g, "style");
            agxset(e, a, "invis");
        }
    }

    ssg = agsubg(g, "output_formats", 1);
    a = agfindgraphattr(ssg, "rank");
    agxset(ssg, a, "same");
    for (package = gvc->packages; package; package = package->next) {
        strcpy(bufa, package->name);
        strcat(bufa, "_");
        buf1 = bufa + strlen(bufa);
        for (api = 0; api < ARRAY_SIZE(api_names); api++) {
            strcpy(buf1, api_names[api]);
            strcat(buf1, "_");
            buf2 = bufa + strlen(bufa);
            for (pnext = &(gvc->apis[api]); *pnext; pnext = &((*pnext)->next)) {
                if ((*pnext)->package == package) {
                    t = q = strdup((*pnext)->typestr);
                    if ((p = strchr(q, ':')))
                        *p++ = '\0';
                    /* Now p = renderer, e.g. "gd"
                     * and q = device, e.g. "png"
                     * or  q = imageloader, e.g. "png" */

 		    /* hack for aliases */
                    lq = q;
                    if (!strncmp(q, "jp", 2)) {
                        q = "jpg";                /* canonical - for node name */
                        lq = "jpeg\\njpe\\njpg";  /* list - for label */
                    }
                    else if (!strncmp(q, "tif", 3)) {
                        q = "tif";
                        lq = "tiff\\ntif";
                    }
                    else if (!strcmp(q, "x11") || !strcmp(q, "xlib")) {
                        q = "x11";
                        lq = "x11\\nxlib";
                    }
                    else if (!strcmp(q, "dot") || !strcmp(q, "gv")) {
                        q = "gv";
                        lq = "gv\\ndot";
                    }

                    switch (api) {
                    case API_device:
                        strcpy(buf2, q);
                        n = agnode(g, bufa, 1);
                        strcpy(bufb, "output_");
                        strcat(bufb, q);
                        m = agfindnode(ssg, bufb);
                        if (!m) {
                            m = agnode(ssg, bufb, 1);
                            a = agfindnodeattr(g, "label");
                            agxset(m, a, lq);
                            a = agfindnodeattr(g, "shape");
                            agxset(m, a, "note");
                        }
                        e = agfindedge(g, n, m);
                        if (!e)
                            e = agedge(g, n, m, NULL, 1);
                        if (p && *p) {
                            strcpy(bufb, "render_");
                            strcat(bufb, p);
                            m = agfindnode(ssg, bufb);
                            if (!m)
                                m = agnode(g, bufb, 1);
                            e = agfindedge(g, m, n);
                            if (!e)
                                e = agedge(g, m, n, NULL, 1);
                        }
                        break;
                    case API_loadimage:
                        strcpy(buf2, q);
                        n = agnode(g, bufa, 1);
                        strcpy(bufb, "input_");
                        strcat(bufb, q);
                        m = agfindnode(g, bufb);
                        if (!m) {
                            m = agnode(g, bufb, 1);
                            a = agfindnodeattr(g, "label");
                            agxset(m, a, lq);
                            a = agfindnodeattr(g, "shape");
                            agxset(m, a, "note");
                        }
                        e = agfindedge(g, m, n);
                        if (!e)
                            e = agedge(g, m, n, NULL, 1);
                        strcpy(bufb, "render_");
                        strcat(bufb, p);
                        m = agfindnode(g, bufb);
                        if (!m)
                            m = agnode(g, bufb, 1);
                        e = agfindedge(g, n, m);
                        if (!e)
                            e = agedge(g, n, m, NULL, 1);
                        break;
                    default:
                        break;
                    }
                    free(t);
                }
            }
        }
    }

    return g;
}
