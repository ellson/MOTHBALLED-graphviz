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

#include	"render.h"
#include	"gvc.h"
#include	"agxbuf.h"

#ifndef DISABLE_CODEGENS
FILE *Output_file;
int Output_lang;
#endif

int y_invert;			/* invert y in bounding box */
static int y_off;		/* ymin + ymax */
static double yf_off;		/* y_off in inches */
static int e_arrows;		/* graph has edges with end arrows */
static int s_arrows;		/* graph has edges with start arrows */

static void extend_attrs(GVC_t * gvc);

  /* macros for inverting the y coordinate with the bounding box */
#define Y(y) (y_invert ? (y_off - (y)) : (y))
#define YF(y) (y_invert ? (yf_off - (y)) : (y))

void dotneato_set_margins(GVC_t * gvc, graph_t * g)
{
    double xf, yf;
    char *p;
    int i;

    /* margins */
    if ((p = agget(g, "margin"))) {
	i = sscanf(p, "%lf,%lf", &xf, &yf);
	if (i > 0)
	    GD_drawing(g)->margin.x = GD_drawing(g)->margin.y = POINTS(xf);
	if (i > 1)
	    GD_drawing(g)->margin.y = POINTS(yf);
    } else {
	/* set default margins depending on format */
	switch (gvc->job->output_lang) {
	case GVRENDER_PLUGIN:
	    GD_drawing(g)->margin.x = GD_drawing(g)->margin.y =
		gvc->render_features->default_margin;
	    break;
	case GIF:
	case PNG:
	case JPEG:
	case WBMP:
	case GD:
	case memGD:
	case GD2:
	case ISMAP:
	case IMAP:
	case CMAP:
	case CMAPX:
	case VRML:
	case DIA:
	case SVG:
	case SVGZ:
	case QEPDF:
	    GD_drawing(g)->margin.x = GD_drawing(g)->margin.y =
		DEFAULT_EMBED_MARGIN;
	    break;
	case POSTSCRIPT:
	case PDF:
	case HPGL:
	case PCL:
	case MIF:
	case METAPOST:
	case FIG:
	case VTX:
	case ATTRIBUTED_DOT:
	case PLAIN:
	case PLAIN_EXT:
	case QPDF:
	    GD_drawing(g)->margin.x = GD_drawing(g)->margin.y =
		DEFAULT_MARGIN;
	    break;
	case CANONICAL_DOT:
	    break;
	default:
	    if (gvc->job->output_lang >= QBM_FIRST
		&& gvc->job->output_lang < QBM_LAST)
		GD_drawing(g)->margin.x = GD_drawing(g)->margin.y =
		    DEFAULT_EMBED_MARGIN;
	    break;
	}
    }
}

/* chkOrder:
 * Determine order of output.
 * Output usually in breadth first graph walk order
 */
static int chkOrder(graph_t * g)
{
    char *p = agget(g, "outputorder");
    if (p) {
	char c = *p;
	if ((c == 'n') && !strcmp(p + 1, "odesfirst"))
	    return EMIT_SORTED;
	if ((c == 'e') && !strcmp(p + 1, "dgesfirst"))
	    return EMIT_EDGE_SORTED;
    }
    return 0;
}

void dotneato_write_one(GVC_t * gvc, graph_t * g)
{
    int flags;

    gvc->g = g;
#ifndef DISABLE_CODEGENS
    Output_file = gvc->job->output_file;
    Output_lang = gvc->job->output_lang;
#endif
    dotneato_set_margins(gvc, g);
    emit_init(gvc, g);
    if (NOT(gvrender_features(gvc) & GVRENDER_DOES_MULTIGRAPH_OUTPUT_FILES)
#ifndef DISABLE_CODEGENS
/* FIXME - bad hack until feaures supported in codegens */
	&& gvc->codegen != &PS_CodeGen
#ifdef QUARTZ_RENDER
	&& gvc->codegen != &QPDF_CodeGen && gvc->codegen != &QEPDF_CodeGen
#endif
#endif
	)
	emit_reset(gvc, g);  /* FIXME - split into emit_init & page reset */
    switch (gvc->job->output_lang) {
    case GVRENDER_PLUGIN:
	flags = chkOrder(g);
	flags |= gvc->render_features->flags;
	gvemit_graph(gvc, g, flags);
	break;
    case POSTSCRIPT:
    case PDF:
    case HPGL:
    case PCL:
    case MIF:
    case PIC_format:
    case GIF:
    case PNG:
    case JPEG:
    case WBMP:
    case GD:
    case memGD:
    case GD2:
    case VRML:
    case METAPOST:
    case SVG:
    case SVGZ:
    case QPDF:
    case QEPDF:
	emit_graph(gvc, g, chkOrder(g));
	break;
    case ISMAP:
    case IMAP:
    case CMAP:
    case CMAPX:
	/* output in breadth first graph walk order, but 
	 * with nodes edges and nested clusters before
	 * clusters */
	emit_graph(gvc, g, EMIT_CLUSTERS_LAST);
	break;
    case FIG:
	/* output color definition objects first */
	emit_graph(gvc, g, EMIT_COLORS);
	break;
    case VTX:
	/* output sorted, i.e. all nodes then all edges */
	emit_graph(gvc, g, EMIT_SORTED);
	break;
    case DIA:
	/* output in preorder traversal of the graph */
	emit_graph(gvc, g, EMIT_PREORDER);
	break;
    case EXTENDED_DOT:
	attach_attrs(g);
	extend_attrs(gvc);
	agwrite(g, gvc->job->output_file);
	break;
    case ATTRIBUTED_DOT:
	attach_attrs(g);
	agwrite(g, gvc->job->output_file);
	break;
    case CANONICAL_DOT:
	if (HAS_CLUST_EDGE(g))
	    undoClusterEdges(g);
	agwrite(g, gvc->job->output_file);
	break;
    case PLAIN:
	/* attach_attrs(g);  */
	write_plain(gvc, gvc->job->output_file);
	break;
    case PLAIN_EXT:
	/* attach_attrs(g);  */
	write_plain_ext(gvc, gvc->job->output_file);
	break;
    default:
	if (gvc->job->output_lang >= QBM_FIRST
	    && gvc->job->output_lang < QBM_LAST)
	    emit_graph(gvc, g, chkOrder(g));
	break;

    }
    fflush(gvc->job->output_file);
    emit_deinit(gvc);
}

static FILE *file_select(char *str)
{
    FILE *rv;
    rv = fopen(str, "wb");
    if (rv == NULL) {
        perror(str);
        exit(1);
    }
    return rv;
}

void dotneato_write(GVC_t * gvc, graph_t * g)
{
    gvrender_job_t *job;

    for (job = gvrender_first_job(gvc); job; job = gvrender_next_job(gvc)) {
	if (!job->output_file) {	/* if not yet opened */
	    if (job->output_filename == NULL) {
		job->output_file = stdout;
	    } else {
		job->output_file = file_select(job->output_filename);
	    }
	    job->output_lang = gvrender_select(gvc, job->output_langname);
	    assert(job->output_lang != NO_SUPPORT); /* should have been verified already */
	}
	dotneato_write_one(gvc, g);
    }
}

void dotneato_eof(GVC_t * gvc)
{
    gvrender_job_t *job;

    for (job = gvrender_first_job(gvc); job; job = gvrender_next_job(gvc)) {
	if (job->output_file) {
	    emit_eof(gvc);
	    fclose(job->output_file);
	    job->output_file = NULL;
	}
    }
}

static void set_record_rects(node_t * n, field_t * f, agxbuf * xb)
{
    int i;
    char buf[BUFSIZ];

    if (f->n_flds == 0) {
	sprintf(buf, "%d,%d,%d,%d ",
		f->b.LL.x + ND_coord_i(n).x,
		Y(f->b.LL.y + ND_coord_i(n).y),
		f->b.UR.x + ND_coord_i(n).x,
		Y(f->b.UR.y + ND_coord_i(n).y));
	agxbput(xb, buf);
    }
    for (i = 0; i < f->n_flds; i++)
	set_record_rects(n, f->fld[i], xb);
}

static attrsym_t *safe_dcl(graph_t * g, void *obj, char *name, char *def,
			   attrsym_t * (*fun) (Agraph_t *, char *, char *))
{
    attrsym_t *a = agfindattr(obj, name);
    if (a == NULL)
	a = fun(g, name, def);
    return a;
}

/* setYInvert:
 * Set parameters used to flip coordinate system (y=0 at top).
 * Values do not need to be unset, since if y_invert is set, it's
 * set for * all graphs during current run, so each will 
 * reinitialize the values for its bbox.
 */
static void setYInvert(graph_t * g)
{
    if (y_invert) {
	y_off = GD_bb(g).UR.y + GD_bb(g).LL.y;
	yf_off = PS2INCH(y_off);
    }
}

void attach_attrs(graph_t * g)
{
    int i, j, sides;
    char buf[BUFSIZ];		/* Used only for small strings */
    unsigned char xbuffer[BUFSIZ];	/* Initial buffer for xb */
    agxbuf xb;
    node_t *n;
    edge_t *e;
    point pt;

    e_arrows = s_arrows = 0;
    setYInvert(g);
    agxbinit(&xb, BUFSIZ, xbuffer);
    safe_dcl(g, g->proto->n, "pos", "", agnodeattr);
    safe_dcl(g, g->proto->n, "rects", "", agnodeattr);
    N_width = safe_dcl(g, g->proto->n, "width", "", agnodeattr);
    N_height = safe_dcl(g, g->proto->n, "height", "", agnodeattr);
    safe_dcl(g, g->proto->e, "pos", "", agedgeattr);
    if (GD_has_labels(g) & EDGE_LABEL)
	safe_dcl(g, g->proto->e, "lp", "", agedgeattr);
    if (GD_has_labels(g) & HEAD_LABEL)
	safe_dcl(g, g->proto->e, "head_lp", "", agedgeattr);
    if (GD_has_labels(g) & TAIL_LABEL)
	safe_dcl(g, g->proto->e, "tail_lp", "", agedgeattr);
    if (GD_label(g)) {
	safe_dcl(g, g, "lp", "", agraphattr);
	if (GD_label(g)->text[0]) {
	    pt = GD_label(g)->p;
	    sprintf(buf, "%d,%d", pt.x, Y(pt.y));
	    agset(g, "lp", buf);
	}
    }
    safe_dcl(g, g, "bb", "", agraphattr);
    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	sprintf(buf, "%d,%d", ND_coord_i(n).x, Y(ND_coord_i(n).y));
	agset(n, "pos", buf);
	sprintf(buf, "%.2f", PS2INCH(ND_ht_i(n)));
	agxset(n, N_height->index, buf);
	sprintf(buf, "%.2f", PS2INCH(ND_lw_i(n) + ND_rw_i(n)));
	agxset(n, N_width->index, buf);
	if (strcmp(ND_shape(n)->name, "record") == 0) {
	    set_record_rects(n, ND_shape_info(n), &xb);
	    agxbpop(&xb);	/* get rid of last space */
	    agset(n, "rects", agxbuse(&xb));
	} else {
	    polygon_t *poly;
	    int i;
	    if (N_vertices && isPolygon(n)) {
		poly = (polygon_t *) ND_shape_info(n);
		sides = poly->sides;
		if (sides < 3) {
		    char *p = agget(n, "samplepoints");
		    if (p)
			sides = atoi(p);
		    else
			sides = 8;
		    if (sides < 3)
			sides = 8;
		}
		for (i = 0; i < sides; i++) {
		    if (i > 0)
			agxbputc(&xb, ' ');
		    if (poly->sides >= 3)
			sprintf(buf, "%.3f %.3f",
				PS2INCH(poly->vertices[i].x),
				YF(PS2INCH(poly->vertices[i].y)));
		    else
			sprintf(buf, "%.3f %.3f",
				ND_width(n) / 2.0 * cos(i /
							(double) sides *
							PI * 2.0),
				YF(ND_height(n) / 2.0 *
				   sin(i / (double) sides * PI * 2.0)));
		    agxbput(&xb, buf);
		}
		agxset(n, N_vertices->index, agxbuse(&xb));
	    }
	}
	if (State >= GVSPLINES) {
	    for (e = agfstout(g, n); e; e = agnxtout(g, e)) {
		if (ED_edge_type(e) == IGNORED)
		    continue;
		if (ED_spl(e) == NULL)
		    continue;	/* reported in postproc */
		for (i = 0; i < ED_spl(e)->size; i++) {
		    if (i > 0)
			agxbputc(&xb, ';');
		    if (ED_spl(e)->list[i].sflag) {
			s_arrows = 1;
			sprintf(buf, "s,%d,%d ",
				ED_spl(e)->list[i].sp.x,
				Y(ED_spl(e)->list[i].sp.y));
			agxbput(&xb, buf);
		    }
		    if (ED_spl(e)->list[i].eflag) {
			e_arrows = 1;
			sprintf(buf, "e,%d,%d ",
				ED_spl(e)->list[i].ep.x,
				Y(ED_spl(e)->list[i].ep.y));
			agxbput(&xb, buf);
		    }
		    for (j = 0; j < ED_spl(e)->list[i].size; j++) {
			if (j > 0)
			    agxbputc(&xb, ' ');
			pt = ED_spl(e)->list[i].list[j];
			sprintf(buf, "%d,%d", pt.x, Y(pt.y));
			agxbput(&xb, buf);
		    }
		}
		agset(e, "pos", agxbuse(&xb));
		if (ED_label(e)) {
		    pt = ED_label(e)->p;
		    sprintf(buf, "%d,%d", pt.x, Y(pt.y));
		    agset(e, "lp", buf);
		}
		if (ED_head_label(e)) {
		    pt = ED_head_label(e)->p;
		    sprintf(buf, "%d,%d", pt.x, Y(pt.y));
		    agset(e, "head_lp", buf);
		}
		if (ED_tail_label(e)) {
		    pt = ED_tail_label(e)->p;
		    sprintf(buf, "%d,%d", pt.x, Y(pt.y));
		    agset(e, "tail_lp", buf);
		}
	    }
	}
    }
    rec_attach_bb(g);
    agxbfree(&xb);

    if (HAS_CLUST_EDGE(g))
	undoClusterEdges(g);
}

void rec_attach_bb(graph_t * g)
{
    int c;
    char buf[32];
    point pt;

    sprintf(buf, "%d,%d,%d,%d", GD_bb(g).LL.x, Y(GD_bb(g).LL.y),
	    GD_bb(g).UR.x, Y(GD_bb(g).UR.y));
    agset(g, "bb", buf);
    if (GD_label(g) && GD_label(g)->text[0]) {
	pt = GD_label(g)->p;
	sprintf(buf, "%d,%d", pt.x, Y(pt.y));
	agset(g, "lp", buf);
    }
    for (c = 1; c <= GD_n_cluster(g); c++)
	rec_attach_bb(GD_clust(g)[c]);
}

static char *getoutputbuffer(char *str)
{
    static char *rv;
    static int len;
    int req;

    req = MAX(2 * strlen(str) + 2, BUFSIZ);
    if (req > len) {
	rv = ALLOC(req, rv, char);
	len = req;
    }
    return rv;
}


static char *canonical(char *str)
{
    return agstrcanon(str, getoutputbuffer(str));
}

static void writenodeandport(FILE * fp, node_t * node, char *port)
{
    char *name;
    if (IS_CLUST_NODE(node))
	name = strchr(node->name, ':') + 1;
    else
	name = node->name;
    fprintf(fp, "%s", canonical(name));	/* slimey i know */
    if (port && *port)
	fprintf(fp, ":%s", canonical(port));
}

/* FIXME - there must be a proper way to get port info - these are 
 * supposed to be private to libgraph - from libgraph.h */
#define TAILX 1
#define HEADX 2

/* _write_plain:
 */
void _write_plain(GVC_t * gvc, FILE * f, boolean extend)
{
    int i, j, splinePoints;
    char *tport, *hport;
    node_t *n;
    edge_t *e;
    bezier bz;
    point pt;
    graph_t *g = gvc->g;
    char *lbl;

//    setup_graph(gvc, g);
    setYInvert(g);
    pt = GD_bb(g).UR;
    fprintf(f, "graph %.3f %.3f %.3f\n", gvc->job->zoom, PS2INCH(pt.x), PS2INCH(pt.y));
    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	if (IS_CLUST_NODE(n))
	    continue;
	fprintf(f, "node %s ", canonical(n->name));
	printptf(f, ND_coord_i(n));
	lbl = ND_label(n)->text;
	if (lbl)
	    lbl = canonical(lbl);
	else
	    lbl = "\"\"";
	fprintf(f, " %.3f %.3f %s %s %s %s %s\n",
		ND_width(n), ND_height(n), lbl,
		late_nnstring(n, N_style, "solid"),
		ND_shape(n)->name,
		late_nnstring(n, N_color, DEFAULT_COLOR),
		late_nnstring(n, N_fillcolor, DEFAULT_FILL));
    }
    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	for (e = agfstout(g, n); e; e = agnxtout(g, e)) {
	    if (extend && e->attr) {
		tport = e->attr[TAILX];
		hport = e->attr[HEADX];
	    } else
		tport = hport = "";
	    if (ED_spl(e)) {
		splinePoints = 0;
		for (i = 0; i < ED_spl(e)->size; i++) {
		    bz = ED_spl(e)->list[i];
		    splinePoints += bz.size;
		}
		fprintf(f, "edge ");
		writenodeandport(f, e->tail, tport);
		fprintf(f, " ");
		writenodeandport(f, e->head, hport);
		fprintf(f, " %d", splinePoints);
		for (i = 0; i < ED_spl(e)->size; i++) {
		    bz = ED_spl(e)->list[i];
		    for (j = 0; j < bz.size; j++)
			printptf(f, bz.list[j]);
		}
	    }
	    if (ED_label(e)) {
		fprintf(f, " %s", canonical(ED_label(e)->text));
		printptf(f, ED_label(e)->p);
	    }
	    fprintf(f, " %s %s\n", late_nnstring(e, E_style, "solid"),
		    late_nnstring(e, E_color, DEFAULT_COLOR));
	}
    }
    fprintf(f, "stop\n");
}

void write_plain(GVC_t * gvc, FILE * f)
{
    _write_plain(gvc, f, FALSE);
}

void write_plain_ext(GVC_t * gvc, FILE * f)
{
    _write_plain(gvc, f, TRUE);
}

void printptf(FILE * f, point pt)
{
    fprintf(f, " %.3f %.3f", PS2INCH(pt.x), PS2INCH(Y(pt.y)));
}

static agxbuf outbuf;
static agxbuf charbuf;
static attrsym_t *g_draw;
static attrsym_t *g_l_draw;

#ifndef DISABLE_CODEGENS

static void xd_textline(point p, textline_t * line)
{
    char buf[BUFSIZ];
    int j;

    agxbputc(&charbuf, 'T');
    switch (line->just) {
    case 'l':
	j = -1;
	break;
    case 'r':
	j = 1;
	break;
    default:
    case 'n':
	j = 0;
	break;
    }
    sprintf(buf, " %d %d %d %d %d -", p.x, Y(p.y), j,
	    (int) line->width, (int) strlen(line->str));
    agxbput(&charbuf, buf);
    agxbput(&charbuf, line->str);
    agxbputc(&charbuf, ' ');
}

static void xd_ellipse(point p, int rx, int ry, int filled)
{
    char buf[BUFSIZ];

    agxbputc(&outbuf, (filled ? 'E' : 'e'));
    sprintf(buf, " %d %d %d %d ", p.x, Y(p.y), rx, ry);
    agxbput(&outbuf, buf);
}

static void points(char c, point * A, int n)
{
    char buf[BUFSIZ];
    int i;
    point p;

    agxbputc(&outbuf, c);
    sprintf(buf, " %d ", n);
    agxbput(&outbuf, buf);
    for (i = 0; i < n; i++) {
	p = A[i];
	sprintf(buf, "%d %d ", p.x, Y(p.y));
	agxbput(&outbuf, buf);
    }
}

static void xd_polygon(point * A, int n, int filled)
{
    points((filled ? 'P' : 'p'), A, n);
}

static void
xd_bezier(point * A, int n, int arrow_at_start, int arrow_at_end)
{
    points('B', A, n);
}

static void xd_polyline(point * A, int n)
{
    points('L', A, n);
}

static Agraph_t *cluster_g;

static void xd_begin_cluster(Agraph_t * sg)
{
    cluster_g = sg;
}

static void xd_end_cluster()
{
    agxset(cluster_g, g_draw->index, agxbuse(&outbuf));
    if (GD_label(cluster_g))
	agxset(cluster_g, g_l_draw->index, agxbuse(&charbuf));
}

codegen_t XDot_CodeGen = {
    0,				/* xd_reset */
    0, /* xd_begin_job */ 0,	/* xd_end_job */
    0, /* xd_begin_graph */ 0,	/* xd_end_graph */
    0, /* xd_begin_page */ 0,	/* xd_end_page */
    0, /* xd_begin_layer */ 0,	/* xd_end_layer */
    xd_begin_cluster, xd_end_cluster,
    0, /* xd_begin_nodes */ 0,	/* xd_end_nodes */
    0, /* xd_begin_edges */ 0,	/* xd_end_edges */
    0, /* xd_begin_node */ 0,	/* xd_node */
    0, /* xd_begin_edge */ 0,	/* xd_edge */
    0, /* xd_begin_context */ 0,	/* xd_context */
    0, /* xd_begin_anchor */ 0,	/* xd_anchor */
    0, /* xd_set_font */ xd_textline,
    0, /* xd_set_pencolor */ 0, /* xd_set_fillcolor */ 0,	/* xd_set_style */
    xd_ellipse, xd_polygon,
    xd_bezier, xd_polyline,
    0, /* xd_has_arrows */ 0,	/* xd_comment */
    0, /* xd_textsize */ 0, /* xd_user_shape */ 0	/* xd_usershapesize */
};

#endif

static int isInvis(char *style)
{
    char **styles = 0;
    char **sp;
    char *p;

    if (style[0]) {
	styles = parse_style(style);
	sp = styles;
	while ((p = *sp++)) {
	    if (streq(p, "invis"))
		return 1;
	}
    }
    return 0;
}

/* 
 * John M. suggests:
 * You might want to add four more:
 *
 * _ohdraw_ (optional head-end arrow for edges)
 * _ohldraw_ (optional head-end label for edges)
 * _otdraw_ (optional tail-end arrow for edges)
 * _otldraw_ (optional tail-end label for edges)
 * 
 * that would be generated when an additional option is supplied to 
 * dot, etc. and 
 * these would be the arrow/label positions to use if a user want to flip the 
 * direction of an edge (as sometimes is there want).
 */
static void extend_attrs(GVC_t * gvc)
{
    int i;
    bezier bz = { 0, 0, 0, 0 };
    double scale;
    node_t *n;
    edge_t *e;
    attrsym_t *n_draw = NULL;
    attrsym_t *n_l_draw = NULL;
    attrsym_t *e_draw = NULL;
    attrsym_t *h_draw = NULL;
    attrsym_t *t_draw = NULL;
    attrsym_t *e_l_draw = NULL;
    attrsym_t *hl_draw = NULL;
    attrsym_t *tl_draw = NULL;
    unsigned char buf[BUFSIZ];
    unsigned char cbuf[BUFSIZ];
    graph_t *g = gvc->g;

    if (GD_has_labels(g) & GRAPH_LABEL)
	g_l_draw = safe_dcl(g, g, "_ldraw_", "", agraphattr);
    if (GD_n_cluster(g))
	g_draw = safe_dcl(g, g, "_draw_", "", agraphattr);

    n_draw = safe_dcl(g, g->proto->n, "_draw_", "", agnodeattr);
    n_l_draw = safe_dcl(g, g->proto->n, "_ldraw_", "", agnodeattr);

    e_draw = safe_dcl(g, g->proto->e, "_draw_", "", agedgeattr);
    if (e_arrows)
	h_draw = safe_dcl(g, g->proto->e, "_hdraw_", "", agedgeattr);
    if (s_arrows)
	t_draw = safe_dcl(g, g->proto->e, "_tdraw_", "", agedgeattr);
    if (GD_has_labels(g) & EDGE_LABEL)
	e_l_draw = safe_dcl(g, g->proto->e, "_ldraw_", "", agedgeattr);
    if (GD_has_labels(g) & HEAD_LABEL)
	hl_draw = safe_dcl(g, g->proto->e, "_hldraw_", "", agedgeattr);
    if (GD_has_labels(g) & TAIL_LABEL)
	tl_draw = safe_dcl(g, g->proto->e, "_tldraw_", "", agedgeattr);

    agxbinit(&outbuf, BUFSIZ, buf);
    agxbinit(&charbuf, BUFSIZ, cbuf);
    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	if (ND_shape(n) && !isInvis(late_string(n, N_style, ""))) {
	    ND_shape(n)->fns->codefn(gvc, n);
	    agxset(n, n_draw->index, agxbuse(&outbuf));
	    agxset(n, n_l_draw->index, agxbuse(&charbuf));
	}
	if (State < GVSPLINES)
	    continue;
	for (e = agfstout(g, n); e; e = agnxtout(g, e)) {
	    if (ED_edge_type(e) == IGNORED)
		continue;
	    if (isInvis(late_string(e, E_style, "")))
		continue;
	    if (ED_spl(e) == NULL)
		continue;

	    scale = late_double(e, E_arrowsz, 1.0, 0.0);
	    for (i = 0; i < ED_spl(e)->size; i++) {
		bz = ED_spl(e)->list[i];
#ifndef DISABLE_CODEGENS
/* FIXME - why is this here? */
		xd_bezier(bz.list, bz.size, FALSE, FALSE);
#endif
	    }
	    agxset(e, e_draw->index, agxbuse(&outbuf));
	    for (i = 0; i < ED_spl(e)->size; i++) {
		if (bz.sflag) {
		    arrow_gen(gvc, bz.sp, bz.list[0], scale, bz.sflag);
		    agxset(e, t_draw->index, agxbuse(&outbuf));
		}
		if (bz.eflag) {
		    arrow_gen(gvc, bz.ep, bz.list[bz.size - 1], scale,
			      bz.eflag);
		    agxset(e, h_draw->index, agxbuse(&outbuf));
		}
	    }
	    if (ED_label(e)) {
		emit_label(gvc, ED_label(e), (void *) e);
		if (mapbool(late_string(e, E_decorate, "false"))
		    && ED_spl(e)) {
		    emit_attachment(gvc, ED_label(e), ED_spl(e));
		    agxbput(&charbuf, agxbuse(&outbuf));
		}
		agxset(e, e_l_draw->index, agxbuse(&charbuf));
	    }
	    if (ED_head_label(e)) {
		emit_label(gvc, ED_head_label(e), (void *) e);
		agxset(e, hl_draw->index, agxbuse(&charbuf));
	    }
	    if (ED_tail_label(e)) {
		emit_label(gvc, ED_tail_label(e), (void *) e);
		agxset(e, tl_draw->index, agxbuse(&charbuf));
	    }
	}
    }
    if (GD_label(g)) {
	emit_label(gvc, GD_label(g), (void *) g);
	agxset(g, g_l_draw->index, agxbuse(&charbuf));
    }
    emit_clusters(gvc, g, 0);
    agxbfree(&outbuf);
    agxbfree(&charbuf);
}
