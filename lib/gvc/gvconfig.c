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
#include	<sys/types.h>
#include	<regex.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<glob.h>
#endif

#include        "memory.h"
#include        "const.h"
#include        "types.h"
#include        "graph.h"

#include	"gvplugin.h"
#include	"gvcjob.h"
#include	"gvcint.h"
#include        "gvcproc.h"

extern const bool Demand_Loading;

#ifdef WITH_CODEGENS
#ifdef QUARTZ_RENDER
#include <QuickTime/QuickTime.h>

    extern codegen_t QPDF_CodeGen, QEPDF_CodeGen, QBM_CodeGen;
#endif
    extern codegen_t HPGL_CodeGen, MIF_CodeGen, MP_CodeGen, PIC_CodeGen, DIA_CodeGen, VTX_CodeGen;
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

#ifdef ENABLE_LTDL
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

static int gvconfig_plugin_install_from_config(GVC_t * gvc, char *s)
{
    char *path, *packagename, *api, *type;
    api_t gv_api;
    int quality, rc;
    int nest = 0;

    separator(&nest, &s);
    while (*s) {
	path = token(&nest, &s);
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
		if (nest == 2) {
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
    gvplugin_installed_t *types;
    int i;

    for (apis = library->apis; (types = apis->types); apis++) {
	for (i = 0; types[i].type; i++) {
	    /* FIXME - should only install if dependencies on other plugins are satisfied */
	    /*         e.g. "render" "gtk" depends on "device" "gtk" */
	    /*         only need to check during actual loading, so no need to store dependencies in config */
	    gvplugin_install(gvc, apis->api, types[i].type,
			types[i].quality, library->packagename, path, &types[i]);
        }
    }
}

static void gvconfig_plugin_install_builtins(GVC_t * gvc)
{
    const lt_symlist_t *s;
    const char *name;

    s = lt_preloaded_symbols;
    for (s = lt_preloaded_symbols; (name = s->name); s++)
	if (name[0] == 'g' && strstr(name, "_LTX_library")) 
	    gvconfig_plugin_install_from_library(gvc, NULL,
		    (gvplugin_library_t *)(s->address));
}

#ifdef ENABLE_LTDL
static void gvconfig_write_library_config(char *path, gvplugin_library_t *library, FILE *f)
{
    gvplugin_api_t *apis;
    gvplugin_installed_t *types;
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

char * gvconfig_libdir(void)
{
    static char line[1024];
    static char *libdir;
    char *path, *tmp;
    FILE *f;

    if (!libdir) {

        /* this only works on linux, other systems will get GVLIBDIR only */
	libdir = GVLIBDIR;
        f = fopen ("/proc/self/maps", "r");
        if (f) {
            while (!feof (f)) {
	        if (!fgets (line, sizeof (line), f))
	            continue;
	        if (!strstr (line, " r-xp "))
	            continue;
		path = strchr (line, '/');
		if (!path)
		    continue;
		tmp = strstr (path, "/libgvc.");
		if (tmp) {
		    *tmp = 0;
		    /* Check for real /lib dir. Don't accept pre-install /.libs */
		    if (strcmp(strrchr(path,'/'), "/graphviz") != 0)
			continue;
		    libdir = path;
		    break;
	        }
            }
            fclose (f);
        }
    }
    return libdir;
}
#endif

#ifdef ENABLE_LTDL
static void config_rescan(GVC_t *gvc, char *config_path)
{
    FILE *f = NULL;
    glob_t globbuf;
    char *config_glob, *config_re, *path, *libdir;
    int i, rc, re_status;
    gvplugin_library_t *library;
    regex_t re;
    char *plugin_glob = "libgvplugin_*";
#if defined(DARWIN_DYLIB)
    char *plugin_re_beg = "[^0-9]\\.";
    char *plugin_re_end = "\\.dylib$";
#elif defined(__CYGWIN__)
    plugin_glob = "cyggvplugin_*";
    char *plugin_re_beg = "[^0-9]-";
    char *plugin_re_end = "\\.dll$"; 
#elif defined(__hpux__)
    char *plugin_re_beg = "\\.sl\\.";
    char *plugin_re_end = "$"; 
#elif defined(__hpux)   
    char *plugin_re_beg = "\\.sl\\.";
    char *plugin_re_end = "$"; 
#else
    /* Everyone else */
    char *plugin_re_beg = "\\.so\\.";
    char *plugin_re_end= "$";
#endif

    if (config_path) {
	f = fopen(config_path,"w");
	if (!f) {
	    agerr(AGERR,"failed to open %s for write.\n", config_path);
	}
    }

    libdir = gvconfig_libdir();

    config_re = gmalloc(strlen(plugin_re_beg) + 20 + strlen(plugin_re_end) + 1);
#ifdef GVPLUGIN_VERSION
    sprintf(config_re,"%s%d%s", plugin_re_beg, GVPLUGIN_VERSION, plugin_re_end);
#else
    sprintf(config_re,"%s[0-9]+%s", plugin_re_beg, plugin_re_end);
#endif

    if (regcomp(&re, config_re, REG_EXTENDED|REG_NOSUB) != 0) {
	agerr(AGERR,"cannot compile regular expression %s", config_re);
    }

    config_glob = gmalloc(strlen(libdir) + 1 + strlen(plugin_glob) + 1);
    strcpy(config_glob, libdir);
    strcat(config_glob, "/");
    strcat(config_glob, plugin_glob);

    /* load all libraries even if can't save config */
    rc = glob(config_glob, GLOB_NOSORT, NULL, &globbuf);
    if (rc == 0) {
	for (i = 0; i < globbuf.gl_pathc; i++) {
	    re_status = regexec(&re, globbuf.gl_pathv[i], (size_t) 0, NULL, 0);
	    if (re_status == 0) {
		library = gvplugin_library_load(gvc, globbuf.gl_pathv[i]);
		if (library) {
		    gvconfig_plugin_install_from_library(gvc, globbuf.gl_pathv[i], library);
		    path = strrchr(globbuf.gl_pathv[i],'/');
		    if (path)
			path++;
		    if (f && path) {
			gvconfig_write_library_config(path, library, f);
		    }
		}
	    }
	}
    }
    regfree(&re);
    globfree(&globbuf);
    free(config_glob);
    free(config_re);
    if (f)
	fclose(f);
}
#endif

#ifdef WITH_CODEGENS

#define MAX_CODEGENS 100

static codegen_info_t cg[MAX_CODEGENS] = {
    {&HPGL_CodeGen, "hpgl", HPGL},
    {&HPGL_CodeGen, "pcl", PCL},
    {&MIF_CodeGen, "mif", MIF},
    {&PIC_CodeGen, "pic", PIC_format},

#ifdef QUARTZ_RENDER
    {&QPDF_CodeGen, "pdf", QPDF},
    {&QEPDF_CodeGen, "epdf", QEPDF},
#endif                          /* QUARTZ_RENDER */

    {&VTX_CodeGen, "vtx", VTX},
    {&MP_CodeGen, "mp", METAPOST},
#ifdef HAVE_LIBZ
    {&DIA_CodeGen, "dia", DIA},
#endif
    {NULL, NULL, 0}
};

codegen_info_t *first_codegen(void)
{
    return cg;
}

codegen_info_t *next_codegen(codegen_info_t * p)
{
    ++p;

#ifdef QUARTZ_RENDER
    static bool unscanned = TRUE;
    if (!p->name && unscanned) {
        /* reached end of codegens but haven't yet scanned for Quicktime codegens... */

        unscanned = FALSE;              /* don't scan again */

        ComponentDescription criteria;
        criteria.componentType = GraphicsExporterComponentType;
        criteria.componentSubType = 0;
        criteria.componentManufacturer = 0;
        criteria.componentFlags = 0;
        criteria.componentFlagsMask = graphicsExporterIsBaseExporter;

        codegen_info_t *next_cg;
        int next_id;
        Component next_component;

        /* make each discovered Quicktime format into a codegen */
        for (next_cg = p, next_id = QBM_FIRST, next_component =
             FindNextComponent(0, &criteria);
             next_cg < cg + MAX_CODEGENS - 1 && next_id <= QBM_LAST
             && next_component;
             ++next_cg, ++next_id, next_component =
             FindNextComponent(next_component, &criteria)) {
            next_cg->cg = &QBM_CodeGen;
            next_cg->id = next_id;
            next_cg->info = next_component;

            /* get four chars of extension, trim and convert to lower case */
            char extension[5];
            GraphicsExportGetDefaultFileNameExtension((GraphicsExportComponent) next_component, (OSType *) & extension);
            extension[4] = '\0';

            char *extension_ptr;
            for (extension_ptr = extension; *extension_ptr;
                 ++extension_ptr)
                *extension_ptr =
                    *extension_ptr == ' ' ? '\0' : tolower(*extension_ptr);
            next_cg->name = strdup(extension);
        }

        /* add new sentinel at end of dynamic codegens */
        next_cg->cg = (codegen_t *) 0;
        next_cg->id = 0;
        next_cg->info = (void *) 0;
        next_cg->name = (char *) 0;
    }
#endif
    return p;
}
#endif

/*
  gvconfig - parse a config file and install the identified plugins
 */
void gvconfig(GVC_t * gvc, bool rescan)
{
#if 0
    gvplugin_library_t **libraryp;
#endif
#ifdef ENABLE_LTDL
    int sz, rc;
    struct stat config_st, libdir_st;
    FILE *f = NULL;
    char *config_text = NULL;
    char *libdir;
    char *config_file_name = "config";

#define MAX_SZ_CONFIG 100000
#endif
    
#ifdef WITH_CODEGENS
    codegen_info_t *p;

    for (p = cg; p->name; ++p)
        gvplugin_install(gvc, API_render, p->name, 0,
                        "cg", NULL, (gvplugin_installed_t *) p);
#endif

    /* builtins don't require LTDL */
    gvconfig_plugin_install_builtins(gvc);
   
    gvc->config_found = FALSE;
#ifdef ENABLE_LTDL
    if (Demand_Loading) {
        /* see if there are any new plugins */
        libdir = gvconfig_libdir();
        rc = stat(libdir, &libdir_st);
        if (rc == -1) {
    	/* if we fail to stat it then it probably doesn't exist so just fail silently */
    	return;
        }
    
        if (! gvc->config_path) {
            gvc->config_path = gmalloc(strlen(libdir) + 1 + strlen(config_file_name) + 1);
            strcpy(gvc->config_path, libdir);
            strcat(gvc->config_path, "/");
            strcat(gvc->config_path, config_file_name);
        }
    	
        if (rescan) {
    	config_rescan(gvc, gvc->config_path);
    	gvc->config_found = TRUE;
    	return;
        }
    
        /* load in the cached plugin library data */
    
        rc = stat(gvc->config_path, &config_st);
        if (rc == -1) {
    	/* silently return without setting gvc->config_found = TRUE */
    	return;
        }
        else if (config_st.st_size > MAX_SZ_CONFIG) {
    	agerr(AGERR,"%s is bigger than I can handle.\n", gvc->config_path);
        }
        else {
    	f = fopen(gvc->config_path,"r");
    	if (!f) {
    	    agerr (AGERR,"failed to open %s for read.\n", gvc->config_path);
    	}
    	else {
    	    config_text = gmalloc(config_st.st_size + 1);
    	    sz = fread(config_text, 1, config_st.st_size, f);
    	    if (sz == 0) {
    		agerr(AGERR,"%s is zero sized, or other read error.\n", gvc->config_path);
    		free(config_text);
    	    }
    	    else {
    	        gvc->config_found = TRUE;
    	        config_text[sz] = '\0';  /* make input into a null terminated string */
    	        rc = gvconfig_plugin_install_from_config(gvc, config_text);
    		/* NB. config_text not freed because we retain char* into it */
    	    }
    	}
    	if (f)
    	    fclose(f);
        }
    }
#endif
    gvtextlayout_select(gvc);   /* choose best available textlayout plugin immediately */
}
