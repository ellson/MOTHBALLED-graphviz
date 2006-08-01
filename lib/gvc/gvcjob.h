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

#ifndef GVCJOB_H
#define GVCJOB_H

#ifdef __cplusplus
extern "C" {
#endif

#include "gvcommon.h"

#define ARRAY_SIZE(A) (sizeof(A)/sizeof(A[0]))

    typedef struct gvdevice_engine_s gvdevice_engine_t;
    typedef struct gvrender_engine_s gvrender_engine_t;
    typedef struct gvlayout_engine_s gvlayout_engine_t;
    typedef struct gvtextlayout_engine_s gvtextlayout_engine_t;
    typedef struct gvloadimage_engine_s gvloadimage_engine_t;

    typedef enum { PEN_NONE, PEN_DASHED, PEN_DOTTED, PEN_SOLID } pen_type;
    typedef enum { FILL_NONE, FILL_SOLID } fill_type;
    typedef enum { FONT_REGULAR, FONT_BOLD, FONT_ITALIC } font_type;
#define PENWIDTH_NORMAL 1.
#define PENWIDTH_BOLD 2.
    typedef enum { GVATTR_STRING, GVATTR_BOOL, GVATTR_COLOR } gvattr_t;

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
#define GVRENDER_DOES_TRANSFORM (1<<11)
#define GVRENDER_DOES_LABELS (1<<12)
#define GVRENDER_DOES_MAPS (1<<13)
#define GVRENDER_DOES_MAP_RECTANGLE (1<<14)
#define GVRENDER_DOES_MAP_CIRCLE (1<<15)
#define GVRENDER_DOES_MAP_POLYGON (1<<16)
#define GVRENDER_DOES_MAP_ELLIPSE (1<<17)
#define GVRENDER_DOES_MAP_BSPLINE (1<<18)
#define GVRENDER_DOES_TOOLTIPS (1<<19)
#define GVRENDER_DOES_TARGETS (1<<20)
#define GVRENDER_DOES_Z (1<<21)

    typedef struct {
	int flags;
	double default_margin; /* graph units */
	pointf default_dpi;
	char **knowncolors;
	int sz_knowncolors;
	color_type_t color_type;
	char *device;
	char *loadimage_target;
    } gvrender_features_t;

    typedef struct {
	int flags;
    } gvdevice_features_t;

#define LAYOUT_USES_RANKDIR (1<<0)

    /* active plugin headers */
    typedef struct gvplugin_active_device_s {
        gvdevice_engine_t *engine;
        int id;
        gvdevice_features_t *features;
    } gvplugin_active_device_t;

    typedef struct gvplugin_active_render_s {
        gvrender_engine_t *engine;
        int id;
        gvrender_features_t *features;
        char *type;
    } gvplugin_active_render_t;

    typedef struct gvplugin_active_loadimage_t {
	gvloadimage_engine_t *engine;
	int id;
	char *type;
    } gvplugin_active_loadimage_t;

    typedef struct gv_argvlist_s {
	char **argv;
	int argc;
	int alloc;
    } gv_argvlist_t;

    typedef struct gvdevice_callbacks_s {
	void (*refresh) (GVJ_t * job);
        void (*button_press) (GVJ_t * job, int button, pointf pointer);
        void (*button_release) (GVJ_t * job, int button, pointf pointer);
        void (*motion) (GVJ_t * job, pointf pointer);
        void (*modify) (GVJ_t * job, char *name, char *value);
        void (*del) (GVJ_t * job);  /* can't use "delete" 'cos C++ stole it */
        void (*read) (GVJ_t * job, char *filename, char *layout);
        void (*layout) (GVJ_t * job, char *layout);
        void (*render) (GVJ_t * job, char *format, char *filename);
    } gvdevice_callbacks_t;

    typedef int (*gvevent_key_callback_t) (GVJ_t * job);

    typedef struct gvevent_key_binding_s {
	char *keystring;
	gvevent_key_callback_t callback;
    } gvevent_key_binding_t;

    typedef enum {MAP_RECTANGLE, MAP_CIRCLE, MAP_POLYGON, } map_shape_t;

    typedef enum {ROOTGRAPH_OBJTYPE, CLUSTER_OBJTYPE, NODE_OBJTYPE, EDGE_OBJTYPE} obj_type;
    typedef struct obj_state_s obj_state_t;

    struct obj_state_s {
	obj_state_t *parent;

	obj_type type;
	union {
	    graph_t *g;
	    graph_t *sg;  
	    node_t *n;
	    edge_t *e;
	} u;

	int oldstate;  /* FIXME - used by one of those other state stacks */

	gvcolor_t pencolor, fillcolor;
	pen_type pen;
	fill_type fill;
	double penwidth;
	char **rawstyle;

	double z, tail_z, head_z;   /* z depths for 2.5D renderers such as vrml */

	/* fully substituted text strings */
	char *label;
	char *taillabel;
	char *headlabel; 

	char *url;              /* if GVRENDER_DOES_MAPS */
	char *tailurl;
	char *headurl; 

	char *tooltip;          /* if GVRENDER_DOES_TOOLTIPS */
	char *tailtooltip;
	char *headtooltip; 
	bool explicit_tooltip;
	bool explicit_tailtooltip;
	bool explicit_headtooltip;

	char *target;           /* if GVRENDER_DOES_TARGETS */
	char *tailtarget;
	char *headtarget; 

	/* primary mapped region - node shape, edge label */
	map_shape_t url_map_shape; 
	int url_map_n;                  /* number of points for url map if GVRENDER_DOES_MAPS */
	pointf *url_map_p;

	/* additonal mapped regions for edges */
	int url_bsplinemap_poly_n;      /* number of polygons in url bspline map
					 if GVRENDER_DOES_MAPS && GVRENDER_DOES_MAP_BSPLINES */
	int *url_bsplinemap_n;          /* array of url_bsplinemap_poly_n ints 
					 of number of points in each polygon */
	pointf *url_bsplinemap_p;       /* all the polygon points */

	int tailurl_map_n;              /* tail label */
	pointf *tailurl_map_p;

	int headurl_map_n;		/* head label */
	pointf *headurl_map_p;

	int tailendurl_map_n;           /* tail end intersection with node */
	pointf *tailendurl_map_p;

	int headendurl_map_n;           /* head end intersection with node */
	pointf *headendurl_map_p;
    };


/* Note on units:
 *     points  - a physical distance (1/72 inch) unaffected by zoom or dpi.
 *     graph units - related to physical distance by zoom.  Equals points at zoom=1
 *     device units - related to physical distance in points by dpi/72
 */

    struct GVJ_s {
	GVC_t *gvc;		/* parent gvc */
	GVJ_t *next;		/* linked list of jobs */
	GVJ_t *next_active;	/* linked list of active jobs (e.g. multiple windows) */

	GVCOMMON_t *common;

	obj_state_t *obj;	/* objects can be nested (at least clusters can)
					so keep object state on a stack */
	char *input_filename;
	int graph_index;

	char *layout_type;

	char *output_filename;
	char *output_langname;
	FILE *output_file;
	int output_lang;

	gvplugin_active_render_t render;
	gvplugin_active_device_t device;
	gvplugin_active_loadimage_t loadimage;
	gvdevice_callbacks_t *callbacks;

	void *surface;		/* gd or cairo surface */
	bool external_surface;	/* surface belongs to caller */

        int flags;		/* emit_graph flags */

	int numLayers;		/* number of layers */
	int layerNum;		/* current layer - 1 based*/

	point 	pagesArraySize; /* 2D size of page array */
	point	pagesArrayFirst;/* 2D starting corner in */
	point	pagesArrayMajor;/* 2D major increment */
	point	pagesArrayMinor;/* 2D minor increment */
	point	pagesArrayElem; /* 2D coord of current page - 0,0 based */
        int	numPages;	/* number of pages */

	boxf    bb;		/* bb in graph units */
	pointf  pad;		/* padding around bb - graph units */
	boxf    clip;		/* clip region in graph units */
	boxf    pageBoxClip;	/* intersection of clip and pageBox in graph units */
	boxf	pageBox;	/* current page in graph units */
	pointf	pageOffset;	/* offset for current page in graph units */
	pointf	pageSize;	/* page size in graph units */
	pointf  focus;		/* viewport focus - graph units */

	double  zoom;		/* viewport zoom factor (points per graph unit) */
	int	rotation;	/* viewport rotation (degrees)  0=portrait, 90=landscape */

	pointf  view;		/* viewport size - points */
	boxf	canvasBox;	/* drawing area - points */
        pointf  margin;		/* job-specific margin - points */

	pointf	dpi;		/* device resolution device-units-per-inch */

        unsigned int width;     /* device width - device units */
        unsigned int height;    /* device height - device units */
	box     pageBoundingBox;/* rotated boundingBox - device units */
	box     boundingBox;    /* cumulative boundingBox over all pages - device units */

	pointf  scale;		/* composite device scale (zoom and dpi) */
	pointf  translation;    /* composite translation */
	pointf  compscale;	/* composite device scale incl: zoom, dpi, y_goes_down */

	bool	fit_mode,
		needs_refresh,
		click,
		has_grown,
		has_been_rendered;

	unsigned char button;   /* active button */
	pointf pointer;		/* pointer position in device units */
	pointf oldpointer;	/* old pointer position in device units */

	void *current_obj;      /* graph object that pointer is in currently */

	void *selected_obj;      /* graph object that has been selected */
					/* (e.g. button 1 clicked on current obj) */
	char *active_tooltip;		/* tooltip of active object - or NULL */
	char *selected_href;		/* href of selected object - or NULL */
	gv_argvlist_t selected_obj_type_name; /* (e.g. "edge" "node3" "e" "->" "node5" "") */
	gv_argvlist_t selected_obj_attributes; /* attribute triplets: name, value, type */
				/* e.g. "color", "red", GVATTR_COLOR,
					"style", "filled", GVATTR_BOOL, */

	void *window;		/* display-specific data for gvrender plugin */

        /* keybindings for keyboard events */
	gvevent_key_binding_t *keybindings;
	int numkeys;
	void *keycodes;

/* Must be last as separately compiled plugins are not compiled with WITH_CODEGENS */
#ifdef WITH_CODEGENS
	codegen_t *codegen;	/* current  codegen */
#endif
    };

#ifdef __cplusplus
}
#endif
#endif				/* GVCJOB_H */
