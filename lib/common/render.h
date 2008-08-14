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

#ifndef GV_RENDER_H
#define GV_RENDER_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STDDEF_H
#include <stddef.h>
#endif
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include "types.h"
#include "macros.h"
#include "const.h"
#include "globals.h"
#include "memory.h"
#include "colorprocs.h"		/* must collow color.h (in types.h) */
#include "geomprocs.h"		/* must follow geom.h (in types.h) */
#include "agxbuf.h"
#ifdef WITH_CGRAPH
#include "cgraph.h"
#else
#include "graph.h"
#endif
#include "utils.h"		/* must follow types.h and agxbuf.h */
#include "gvplugin.h"		/* must follow gvcext.h (in types.h) */
#include "gvcjob.h"		/* must follow gvcext.h (in types.h) */
#include "gvcint.h"		/* must follow gvcext.h (in types.h) */
#include "gvcproc.h"		/* must follow gvcext.h (in types.h) */

    typedef struct epsf_s {
	int macro_id;
	point offset;
    } epsf_t;

    typedef void (*nodesizefn_t) (Agnode_t *, boolean);

    extern void add_box(path *, box);
    extern void arrow_flags(Agedge_t * e, int *sflag, int *eflag);
    extern boxf arrow_bb(pointf p, pointf u, double arrowsize, int flag);
    extern void arrow_gen(GVJ_t * job, emit_state_t emit_state, pointf p, pointf u,
			  double arrowsize, double penwidth, int flag);
    extern double arrow_length(edge_t * e, int flag);
    extern int arrowEndClip(edge_t*, point*, int, int , bezier*, int eflag);
    extern int arrowStartClip(edge_t*, point* ps, int, int, bezier*, int sflag);
    extern void attach_attrs(Agraph_t *);
    extern void beginpath(path *, Agedge_t *, int, pathend_t *, boolean);
    extern void bezier_clip(inside_t * inside_context,
			    boolean(*insidefn) (inside_t * inside_context,
						pointf p), pointf * sp,
			    boolean left_inside);
    extern shape_desc *bind_shape(char *name, node_t *);
    extern void clip_and_install(edge_t *, node_t *, point *, int,
				 splineInfo *);
    extern char* charsetToStr (int c);
    extern point coord(node_t * n);
    extern void do_graph_label(graph_t * sg);
    extern void graph_init(graph_t * g, boolean use_rankdir);
    extern void graph_cleanup(graph_t * g);
    extern void dotneato_args_initialize(GVC_t * gvc, int, char **);
    extern void dotneato_usage(int);
    extern void dotneato_postprocess(Agraph_t *);
    extern void dotneato_set_margins(GVC_t * gvc, Agraph_t *);
    extern void dotneato_write(GVC_t * gvc, graph_t *g);
    extern void dotneato_write_one(GVC_t * gvc, graph_t *g);
    extern double elapsed_sec(void);
    extern void emit_background(GVJ_t * job, graph_t *g);
    extern void emit_clusters(GVJ_t * job, Agraph_t * g, int flags);
    /* extern void emit_begin_edge(GVJ_t * job, edge_t * e, char**); */
    /* extern void emit_end_edge(GVJ_t * job); */
    extern void emit_graph(GVJ_t * job, graph_t * g);
    extern void emit_label(GVJ_t * job, emit_state_t emit_state, textlabel_t *);
    extern int emit_once(char *message);
    extern void emit_jobs_eof(GVC_t * gvc);
    extern void emit_map_rect(GVJ_t *job, point LL, point UR);
    extern void enqueue_neighbors(nodequeue *, Agnode_t *, int);
    extern void endpath(path *, Agedge_t *, int, pathend_t *, boolean);
    extern void epsf_init(node_t * n);
    extern void epsf_free(node_t * n);
    extern shape_desc *find_user_shape(char *);
    extern void free_line(textpara_t *);
    extern void free_label(textlabel_t *);
    extern void getdouble(graph_t * g, char *name, double *result);
    extern splines *getsplinepoints(edge_t * e);
    extern void global_def(char *,
			   Agsym_t * (*fun) (Agraph_t *, char *, char *));
    extern int gvRenderJobs (GVC_t * gvc, graph_t * g);
    extern void gv_free_splines(edge_t * e);
    extern void gv_cleanup_edge(Agedge_t * e);
    extern void gv_nodesize(Agnode_t * n, boolean flip);
    extern void gv_cleanup_node(Agnode_t * n);
    extern int initMapData (GVJ_t*, char*, char*, char*, char*, void*);
    extern boolean isPolygon(node_t *);
    extern char *strdup_and_subst_obj(char *str, void *obj);
    extern char *xml_string(char *s);
    extern char *xml_url_string(char *s);
    extern void makeSelfEdge(path *, edge_t **, int, int, int, int,
			     splineInfo *);
    extern textlabel_t *make_label(graph_t *g, int html, char *str,
				   double fontsize, char *fontname, char *fontcolor);
    extern bezier *new_spline(edge_t * e, int sz);
    extern void osize_label(textlabel_t *, int *, int *, int *, int *);
    extern char **parse_style(char *s);
    extern void place_graph_label(Agraph_t *);
    extern void place_portlabel(edge_t * e, boolean head_p);
    extern char *ps_string(char *s, int);
    extern void pop_obj_state(GVJ_t *job);
    extern obj_state_t* push_obj_state(GVJ_t *job);
    extern int rank(graph_t * g, int balance, int maxiter);
    extern void resolvePorts (edge_t* e);
    extern void round_corners(GVJ_t*, char*, char*, pointf*, int, int);
    extern void routesplinesinit(void);
    extern point *routesplines(path *, int *);
    extern void routesplinesterm(void);
    extern point *routepolylines(path* pp, int* npoints);
    extern int selfRightSpace (edge_t* e);
    extern void setup_graph(GVC_t * gvc, graph_t * g);
    extern shape_kind shapeOf(node_t *);
    extern void shape_clip(node_t * n, point curve[4]);
    extern void size_label (graph_t* g, textlabel_t* rv);
    extern void start_timer(void);
    extern pointf textsize(graph_t *g, textpara_t * para, char *fontname, double fontsize);
    extern void translate_bb(Agraph_t *, int);
    extern void update_bb_bz(boxf *bb, pointf *cp);
    extern void write_attributed_dot(graph_t *g, FILE *f);
    extern void write_canonical_dot(graph_t *g, FILE *f);
    extern void write_plain(GVJ_t * job, graph_t * g, FILE * f, boolean extend);

#if defined(_BLD_dot) && defined(_DLL)
#   define extern __EXPORT__
#endif

#ifdef WITH_CODEGENS
    extern codegen_info_t *first_codegen(void);
    extern codegen_info_t *next_codegen(codegen_info_t * p);
#endif

#undef extern

#ifdef __cplusplus
}
#endif

#endif
