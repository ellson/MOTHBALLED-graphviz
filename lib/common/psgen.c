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

/*
    J$: added `pdfmark' URL embedding.  PostScript rendered from
        dot files with URL attributes will get active PDF links
        from Adobe's Distiller.
 */
#define	PDFMAX	3240		/*  Maximum size of Distiller's PDF canvas  */

#include	"render.h"
#include	"gvc.h"
#include	"ps.h"
#include	"utils.h"

#ifndef MSWIN32
#include <unistd.h>
#endif

#include <sys/stat.h>
#include <stdio.h>

extern void epsf_define(FILE * of);
extern void ps_freeusershapes(void);
extern ps_image_t *ps_usershape(char *shapeimagefile);

static int N_pages, Cur_page;
/* static 	point	Pages; */
static box PB;
static int onetime = TRUE;
static node_t *Curnode;		/* for user shapes */

static char *Fill = "fill\n";
static char *Stroke = "stroke\n";
static char *Newpath_Moveto = "newpath %d %d moveto\n";
static char **U_lib;

typedef struct grcontext_t {
    char *pencolor, *fillcolor, *font;
    char invis;
    double size;
} grcontext_t;

#define STACKSIZE 8
static grcontext_t S[STACKSIZE];
static int SP = 0;

static void ps_reset(void)
{
    onetime = TRUE;
}

static void
ps_begin_job(FILE * ofp, graph_t * g, char **lib, char *user, char *info[],
	     point pages)
{
    /* Pages = pages; */
    U_lib = lib;
    /* wrong when drawing more than one than one graph - use (atend) */
    N_pages = pages.x * pages.y;
    Cur_page = 0;
    fprintf(Output_file, "%%!PS-Adobe-2.0\n");
    fprintf(Output_file, "%%%%Creator: %s version %s (%s)\n",
	    info[0], info[1], info[2]);
    fprintf(Output_file, "%%%%For: %s\n", user);
    fprintf(Output_file, "%%%%Title: %s\n", g->name);
    fprintf(Output_file, "%%%%Pages: (atend)\n");

    /* remainder is emitted by first begin_graph */
}

static void ps_end_job(void)
{
    fprintf(Output_file, "%%%%Trailer\n");
    fprintf(Output_file, "%%%%Pages: %d\n", Cur_page);
    fprintf(Output_file, "end\nrestore\n");
    fprintf(Output_file, "%%%%EOF\n");
}

static void ps_comment(void *obj, attrsym_t * sym)
{
    char *str;
    str = late_string(obj, sym, "");
    if (str[0])
	fprintf(Output_file, "%% %s\n", str);
}

static void ps_begin_graph(GVC_t * gvc, graph_t * g, box bb, point pb)
{
    char *s;
    static char setupLatin1 = FALSE;

    PB = bb;
    if (onetime) {
	fprintf(Output_file, "%%%%BoundingBox: %d %d %d %d\n",
		bb.LL.x - 1, bb.LL.y - 1, bb.UR.x + 1, bb.UR.y + 1);
	ps_comment(g, agfindattr(g, "comment"));
	fprintf(Output_file, "%%%%EndComments\nsave\n");
	cat_libfile(Output_file, U_lib, ps_txt);
	epsf_define(Output_file);

	/*  Set base URL for relative links (for Distiller >= 3.0)  */
	if (((s = agget(g, "href")) && s[0])
	    || ((s = agget(g, "URL")) && s[0])) {
	    fprintf(Output_file,
		    "[ {Catalog} << /URI << /Base (%s) >> >>\n"
		    "/PUT pdfmark\n", s);
	}
    }
    if (GD_has_Latin1char(g) && !setupLatin1) {
	fprintf(Output_file, "setupLatin1\n");	/* as defined in ps header */
	setupLatin1 = TRUE;
    }
}

static void ps_end_graph(void)
{
    ps_freeusershapes();
    onetime = FALSE;
}

static void
ps_begin_page(graph_t * g, point page, double scale, int rot, point offset)
{
    point sz;

    Cur_page++;
    sz = sub_points(PB.UR, PB.LL);
    fprintf(Output_file, "%%%%Page: %d %d\n", Cur_page, Cur_page);
    fprintf(Output_file, "%%%%PageBoundingBox: %d %d %d %d\n",
	    PB.LL.x, PB.LL.y, PB.UR.x + 1, PB.UR.y + 1);
    fprintf(Output_file, "%%%%PageOrientation: %s\n",
	    (rot ? "Landscape" : "Portrait"));
    fprintf(Output_file, "gsave\n%d %d %d %d boxprim clip newpath\n",
	    PB.LL.x - 1, PB.LL.y - 1, sz.x + 2, sz.y + 2);
    fprintf(Output_file, "%d %d translate\n", PB.LL.x, PB.LL.y);
    if (rot)
	fprintf(Output_file, "gsave %d %d translate %d rotate\n",
		PB.UR.x - PB.LL.x, 0, rot);
    fprintf(Output_file, "%d %d %d beginpage\n", page.x, page.y, N_pages);
    if (rot)
	fprintf(Output_file, "grestore\n");
    if (scale != 1.0)
	fprintf(Output_file, "%.4f set_scale\n", scale);
    fprintf(Output_file, "%d %d translate %d rotate\n", offset.x, offset.y,
	    rot);
    assert(SP == 0);
    S[SP].font = S[SP].pencolor = S[SP].fillcolor = "";
    S[SP].size = 0.0;

    /*  Define the size of the PS canvas  */
    if (Output_lang == PDF) {
	if (PB.UR.x >= PDFMAX || PB.UR.y >= PDFMAX)
	    agerr(AGWARN,
		  "canvas size (%d,%d) exceeds PDF limit (%d)\n"
		  "\t(suggest setting a bounding box size, see dot(1))\n",
		  PB.UR.x, PB.UR.y, PDFMAX);
	fprintf(Output_file, "[ /CropBox [%d %d %d %d] /PAGES pdfmark\n",
		PB.LL.x, PB.LL.y, PB.UR.x + 1, PB.UR.y + 1);
    }
}

static void ps_end_page(void)
{
    /* the showpage is really a no-op, but at least one PS processor
     * out there needs to see this literal token.  endpage does the real work.
     */
    fprintf(Output_file, "endpage\nshowpage\ngrestore\n");
    fprintf(Output_file, "%%%%PageTrailer\n");
    fprintf(Output_file, "%%%%EndPage: %d\n", Cur_page);
    assert(SP == 0);
}

static void ps_begin_layer(char *s, int n, int Nlayers)
{
    fprintf(Output_file, "%d %d setlayer\n", n, Nlayers);
}

static void ps_begin_cluster(graph_t * g)
{
    fprintf(Output_file, "%% %s\n", g->name);

    /*  Embed information for Distiller to generate hyperlinked PDF  */
    map_begin_cluster(g);
}

static void ps_begin_node(node_t * n)
{
    Curnode = n;
    fprintf(Output_file, "\n%%\t%s\n", n->name);
    ps_comment(n, N_comment);

    /*  Embed information for Distiller to generate hyperlinked PDF  */
    map_begin_node(n);
}

static void ps_begin_edge(edge_t * e)
{
    fprintf(Output_file, "\n%%\t%s -> %s\n", e->tail->name, e->head->name);
    ps_comment(e, E_comment);

    /*  Embed information for Distiller, so it can generate hyperactive PDF  */
    map_begin_edge(e);
}


static void ps_begin_context(void)
{
    fprintf(Output_file, "gsave 10 dict begin\n");
    if (SP == STACKSIZE - 1)
	agerr(AGWARN, "psgen stk ovfl\n");
    else {
	SP++;
	S[SP] = S[SP - 1];
    }
}

static void ps_end_context(void)
{
    if (SP == 0)
	agerr(AGWARN, "psgen stk undfl\n");
    else
	SP--;
    fprintf(Output_file, "end grestore\n");
}

static void ps_set_font(char *name, double size)
{
    if (strcmp(S[SP].font, name) || (size != S[SP].size)) {
	fprintf(Output_file, "%.2f /%s set_font\n", size, name);
	S[SP].font = name;
	S[SP].size = size;
    }
}

static void ps_set_color(char *name)
{
    static char *op[] = { "graph", "node", "edge", "sethsb" };
    color_t color;

    colorxlate(name, &color, HSV_DOUBLE);
    fprintf(Output_file, "%.3f %.3f %.3f %scolor\n",
	    color.u.HSV[0], color.u.HSV[1], color.u.HSV[2], op[Obj]);
}

static void ps_set_pencolor(char *name)
{
    if (streq(name, "transparent"))
	S[SP].pencolor = "";
    else if (strcmp(name, S[SP].pencolor)) {
	ps_set_color(name);	/* change pen color immediately */
	S[SP].pencolor = name;
    }
}

static void ps_set_fillcolor(char *name)
{
    /* defer changes to fill color to shape */
    if (streq(name, "transparent"))
	S[SP].fillcolor = "";
    else
	S[SP].fillcolor = name;
}

static void ps_set_style(char **s)
{
    char *line, *p;

    while ((p = line = *s++)) {
	while (*p)
	    p++;
	p++;
	while (*p) {
	    fprintf(Output_file, "%s ", p);
	    while (*p)
		p++;
	    p++;
	}
	if (streq(line, "invis"))
	    S[SP].invis = TRUE;
	fprintf(Output_file, "%s\n", line);
    }
}

char *ps_string(char *s)
{
    static char *buf = NULL;
    static int bufsize = 0;
    int pos = 0;
    char *p;

    if (!buf) {
	bufsize = 64;
	buf = N_GNEW(bufsize, char);
    }

    p = buf;
    *p++ = LPAREN;
    pos++;
    while (*s) {
	if (pos > (bufsize - 8)) {
	    bufsize *= 2;
	    buf = grealloc(buf, bufsize);
	    p = buf + pos;
	}
	if ((*s == LPAREN) || (*s == RPAREN) || (*s == '\\')) {
	    *p++ = '\\';
	    pos++;
	}
	*p++ = *s++;
	pos++;
    }
    *p++ = RPAREN;
    *p = '\0';
    return buf;
}

static void ps_textline(point p, textline_t * line)
{
    double adj;

    if (S[SP].invis)
	return;
    if (*S[SP].pencolor == '\0')
	return;
    if (line->xshow) {
	switch (line->just) {
	case 'l':
	    break;
	case 'r':
	    p.x -= line->width;
	    break;
	default:
	case 'n':
	    p.x -= line->width / 2;
	    break;
	}
	fprintf(Output_file, "%d %d moveto\n%s\n[%s]\nxshow\n",
		p.x, p.y, ps_string(line->str), line->xshow);
    } else {
	switch (line->just) {
	case 'l':
	    adj = 0.0;
	    break;
	case 'r':
	    adj = -1.0;
	    break;
	default:
	case 'n':
	    adj = -0.5;
	    break;
	}
	fprintf(Output_file, "%d %d moveto %.1f %.1f %s alignedtext\n",
		p.x, p.y, line->width, adj, ps_string(line->str));
    }
}

static void
ps_bezier(point * A, int n, int arrow_at_start, int arrow_at_end)
{
    int j;
    if (S[SP].invis)
	return;
    if (*S[SP].pencolor == '\0')
	return;
    if (arrow_at_start || arrow_at_end)
	agerr(AGERR, "ps_bezier illegal arrow args\n");
    fprintf(Output_file, Newpath_Moveto, A[0].x, A[0].y);
    for (j = 1; j < n; j += 3)
	fprintf(Output_file, "%d %d %d %d %d %d curveto\n",
		A[j].x, A[j].y, A[j + 1].x, A[j + 1].y, A[j + 2].x,
		A[j + 2].y);
    fprintf(Output_file, Stroke);
}

static void ps_polygon(point * A, int n, int filled)
{
    int j;

    if (S[SP].invis)
	return;
    if (filled && *S[SP].fillcolor) {
	ps_set_color(S[SP].fillcolor);
	fprintf(Output_file, Newpath_Moveto, A[0].x, A[0].y);
	for (j = 1; j < n; j++)
	    fprintf(Output_file, "%d %d lineto\n", A[j].x, A[j].y);
	fprintf(Output_file, "closepath\n");
	fprintf(Output_file, Fill);
	if (*S[SP].pencolor)
	    ps_set_color(S[SP].pencolor);
    }
    if (*S[SP].pencolor == '\0')
	return;
    fprintf(Output_file, Newpath_Moveto, A[0].x, A[0].y);
    for (j = 1; j < n; j++)
	fprintf(Output_file, "%d %d lineto\n", A[j].x, A[j].y);
    fprintf(Output_file, "closepath\n");
    fprintf(Output_file, Stroke);
}

static void ps_ellipse(point p, int rx, int ry, int filled)
{
    if (S[SP].invis)
	return;
    if (filled && *S[SP].fillcolor) {
	ps_set_color(S[SP].fillcolor);
	fprintf(Output_file, "%d %d %d %d ellipse_path\n", p.x, p.y, rx,
		ry);
	fprintf(Output_file, Fill);
	if (*S[SP].pencolor)
	    ps_set_color(S[SP].pencolor);
    }
    if (*S[SP].pencolor == '\0')
	return;
    if (!filled || (filled && strcmp(S[SP].fillcolor, S[SP].pencolor))) {
	fprintf(Output_file, "%d %d %d %d ellipse_path\n", p.x, p.y, rx,
		ry);
	fprintf(Output_file, Stroke);
    }
}

static void ps_polyline(point * A, int n)
{
    int j;

    if (S[SP].invis)
	return;
    if (*S[SP].pencolor == '\0')
	return;
    fprintf(Output_file, Newpath_Moveto, A[0].x, A[0].y);
    for (j = 1; j < n; j++)
	fprintf(Output_file, "%d %d lineto\n", A[j].x, A[j].y);
    fprintf(Output_file, Stroke);
}

static void ps_user_shape(char *name, point * A, int sides, int filled)
{
    int j;
    ps_image_t *img;
    point offset;
    char *shapeimagefile;
    char *suffix;

    if (S[SP].invis)
	return;
    if (streq(name, "custom")) {
	shapeimagefile = agget(Curnode, "shapefile");
	if ((img = ps_usershape(shapeimagefile))) {
	    ps_begin_context();
	    offset.x = -img->origin.x - (img->size.x) / 2;
	    offset.y = -img->origin.y - (img->size.y) / 2;
	    fprintf(Output_file, "%d %d translate newpath user_shape_%d\n",
		    ND_coord_i(Curnode).x + offset.x,
		    ND_coord_i(Curnode).y + offset.y, img->macro_id);
	    ps_end_context();
	} else {
	    suffix = strrchr(shapeimagefile, '.');
	    if (suffix) {
		suffix++;
		if (strcmp(suffix, "ps")) {
		    agerr(AGERR,
			  "image type \"%s\" not supported in PostScript output\n",
			  suffix);
		} else {
		    agerr(AGERR, "Could not find image file \"%s\"\n",
			  shapeimagefile);
		}
	    } else {
		agerr(AGERR,
		      "image file %s not supported in PostScript output\n",
		      shapeimagefile);
	    }
	}
    } else {
	fprintf(Output_file, "[ ");
	for (j = 0; j < sides; j++)
	    fprintf(Output_file, "%d %d ", A[j].x, A[j].y);
	fprintf(Output_file, "%d %d ", A[0].x, A[0].y);
	fprintf(Output_file, "]  %d %s %s\n", sides,
		(filled ? "true" : "false"), name);
    }
}

codegen_t PS_CodeGen = {
    ps_reset,
    ps_begin_job, ps_end_job,
    ps_begin_graph, ps_end_graph,
    ps_begin_page, ps_end_page,
    ps_begin_layer, 0,		/* ps_end_layer */
    ps_begin_cluster, 0,	/* ps_end_cluster */
    0, /* ps_begin_nodes */ 0,	/* ps_end_nodes */
    0, /* ps_begin_edges */ 0,	/* ps_end_edges */
    ps_begin_node, 0,		/* ps_end_node */
    ps_begin_edge, 0,		/* ps_end_edge */
    ps_begin_context, ps_end_context,
    0, /* ps_begin_anchor */ 0,	/* ps_end_anchor */
    ps_set_font, ps_textline,
    ps_set_pencolor, ps_set_fillcolor, ps_set_style,
    ps_ellipse, ps_polygon,
    ps_bezier, ps_polyline,
    0,				/* bezier_has_arrows */
    ps_comment,
    0,				/* ps_textsize */
    ps_user_shape,
    0				/* usershapesize */
};
