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

/* macros for inverting the y coordinate with the bounding box */
#define Y(y) (y_invert ? (y_off - (y)) : (y))

static agxbuf charbuf;
static agxbuf outbuf;
static Agraph_t *cluster_g;

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

static void xd_points(char c, point * A, int n)
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
    xd_points((filled ? 'P' : 'p'), A, n);
}

static void
xd_bezier(point * A, int n, int arrow_at_start, int arrow_at_end)
{
    xd_points('B', A, n);
}

static void xd_polyline(point * A, int n)
{
    xd_points('L', A, n);
}

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
