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

#ifndef GVRENDERINT_H
#define GVRENDERINT_H

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct gvrender_job_s gvrender_job_t;

    struct gvrender_job_s {
	gvrender_job_t *next;
	char *output_filename;
	char *output_langname;
	FILE *output_file;
	void *interim_output;
	int output_lang;
    };

#define EMIT_SORTED (1<<0)
#define EMIT_COLORS (1<<1)
#define EMIT_CLUSTERS_LAST (1<<2)
#define EMIT_PREORDER (1<<3)
#define EMIT_EDGE_SORTED (1<<4)
#define GVRENDER_DOES_ARROWS (1<<5)
#define GVRENDER_DOES_LAYERS (1<<6)
#define GVRENDER_DOES_MULTIGRAPH_OUTPUT_FILES (1<<7)
#define GVRENDER_DOES_TRUECOLOR (1<<8)
#define GVRENDER_Y_GOES_DOWN (1<<9)
#define GVRENDER_X11_EVENTS (1<<10)

    typedef struct {
	int flags;
	int default_margin;
	double default_dpi;
	char **knowncolors;
	int sz_knowncolors;
	color_type_t color_type;
    } gvrender_features_t;


/* gv_plugin_t is a descriptor for available plugins; gvplugin_t is for installed plugins */
    typedef struct gv_plugin_s gv_plugin_t;

    struct gv_plugin_s {
	gv_plugin_t *next;
	char *typestr;
	int quality;
	char *path;
	gvplugin_type_t *typeptr;
    };

    typedef enum { PEN_NONE, PEN_DASHED, PEN_DOTTED, PEN_SOLID } pen_type;
    typedef enum { FILL_NONE, FILL_SOLID } fill_type;
    typedef enum { FONT_REGULAR, FONT_BOLD, FONT_ITALIC } font_type;
#define PENWIDTH_NORMAL 1
#define PENWIDTH_BOLD 2

    typedef struct {
	char *fontfam, fontopt;
	color_t pencolor, fillcolor;
	pen_type pen;
	fill_type fill;
	int penwidth;
	double fontsz;
    } gvstyle_t;

#define MAXNEST 4

    struct GVC_s {
	/* gvNEWcontext() */
	char *user;
	char **info;

	/* gvrender_config() */
	gvrender_job_t *jobs;	/* linked list of jobs */
	gvrender_job_t *job;	/* current job */
	void (*errorfn) (char *fmt, ...);

	/* plugins */
	gv_plugin_t *apis[NUM_APIS];	/* array of linked lists of plugins for each api */
	gv_plugin_t *api[NUM_APIS];	/* array of current plugins for each api */

	/* gvrender_begin_job() */
	gvrender_engine_t *render_engine;	/* current render engine */
	gvrender_features_t *render_features;	/* features of current render */
	int render_id;		/* internal id of current render */
#ifndef DISABLE_CODEGENS
	codegen_t *codegen;
#endif

	char *layout_type;      /* string name of layout type */
	gvlayout_engine_t *layout_engine;	/* current layout engine */
	int layout_id;		/* internal id of current layout */

	char **lib;
	point pages;
	int nPages;

	/* gvrender_begin_graph() */
	graph_t *g;
	void *surface;		/* gd or cairo surface */
	boolean external_surface;	/* surface belongs to caller */

	box bb;			/* graph bounding box (what units???) */
	point pb;		/* page size - including margins (inches) */
	point margin;		/* page margins (inches) */

	gvstyle_t styles[MAXNEST];	/* style stack */
	int SP;
	gvstyle_t *style;

	/* render defaults set from graph */
	color_t bgcolor;	/* background color */
        unsigned int width;
        unsigned int height;
	double dpi;		/* resolution dots-per-inch */
	int rot;		/* rotation */

	double zoom;		/* viewport zoom factor */
	pointf focus;		/* viewport focus in graph units */
	pointf compscale;	/* composite device scale incl: scale, zoom, dpi, y_goes_down */
	box clip;		/* clip region in graph units */

	/* gvrender_begin_page() */
	point page;
	point offset;
	int page_number;

	/* gvrender_begin_layer() */
	int layer;
	int nLayers;
    };

#ifdef __cplusplus
}
#endif
#endif				/* GVRENDERINT_H */
