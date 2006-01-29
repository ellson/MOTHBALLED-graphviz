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

#include <stdio.h>
#include "geom.h"
#include "color.h"
#include "gvcext.h"

#define ARRAY_SIZE(A) (sizeof(A)/sizeof(A[0]))

    typedef struct gvdevice_engine_s gvdevice_engine_t;
    typedef struct gvrender_engine_s gvrender_engine_t;
    typedef struct gvlayout_engine_s gvlayout_engine_t;
    typedef struct gvtextlayout_engine_s gvtextlayout_engine_t;
    typedef struct gvusershape_engine_s gvusershape_engine_t;


    typedef enum { PEN_NONE, PEN_DASHED, PEN_DOTTED, PEN_SOLID } pen_type;
    typedef enum { FILL_NONE, FILL_SOLID } fill_type;
    typedef enum { FONT_REGULAR, FONT_BOLD, FONT_ITALIC } font_type;
#define PENWIDTH_NORMAL 1.
#define PENWIDTH_BOLD 2.
    typedef enum { GVATTR_STRING, GVATTR_BOOL, GVATTR_COLOR } gvattr_t;

    typedef struct {
	char *fontfam, fontopt;
	gvcolor_t pencolor, fillcolor;
	pen_type pen;
	fill_type fill;
	double penwidth;
	double fontsz;
    } gvstyle_t;

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

    typedef struct {
	int flags;
	double default_margin; /* graph units */
	int default_dpi;
	char **knowncolors;
	int sz_knowncolors;
	color_type_t color_type;
	char *device;
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

    /*
     * gv_matrix_t:   (compat with cairo_matrix_t)
     *
     * A #gv_matrix_t holds an affine transformation, such as a scale,
     * rotation, or shear, or a combination of those.
     */
    typedef struct gv_matrix_s {
	double xx; double yx;
	double xy; double yy;
	double x0; double y0;
    } gv_matrix_t;

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

    struct GVJ_s {
	GVC_t *gvc;		/* parent gvc */
	GVJ_t *next;		/* linked list of jobs */
	GVJ_t *next_active;	/* linked list of active jobs (e.g. multiple windows) */
	char *output_filename;
	char *output_langname;
	FILE *output_file;
	int output_lang;

	gvplugin_active_render_t render;
	gvplugin_active_device_t device;
	gvdevice_callbacks_t *callbacks;

#ifndef DISABLE_CODEGENS
	codegen_t *codegen;	/* current  codegen */
#endif

	void *surface;		/* gd or cairo surface */
	bool external_surface; /* surface belongs to caller */

	gvstyle_t *style;       /* active style from gvc->styles[] */

        int flags;		/* emit_graph flags */

        pointf margin;		 /* job-specific margin - graph units */

	int numLayers;		/* number of layers */
	int layerNum;		/* current layer - 1 based*/

	boxf	pageBox;	 /* current page in graph coords */
	pointf	pageOffset;	 /* offset for current page in graph coords */
	pointf	pageSize;	 /* page size in graph units */
	point 	pagesArraySize;  /* 2D size of page array */
	point	pagesArrayFirst; /* 2D starting corner in */
	point	pagesArrayMajor; /* 2D major increment */
	point	pagesArrayMinor; /* 2D minor increment */
	point	pagesArrayElem;  /* 2D coord of current page - 0,0 based */
        int	numPages;	 /* number of pages */

        unsigned int width;     /* device width in device units */
        unsigned int height;    /* device height in device units */
	box	boundingBox;	/* drawable region in device units */
	pointf	dpi;		/* device resolution device-units-per-inch */

	boxf bb;		/* bb in graph units */
	double zoom;		/* viewport zoom factor */
	int rotation;		/* viewport rotation  0=portrait, 1=landscape */
	pointf focus;		/* viewport focus in graph units */

	boxf clip;		/* clip region in graph units */
	boxf pageBoxClip;       /* intersection of clip and pageBox */

	gv_matrix_t transform;  /* transformation matrix for renderers that can use it */
	pointf compscale;	/* composite device scale incl: scale, zoom, dpi, y_goes_down */
	pointf offset;		/* composite translation */
	
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
    };

#ifdef __cplusplus
}
#endif
#endif				/* GVCJOB_H */
