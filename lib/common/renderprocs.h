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

#ifndef GV_RENDERPROCS_H
#define GV_RENDERPROCS_H

#ifdef __cplusplus
extern "C" {
#endif

    typedef void (*nodesizefn_t) (Agnode_t *, boolean);

    extern void add_box(path *, box);
    extern point add_points(point, point);
    extern pointf add_pointfs(pointf, pointf);
    extern void arrow_flags(Agedge_t * e, int *sflag, int *eflag);
    extern boxf arrow_bb(pointf p, pointf u, double scale, int flag);
    extern void arrow_gen(GVJ_t * job, point p, point u, double scale,
			  int flag);
    extern double arrow_length(edge_t * e, int flag);
    extern int arrowEndClip(edge_t*, point*, int, int , bezier*, int eflag);
    extern int arrowStartClip(edge_t*, point* ps, int, int, bezier*, int sflag);
    extern void attach_attrs(Agraph_t *);
    extern void beginpath(path *, Agedge_t *, int, pathend_t *, boolean);
    extern pointf Bezier(pointf *, int, double, pointf *, pointf *);
    extern void bezier_clip(inside_t * inside_context,
			    boolean(*insidefn) (inside_t * inside_context,
						pointf p), pointf * sp,
			    boolean left_inside);
    extern shape_desc *bind_shape(char *name, node_t *);
    extern box boxof(int llx, int lly, int urx, int ury);
    extern boxf boxfof(double llx, double lly, double urx, double ury);
    extern box box_bb(box, box);
    extern boxf boxf_bb(boxf, boxf);
    extern box box_intersect(box, box);
    extern boxf boxf_intersect(boxf, boxf);
    extern boolean box_overlap(box, box);
    extern boolean boxf_overlap(boxf, boxf);
    extern boolean box_contains(box, box);
    extern boolean boxf_contains(boxf, boxf);
    extern void cat_libfile(FILE *, char **, char **);
    extern void clip_and_install(edge_t *, edge_t *, point *, int,
				 splineInfo *);
    extern char *canontoken(char *str);
    extern char* charsetToStr (int c);
    extern void colorxlate(char *str, color_t * color,
			   color_type_t target_type);
    extern void common_init_node(node_t * n);
    extern int common_init_edge(edge_t * e);
    extern void compute_bb(Agraph_t *);
    extern void config_codegen_builtins(GVC_t * gvc);
    extern point coord(node_t * n);
    extern pointf cvt2ptf(point);
    extern point cvt2pt(pointf);
    extern Agnode_t *dequeue(nodequeue *);
    extern void do_graph_label(graph_t * sg);
    extern point dotneato_closest(splines * spl, point p);
    extern void graph_init(graph_t * g);
    extern void dotneato_initialize(GVC_t * gvc, int, char **);
    extern void dotneato_usage(int);
    extern void dotneato_postprocess(Agraph_t *, nodesizefn_t);
    extern void dotneato_set_margins(GVC_t * gvc, Agraph_t *);
    extern void dotneato_terminate(GVC_t * gvc);
    extern void dotneato_write(GVC_t * gvc, graph_t *g);
    extern void dotneato_write_one(GVC_t * gvc, graph_t *g);
    extern double elapsed_sec(void);
    extern void enqueue(nodequeue *, Agnode_t *);
    extern void enqueue_neighbors(nodequeue *, Agnode_t *, int);
    extern void emit_background(GVJ_t * job, graph_t *g);
    extern void emit_clusters(GVJ_t * job, Agraph_t * g, int flags);
    extern void emit_edge_graphics(GVJ_t * job, edge_t * e);
    extern void emit_graph(GVJ_t * job, graph_t * g);
    extern void emit_label(GVJ_t * job, textlabel_t *, void *obj);
    extern int emit_once(char *message);
    extern void emit_jobs(GVC_t * gvc, graph_t *g);
    extern void emit_jobs_eof(GVC_t * gvc);
    extern void emit_textlines(GVJ_t*, int, textline_t*, pointf,
              double, char*, double, char*);
    extern void endpath(path *, Agedge_t *, int, pathend_t *, boolean);
    extern void epsf_init(node_t * n);
    extern void epsf_free(node_t * n);
    extern point exch_xy(point p);
    extern pointf exch_xyf(pointf p);
    extern void extend_attrs(GVJ_t * job, graph_t *g, int s_arrows, int e_arrows);
    extern shape_desc *find_user_shape(char *);
    extern box flip_rec_box(box b, point p);
    extern point flip_pt(point p, int rankdir);
    extern pointf flip_ptf(pointf p, int rankdir);
    extern void free_line(textline_t *);
    extern void free_label(textlabel_t *);
    extern void free_queue(nodequeue *);
    extern void free_ugraph(graph_t *);
    extern char *gd_alternate_fontlist(char *font);
    extern char *gd_textsize(textline_t * textline, char *fontname,
			     double fontsz, char **fontpath);
    extern point gd_user_shape_size(node_t * n, char *shapeimagefile);
    extern point ps_user_shape_size(node_t * n, char *shapeimagefile);
    extern point svg_user_shape_size(node_t * n, char *shapeimagefile);
    extern point quartz_user_shape_size(node_t * n, char *shapeimagefile);
    extern point gd_image_size(graph_t * n, char *shapeimagefile);
    extern point ps_image_size(graph_t * n, char *shapeimagefile);
    extern point svg_image_size(graph_t * n, char *shapeimagefile);
    extern point quartz_image_size(graph_t * n, char *shapeimagefile);
    extern void getdouble(graph_t * g, char *name, double *result);
    extern splines *getsplinepoints(edge_t * e);
    extern void global_def(char *,
			   Agsym_t * (*fun) (Agraph_t *, char *, char *));
    extern point image_size(graph_t * g, char *shapefile);
    extern void init_ugraph(graph_t * g);
    extern point invflip_pt(point p, int rankdir);
    extern boolean isPolygon(node_t *);
    extern int late_attr(void *, char *);
    extern int late_bool(void *, Agsym_t *, int);
    extern double late_double(void *, Agsym_t *, double, double);
    extern int late_int(void *, Agsym_t *, int, int);
    extern char *late_nnstring(void *, Agsym_t *, char *);
    extern char *late_string(void *, Agsym_t *, char *);
    extern char *latin1ToUTF8(char *);
    extern char *strdup_and_subst_graph(char *str, Agraph_t * g);
    extern char *strdup_and_subst_node(char *str, Agnode_t * n);
    extern char *strdup_and_subst_edge(char *str, Agedge_t * e);
    extern char *xml_string(char *s);
    extern void makeSelfEdge(path *, edge_t **, int, int, int, int,
			     splineInfo *);
    extern textlabel_t *make_label(int, char *, double, char *, char *,
				   graph_t *);
    extern int mapbool(char *);
    extern int maptoken(char *, char **, int *);
    extern void map_begin_cluster(graph_t * g);
    extern void map_begin_edge(Agedge_t * e);
    extern void map_begin_node(Agnode_t * n);
    extern void map_edge(Agedge_t *);
    extern point map_point(point);
    extern box mkbox(point, point);
    extern boxf mkboxf(pointf, pointf);
    extern point neato_closest(splines * spl, point p);
    extern bezier *new_spline(edge_t * e, int sz);
    extern nodequeue *new_queue(int);
    extern Agraph_t *next_input_graph(void);
    extern void osize_label(textlabel_t *, int *, int *, int *, int *);
    extern boolean overlap_edge(edge_t *e, boxf b);
    extern boolean overlap_node(node_t *n, boxf b);
    extern boolean overlap_label(textlabel_t *lp, boxf b);
    extern char **parse_style(char *s);
    extern void place_graph_label(Agraph_t *);
    extern void place_portlabel(edge_t * e, boolean head_p);
    extern point pointof(int, int);
    extern int processClusterEdges(graph_t * g);
    extern char *ps_string(char *s, int);
    extern int rank(graph_t * g, int balance, int maxiter);
    extern void* routesplinesinit(void);
    extern point *routesplines(void*, path *, int *);
    extern void routesplinesterm(void*);
    extern char *safefile(char *shapefilename);
    extern attrsym_t* safe_dcl(graph_t*, void*, char*, char*,
         attrsym_t * (*fun) (Agraph_t *, char *, char *));
    extern int selfRightSpace (edge_t* e);
    extern void setup_graph(GVC_t * gvc, graph_t * g);
    extern shape_kind shapeOf(node_t *);
    extern void shape_clip(node_t * n, point curve[4]);
    extern point spline_at_y(splines * spl, int y);
    extern void start_timer(void);
    extern double textwidth(textline_t * textline, char *fontname,
			    double fontsz);
    extern void translate_bb(Agraph_t *, int);
    extern Agnode_t *UF_find(Agnode_t *);
    extern void UF_remove(Agnode_t *, Agnode_t *);
    extern void UF_setname(Agnode_t *, Agnode_t *);
    extern void UF_singleton(Agnode_t *);
    extern Agnode_t *UF_union(Agnode_t *, Agnode_t *);
    extern void undoClusterEdges(graph_t * g);
    extern void updateBB(graph_t * g, textlabel_t * lp);
    extern void use_library(char *);
    extern char *username(void);
    extern point user_shape_size(node_t * n, char *shapefile);
    extern char* utf8ToLatin1 (char* ins);
    extern void write_attributed_dot(graph_t *g, FILE *f);
    extern void write_canonical_dot(graph_t *g, FILE *f);
    extern void write_extended_dot(GVJ_t * job, graph_t *g, FILE *f);
    extern void write_plain(GVJ_t * job, graph_t * g, FILE * f);
    extern void write_plain_ext(GVJ_t * job, graph_t * g, FILE * f);
    extern void *zmalloc(size_t);
    extern void *zrealloc(void *, size_t, size_t, size_t);
    extern void *gmalloc(size_t);
    extern void *grealloc(void *, size_t);

#if defined(_BLD_dot) && defined(_DLL)
#   define extern __EXPORT__
#endif
    extern point sub_points(point, point);
    extern pointf sub_pointfs(pointf, pointf);

    extern void toggle(int);
    extern int test_toggle(void);

#ifndef DISABLE_CODEGENS
#ifndef HAVE_GD_FREETYPE
    extern void initDPI(graph_t *);
    extern double textheight(int nlines, double fontsz);
    extern int builtinFontHt(double fontsz);
    extern int builtinFontWd(double fontsz);
#endif
    extern codegen_info_t *first_codegen(void);
    extern codegen_info_t *next_codegen(codegen_info_t * p);
#endif

#undef extern

#ifdef __cplusplus
}
#endif
#endif
