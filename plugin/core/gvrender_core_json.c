/* $Id$ $Revision$ */
/* vim:set shiftwidth=4 ts=8: */

/*************************************************************************
 * Copyright (c) 2015 AT&T Intellectual Property 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: See CVS logs. Details at http://www.graphviz.org/
 *************************************************************************/

#include "config.h"

#ifdef _WIN32
#include <io.h>
#include "compat.h"
#endif

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "macros.h"
#include "const.h"
#include "xdot.h"

#include "gvplugin_render.h"
#include "gvplugin_device.h"
#include "agxbuf.h"
#include "utils.h"
#include "gvc.h"
#include "gvio.h"
#include "gvcint.h"

typedef enum {
	FORMAT_JSON,
	FORMAT_JSON0,
	FORMAT_DOT_JSON,
	FORMAT_XDOT_JSON,
} format_type;

typedef struct {
    int Level;
    boolean isLatin;
    boolean doXDot;
    boolean Attrs_not_written_flag;
} state_t;

typedef struct {
    Agrec_t h;
    int id;
} gvid_t;

#define ID "id"
#define ND_gid(n) (((gvid_t*)aggetrec(n, ID, FALSE))->id) 
#define ED_gid(n) (((gvid_t*)aggetrec(n, ID, FALSE))->id) 
#define GD_gid(n) (((gvid_t*)aggetrec(n, ID, FALSE))->id) 

#define IS_CLUSTER(s) (!strncmp(agnameof(s), "cluster", 7))

static void json_begin_graph(GVJ_t *job)
{
    if (job->render.id == FORMAT_JSON) {
	GVC_t* gvc = gvCloneGVC (job->gvc); 
	graph_t *g = job->obj->u.g;
	gvRender (gvc, g, "xdot", NULL); 
	gvFreeCloneGVC (gvc);
    }
    else if (job->render.id == FORMAT_JSON0) {
	attach_attrs(job->gvc->g);
    }
}

#define LOCALNAMEPREFIX		'%'

/* stoj:
 * Convert dot string to a valid json string embedded in double quotes.
 */
static char* stoj (char* ins, state_t* sp)
{
    char* s;
    char* input;
    static agxbuf xb;
    unsigned char c;

    if (sp->isLatin)
	input = latin1ToUTF8 (ins);
    else
	input = ins;

    if (xb.buf == NULL)
	agxbinit(&xb, BUFSIZ, NULL);
    for (s = input; (c = *s); s++) {
	switch (c) {
	case '"' :
	    agxbput(&xb, "\\\"");
	    break;
	case '\\' :
	    agxbput(&xb, "\\\\");
	    break;
	case '/' :
	    agxbput(&xb, "\\/");
	    break;
	case '\b' :
	    agxbput(&xb, "\\b");
	    break;
	case '\f' :
	    agxbput(&xb, "\\f");
	    break;
	case '\n' :
	    agxbput(&xb, "\\n");
	    break;
	case '\r' :
	    agxbput(&xb, "\\r");
	    break;
	case '\t' :
	    agxbput(&xb, "\\t");
	    break;
	default :
	    agxbputc(&xb, c);
	    break;
	}
    }
    s = agxbuse(&xb);

    if (sp->isLatin)
	free (input);
    return s;
}

static void indent(GVJ_t * job, int level)
{
    int i;
    for (i = level; i > 0; i--)
	gvputs(job, "  ");
}

static void set_attrwf(Agraph_t * g, int toplevel, int value)
{
    Agraph_t *subg;
    Agnode_t *n;
    Agedge_t *e;

    AGATTRWF(g) = value;
    for (subg = agfstsubg(g); subg; subg = agnxtsubg(subg)) {
	set_attrwf(subg, FALSE, value);
    }
    if (toplevel) {
	for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	    AGATTRWF(n) = value;
	    for (e = agfstout(g, n); e; e = agnxtout(g, e))
		AGATTRWF(e) = value;
	}
    }
}

static void write_polyline (GVJ_t * job, xdot_polyline* polyline)
{
    int i;
    int cnt = polyline->cnt;
    xdot_point* pts = polyline->pts;

    gvprintf(job, "\"points\": [");
    for (i = 0; i < cnt; i++) {
	if (i > 0) gvprintf(job, ",");
	gvprintf(job, "[%.03f,%.03f]", pts[i].x, pts[i].y);
    }
    gvprintf(job, "]\n");
}

static void write_stops (GVJ_t * job, int n_stops, xdot_color_stop* stp, state_t* sp)
{
    int i;

    gvprintf(job, "\"stops\": [");
    for (i = 0; i < n_stops; i++) {
	if (i > 0) gvprintf(job, ",");
	gvprintf(job, "{\"frac\": %.03f, \"color\": \"%s\"}",
	    stp[i].frac, stoj(stp[i].color, sp));
    }
    gvprintf(job, "]\n");
} 

static void write_radial_grad (GVJ_t * job, xdot_radial_grad* rg, state_t* sp)
{
    indent(job, sp->Level);
    gvprintf(job, "\"p0\": [%.03f,%.03f,%.03f],\n", rg->x0, rg->y0, rg->r0); 
    indent(job, sp->Level);
    gvprintf(job, "\"p1\": [%.03f,%.03f,%.03f],\n", rg->x1, rg->y1, rg->r1); 
    indent(job, sp->Level);
    write_stops (job, rg->n_stops, rg->stops, sp);
}

static void write_linear_grad (GVJ_t * job, xdot_linear_grad* lg, state_t* sp)
{
    indent(job, sp->Level);
    gvprintf(job, "\"p0\": [%.03f,%.03f],\n", lg->x0, lg->y0); 
    indent(job, sp->Level);
    gvprintf(job, "\"p1\": [%.03f,%.03f],\n", lg->x1, lg->y1); 
    indent(job, sp->Level);
    write_stops (job, lg->n_stops, lg->stops, sp);
}

static void write_xdot (xdot_op * op, GVJ_t * job, state_t* sp)
{
    indent(job, sp->Level++);
    gvputs(job, "{\n");
    indent(job, sp->Level);

    switch (op->kind) {
    case xd_filled_ellipse :
    case xd_unfilled_ellipse :
	gvprintf(job, "\"op\": \"%c\",\n", 
	    (op->kind == xd_filled_ellipse ? 'E' : 'e')); 
 	indent(job, sp->Level);
	gvprintf(job, "\"rect\": [%.03f,%.03f,%.03f,%.03f]\n", 
	    op->u.ellipse.x, op->u.ellipse.y, op->u.ellipse.w, op->u.ellipse.h);
	break;
    case xd_filled_polygon :
    case xd_unfilled_polygon :
	gvprintf(job, "\"op\": \"%c\",\n",
	    (op->kind == xd_filled_polygon ? 'P' : 'p')); 
 	indent(job, sp->Level);
	write_polyline (job, &op->u.polygon);
	break;
    case xd_filled_bezier :
    case xd_unfilled_bezier :
	gvprintf(job, "\"op\": \"%c\",\n",
	    (op->kind == xd_filled_bezier ? 'B' : 'b')); 
 	indent(job, sp->Level);
	write_polyline (job, &op->u.bezier);
	break;
    case xd_polyline :
	gvprintf(job, "\"op\": \"L\",\n"); 
 	indent(job, sp->Level);
	write_polyline (job, &op->u.polyline);
	break;
    case xd_text :
	gvprintf(job, "\"op\": \"T\",\n"); 
 	indent(job, sp->Level);
	gvprintf(job, "\"pt\": [%.03f,%.03f],\n", op->u.text.x, op->u.text.y); 
 	indent(job, sp->Level);
	gvprintf(job, "\"align\": \"%c\",\n",
	    (op->u.text.align == xd_left? 'l' : 
	    (op->u.text.align == xd_center ? 'c' : 'r'))); 
 	indent(job, sp->Level);
	gvprintf(job, "\"width\": %.03f,\n", op->u.text.width); 
 	indent(job, sp->Level);
	gvprintf(job, "\"text\": \"%s\"\n", stoj(op->u.text.text, sp));
	break;
    case xd_fill_color :
    case xd_pen_color :
	gvprintf(job, "\"op\": \"%c\",\n", 
	    (op->kind == xd_fill_color ? 'C' : 'c')); 
 	indent(job, sp->Level);
	gvprintf(job, "\"grad\": \"none\",\n"); 
 	indent(job, sp->Level);
	gvprintf(job, "\"color\": \"%s\"\n", stoj(op->u.color, sp));
	break;
    case xd_grad_pen_color :
    case xd_grad_fill_color :
	gvprintf(job, "\"op\": \"%c\",\n", 
	    (op->kind == xd_grad_fill_color ? 'C' : 'c')); 
 	indent(job, sp->Level);
	if (op->u.grad_color.type == xd_none) {
	    gvprintf(job, "\"grad\": \"none\",\n"); 
 	    indent(job, sp->Level);
	    gvprintf(job, "\"color\": \"%s\"\n", 
		stoj(op->u.grad_color.u.clr, sp));
	}
	else {
	    if (op->u.grad_color.type == xd_linear) {
		gvprintf(job, "\"grad\": \"linear\",\n"); 
		indent(job, sp->Level);
		write_linear_grad (job, &op->u.grad_color.u.ling, sp);
	    }
	    else {
		gvprintf(job, "\"grad\": \"radial\",\n"); 
		indent(job, sp->Level);
		write_radial_grad (job, &op->u.grad_color.u.ring, sp);
	    }
	}
	break;
    case xd_font :
	gvprintf(job, "\"op\": \"F\",\n");
 	indent(job, sp->Level);
	gvprintf(job, "\"size\": %.03f,\n", op->u.font.size); 
 	indent(job, sp->Level);
	gvprintf(job, "\"face\": \"%s\"\n", stoj(op->u.font.name, sp)); 
	break;
    case xd_style :
	gvprintf(job, "\"op\": \"S\",\n");
 	indent(job, sp->Level);
	gvprintf(job, "\"style\": \"%s\"\n", stoj(op->u.style, sp));
	break;
    case xd_image :
	break;
    case xd_fontchar :
	gvprintf(job, "\"op\": \"t\",\n");
 	indent(job, sp->Level);
	gvprintf(job, "\"fontchar\": %d\n", op->u.fontchar);
	break;
    }
    sp->Level--;
    indent(job, sp->Level);
    gvputs(job, "}");
}

static void write_xdots (char * val, GVJ_t * job, state_t* sp)
{
    xdot* cmds;
    int i;
    int not_first = 0;

    if (!val || (*val == '\0')) return;

    cmds = parseXDot(val);
    if (!cmds) {
	agerr(AGWARN, "Could not parse xdot \"%s\"\n", val);
	return;
    }

    gvputs(job, "\n");
    indent(job, sp->Level++);
    gvputs(job, "[\n");
    for (i = 0; i < cmds->cnt; i++) {
	if (not_first) 
	    gvputs(job, ",\n");
	else
	    not_first = 1;
	write_xdot (cmds->ops+i, job, sp);
    }
    sp->Level--;
    gvputs(job, "\n");
    indent(job, sp->Level);
    gvputs(job, "]");
    freeXDot(cmds);
}

static int isXDot (char* name)
{
  return ((*name++ == '_') &&
          (streq(name,"draw_") || streq(name,"ldraw_") ||
          streq(name,"hdraw_") || streq(name,"tdraw_") ||
          streq(name,"hldraw_") || streq(name,"tldraw_")));
}

static void write_attrs(Agobj_t * obj, GVJ_t * job, state_t* sp)
{
    Agraph_t* g = agroot(obj);
    int type = AGTYPE(obj);
    char* attrval;
    Agsym_t* sym = agnxtattr(g, type, NULL);
    if (!sym) return;

    for (; sym; sym = agnxtattr(g, type, sym)) {
	if (!(attrval = agxget(obj, sym))) continue;
	if ((*attrval == '\0') && !streq(sym->name, "label")) continue;
	gvputs(job, ",\n");
	indent(job, sp->Level);
	gvprintf(job, "\"%s\": ", stoj(sym->name, sp));
	if (sp->doXDot && isXDot(sym->name))
	    write_xdots(agxget(obj, sym), job, sp);
	else
	    gvprintf(job, "\"%s\"", stoj(agxget(obj, sym), sp));
    }
}

static void write_hdr(Agraph_t * g, GVJ_t * job, int top, state_t* sp)
{
    char *name;

    name = agnameof(g);
    indent(job, sp->Level);
    gvprintf(job, "\"name\": \"%s\"", stoj (name, sp));

    if (top) {
	gvputs(job, ",\n");
	indent(job, sp->Level);
	gvprintf(job, "\"directed\": %s,\n", (agisdirected(g)?"true":"false"));
	indent(job, sp->Level);
	gvprintf(job, "\"strict\": %s", (agisstrict(g)?"true":"false"));
    }
}

static void write_graph(Agraph_t * g, GVJ_t * job, int top, state_t* sp);

static void write_subg(Agraph_t * g, GVJ_t * job, state_t* sp)
{
    Agraph_t* sg;

    write_graph (g, job, FALSE, sp);
    for (sg = agfstsubg(g); sg; sg = agnxtsubg(sg)) {
	gvputs(job, ",\n");
	write_subg(sg, job, sp);
    }
}

/*
static int write_subgs(Agraph_t * g, GVJ_t * job, int top, state_t* sp)
{
    Agraph_t* sg;
    int not_first = 0;

    sg = agfstsubg(g);
    if (!sg) return 0;
   
    gvputs(job, ",\n");
    indent(job, sp->Level++);
    gvputs(job, "\"subgraphs\": [\n");
    for (; sg; sg = agnxtsubg(sg)) {
	if (not_first) 
	    gvputs(job, ",\n");
	else
	    not_first = 1;
	write_subg (sg, job, top, sp);
    }
    sp->Level--;
    gvputs(job, "\n");
    indent(job, sp->Level);
    gvputs(job, "]");
    return 1;
}
*/

static int write_subgs(Agraph_t * g, GVJ_t * job, int top, state_t* sp)
{
    Agraph_t* sg;
    int not_first = 0;

    sg = agfstsubg(g);
    if (!sg) return 0;
   
    gvputs(job, ",\n");
    indent(job, sp->Level++);
    if (top)
	gvputs(job, "\"objects\": [\n");
    else {
	gvputs(job, "\"subgraphs\": [\n");
	indent(job, sp->Level);
    }
    for (; sg; sg = agnxtsubg(sg)) {
	if (not_first) 
	    gvputs(job, ",\n");
	else
	    not_first = 1;
        if (top)
	    write_subg (sg, job, sp);
	else
	    gvprintf(job, "%d", GD_gid(sg));
    }
    if (!top) {
	sp->Level--;
	gvputs(job, "\n");
	indent(job, sp->Level);
	gvputs(job, "]");
    }

    return 1;
}

static void write_edge(Agedge_t * e, GVJ_t * job, int top, state_t* sp)
{
    if (top) {
	indent(job, sp->Level++);
	gvputs(job, "{\n");
	indent(job, sp->Level);
	gvprintf(job, "\"_gvid\": %d,\n", ED_gid(e));
	indent(job, sp->Level);
	gvprintf(job, "\"tail\": %d,\n", ND_gid(agtail(e)));
	indent(job, sp->Level);
	gvprintf(job, "\"head\": %d", ND_gid(aghead(e)));
    	write_attrs((Agobj_t*)e, job, sp);
	gvputs(job, "\n");
	sp->Level--;
	indent(job, sp->Level);
	gvputs(job, "}");
    }
    else {
	gvprintf(job, "%d", ED_gid(e));
    }
}

static int write_edges(Agraph_t * g, GVJ_t * job, int top, state_t* sp)
{
    Agnode_t* np;
    Agedge_t* ep;
    int not_first = 0;

    np = agfstnode(g);
    if (!np) return 0;
    ep = NULL;
    /* find a first edge */
    for (; np; np = agnxtnode(g,np)) {
	for (ep = agfstout(g, np); ep; ep = agnxtout(g,ep)) {
	    if (ep) break;
	}
	if (ep) break;
    }
    if (!ep) return 0;

    gvputs(job, ",\n");
    indent(job, sp->Level++);
    gvputs(job, "\"edges\": [\n");
    if (!top)
        indent(job, sp->Level);
    for (; np; np = agnxtnode(g,np)) {
	for (ep = agfstout(g, np); ep; ep = agnxtout(g,ep)) {
	    if (not_first) 
                if (top)
		    gvputs(job, ",\n");
                else
		    gvputs(job, ",");
	    else
		not_first = 1;
	    write_edge(ep, job, top, sp);
	}
    }
    sp->Level--;
    gvputs(job, "\n");
    indent(job, sp->Level);
    gvputs(job, "]");
    return 1;
}

static void write_node(Agnode_t * n, GVJ_t * job, int top, state_t* sp)
{
    if (top) {
	indent(job, sp->Level++);
	gvputs(job, "{\n");
	indent(job, sp->Level);
	gvprintf(job, "\"_gvid\": %d,\n", ND_gid(n));
	indent(job, sp->Level);
	gvprintf(job, "\"name\": \"%s\"", stoj (agnameof(n), sp));
    	write_attrs((Agobj_t*)n, job, sp);
	gvputs(job, "\n");
	sp->Level--;
	indent(job, sp->Level);
	gvputs(job, "}");
    }
    else {
	gvprintf(job, "%d", ND_gid(n));
    }
}

static int write_nodes(Agraph_t * g, GVJ_t * job, int top, int has_subgs, state_t* sp)
{
    Agnode_t* n;
    int not_first = 0;

    n = agfstnode(g);
    if (!n) {
	if (has_subgs && top) {
	    sp->Level--;
	    gvputs(job, "\n");
	    indent(job, sp->Level);
	    gvputs(job, "]");
	}
	return 0;
    }
    gvputs(job, ",\n");
    if (top) {
	if (!has_subgs) {
            indent(job, sp->Level++);
            gvputs(job, "\"objects\": [\n");
        }
    }
    else {
        indent(job, sp->Level++);
	gvputs(job, "\"nodes\": [\n");
	indent(job, sp->Level);
    }
    for (; n; n = agnxtnode(g, n)) {
	if (IS_CLUST_NODE(n)) continue;
	if (not_first) 
            if (top)
	        gvputs(job, ",\n");
            else
	        gvputs(job, ",");
	else
	    not_first = 1;
	write_node (n, job, top, sp);
    }
    sp->Level--;
    gvputs(job, "\n");
    indent(job, sp->Level);
    gvputs(job, "]");
    return 1;
}

typedef struct {
    Dtlink_t link;
    char* id;
    int v;
} intm;

static void freef(Dt_t * dt, intm * obj, Dtdisc_t * disc)
{
    free(obj->id);
    free(obj);
}

static Dtdisc_t intDisc = {
    offsetof(intm, id),
    -1,
    offsetof(intm, link),
    (Dtmake_f) NULL,
    (Dtfree_f) freef,
    (Dtcompar_f) NULL,
    0,
    0,
    0
};

#define NEW(t)          (t*)calloc(1,sizeof(t))

static int lookup (Dt_t* map, char* name)
{
    intm* ip = (intm*)dtmatch(map, name);    
    if (ip) return ip->v;
    else return -1;
}
 
static void insert (Dt_t* map, char* name, int v)
{
    intm* ip = (intm*)dtmatch(map, name);    

    if (ip) {
	if (ip->v != v)
	    agerr(AGWARN, "Duplicate cluster name \"%s\"\n", name);
	return;
    }
    ip = NEW(intm);
    ip->id = strdup(name);
    ip->v = v;
    dtinsert (map, ip);
}

static int label_subgs(Agraph_t* g, int lbl, Dt_t* map)
{
    Agraph_t* sg;

    if (g != agroot(g)) {
	GD_gid(g) = lbl++;
	if (IS_CLUSTER(g))
	    insert (map, agnameof(g), GD_gid(g)); 
    }
    for (sg = agfstsubg(g); sg; sg = agnxtsubg(sg)) {
	lbl = label_subgs(sg, lbl, map);
    }
    return lbl;
}


static void write_graph(Agraph_t * g, GVJ_t * job, int top, state_t* sp)
{
    Agnode_t* np; 
    Agedge_t* ep; 
    int ncnt = 0;
    int ecnt = 0;
    int sgcnt = 0;
    int has_subgs;
    Dt_t* map;

    if (top) {
	map = dtopen (&intDisc, Dtoset);
	aginit(g, AGNODE, ID, sizeof(gvid_t), FALSE);
	aginit(g, AGEDGE, ID, sizeof(gvid_t), FALSE);
	aginit(g, AGRAPH, ID, -((int)sizeof(gvid_t)), FALSE);
	sgcnt = label_subgs(g, sgcnt, map);
	for (np = agfstnode(g); np; np = agnxtnode(g,np)) {
	    if (IS_CLUST_NODE(np)) {
		ND_gid(np) = lookup(map, agnameof(np));
	    }
	    else {
		ND_gid(np) = sgcnt + ncnt++;
	    }
	    for (ep = agfstout(g, np); ep; ep = agnxtout(g,ep)) {
		ED_gid(ep) = ecnt++;
	    }
	}
	dtclose(map);
    }

    indent(job, sp->Level++);
    gvputs(job, "{\n");
    write_hdr(g, job, top, sp);
    write_attrs((Agobj_t*)g, job, sp);
    if (top) {
	gvputs(job, ",\n");
	indent(job, sp->Level);
	gvprintf(job, "\"_subgraph_cnt\": %d", sgcnt);
    } else {
	gvputs(job, ",\n");
	indent(job, sp->Level);
	gvprintf(job, "\"_gvid\": %d", GD_gid(g));
    }
    has_subgs = write_subgs(g, job, top, sp);
    write_nodes (g, job, top, has_subgs, sp);
    write_edges (g, job, top, sp);
    gvputs(job, "\n");
    sp->Level--;
    indent(job, sp->Level);
    if (top)
	gvputs(job, "}\n");
    else
	gvputs(job, "}");
}

typedef int (*putstrfn) (void *chan, const char *str);
typedef int (*flushfn) (void *chan);

static void json_end_graph(GVJ_t *job)
{
    graph_t *g = job->obj->u.g;
    state_t sp;
    Agiodisc_t* io_save;
    static Agiodisc_t io;

    if (io.afread == NULL) {
	io.afread = AgIoDisc.afread;
	io.putstr = (putstrfn)gvputs;
	io.flush = (flushfn)gvflush;
    }

    io_save = g->clos->disc.io;
    g->clos->disc.io = &io;

    set_attrwf(g, TRUE, FALSE);
    sp.Level = 0;
    sp.isLatin = (GD_charset(g) == CHAR_LATIN1);
    sp.doXDot = ((job->render.id == FORMAT_JSON) || (job->render.id == FORMAT_XDOT_JSON));
    sp.Attrs_not_written_flag = 0;
    write_graph(g, job, TRUE, &sp);
    /* agwrite(g, (FILE*)job); */
}

gvrender_engine_t json_engine = {
    0,				/* json_begin_job */
    0,				/* json_end_job */
    json_begin_graph,
    json_end_graph,
    0,				/* json_begin_layer */
    0,				/* json_end_layer */
    0,				/* json_begin_page */
    0,				/* json_end_page */
    0,				/* json_begin_cluster */
    0,				/* json_end_cluster */
    0,				/* json_begin_nodes */
    0,				/* json_end_nodes */
    0,				/* json_begin_edges */
    0,				/* json_end_edges */
    0,				/* json_begin_node */
    0,				/* json_end_node */
    0,				/* json_begin_edge */
    0,				/* json_end_edge */
    0,				/* json_begin_anchor */
    0,				/* json_end_anchor */
    0,				/* json_begin_label */
    0,				/* json_end_label */
    0,				/* json_textspan */
    0,				/* json_resolve_color */
    0,				/* json_ellipse */
    0,				/* json_polygon */
    0,				/* json_bezier */
    0,				/* json_polyline */
    0,				/* json_comment */
    0,				/* json_library_shape */
};

gvrender_features_t render_features_json1 = {
    GVRENDER_DOES_TRANSFORM,	/* not really - uses raw graph coords */  /* flags */
    0.,                         /* default pad - graph units */
    NULL,			/* knowncolors */
    0,				/* sizeof knowncolors */
    COLOR_STRING,		/* color_type */
};

gvrender_features_t render_features_json = {
    GVRENDER_DOES_TRANSFORM	/* not really - uses raw graph coords */
	| GVRENDER_DOES_MAPS
	| GVRENDER_DOES_TARGETS
	| GVRENDER_DOES_TOOLTIPS, /* flags */
    0.,                         /* default pad - graph units */
    NULL,			/* knowncolors */
    0,				/* sizeof knowncolors */
    COLOR_STRING,		/* color_type */
};

gvdevice_features_t device_features_json_nop = {
    LAYOUT_NOT_REQUIRED,	/* flags */
    {0.,0.},			/* default margin - points */
    {0.,0.},			/* default page width, height - points */
    {72.,72.},			/* default dpi */
};

gvdevice_features_t device_features_json = {
    0,				/* flags */
    {0.,0.},			/* default margin - points */
    {0.,0.},			/* default page width, height - points */
    {72.,72.},			/* default dpi */
};

gvplugin_installed_t gvrender_json_types[] = {
    {FORMAT_JSON, "json", 1, &json_engine, &render_features_json},
    {FORMAT_JSON0, "json0", 1, &json_engine, &render_features_json},
    {FORMAT_DOT_JSON, "dot_json", 1, &json_engine, &render_features_json},
    {FORMAT_XDOT_JSON, "xdot_json", 1, &json_engine, &render_features_json},
    {0, NULL, 0, NULL, NULL}
};

gvplugin_installed_t gvdevice_json_types[] = {
    {FORMAT_JSON, "json:json", 1, NULL, &device_features_json},
    {FORMAT_JSON0, "json0:json", 1, NULL, &device_features_json},
    {FORMAT_DOT_JSON, "dot_json:json", 1, NULL, &device_features_json_nop},
    {FORMAT_XDOT_JSON, "xdot_json:json", 1, NULL, &device_features_json_nop},
    {0, NULL, 0, NULL, NULL}
};
