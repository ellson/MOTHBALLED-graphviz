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

#include "gvplugin_render.h"

/* for g->name */
#include "graph.h"

extern char *xml_string(char *str);

typedef enum { FORMAT_IMAP, FORMAT_ISMAP, FORMAT_CMAP, FORMAT_CMAPX, } format_type;

static void map_output_shape (GVJ_t *job, map_shape_t map_shape, pointf * AF, int nump,
                char* url, char *tooltip, char *target)
{
    int i;

    static point *A;
    static int size_A;

    if (!AF || !nump)
	return;

    if (size_A < nump) {
	size_A = nump + 10;
	A = realloc(A, size_A * sizeof(point));
    }
    for (i = 0; i < nump; i++)
	PF2P(AF[i], A[i]);

    if (job->render.id == FORMAT_IMAP && url && url[0]) {
        switch (map_shape) {
        case MAP_RECTANGLE:
	    /* Y_GOES_DOWN so need UL to LR */
            gvdevice_printf(job, "rect %s %d,%d %d,%d\n", url,
                A[0].x, A[1].y, A[1].x, A[0].y);
            break;
        case MAP_CIRCLE:
            gvdevice_printf(job, "circle %s %d,%d,%d\n", url,
                A[0].x, A[0].y, A[1].x-A[0].x);
            break;
        case MAP_POLYGON:
            gvdevice_printf(job, "poly %s", url);
            for (i = 0; i < nump; i++)
                gvdevice_printf(job, " %d,%d", A[i].x, A[i].y);
            gvdevice_fputs(job, "\n");
            break;
        default:
            assert(0);
            break;
        }

    } else if (job->render.id == FORMAT_ISMAP && url && url[0]) {
        switch (map_shape) {
        case MAP_RECTANGLE:
	    /* Y_GOES_DOWN so need UL to LR */
            gvdevice_printf(job, "rectangle (%d,%d) (%d,%d) %s %s\n",
                A[0].x, A[1].y, A[1].x, A[0].y, url, tooltip);
	    break;
        default:
            assert(0);
            break;
        }

    } else if (job->render.id == FORMAT_CMAP || job->render.id == FORMAT_CMAPX) {
        switch (map_shape) {
        case MAP_CIRCLE:
            gvdevice_fputs(job, "<area shape=\"circle\"");
            break;
        case MAP_RECTANGLE:
            gvdevice_fputs(job, "<area shape=\"rect\"");
            break;
        case MAP_POLYGON:
            gvdevice_fputs(job, "<area shape=\"poly\"");
            break;
        default:
            assert(0);
            break;
        }
        if (url && url[0]) {
            gvdevice_fputs(job, " href=\"");
	    gvdevice_fputs(job, xml_string(url));
	    gvdevice_fputs(job, "\"");
	}
        if (target && target[0]) {
            gvdevice_fputs(job, " target=\"");
	    gvdevice_fputs(job, xml_string(target));
	    gvdevice_fputs(job, "\"");
	}
        if (tooltip && tooltip[0]) {
            gvdevice_fputs(job, " title=\"");
	    gvdevice_fputs(job, xml_string(tooltip));
	    gvdevice_fputs(job, "\"");
	}
        /*
	 * alt text is intended for the visually impaired, but such
	 * folk are not likely to be clicking around on a graph anyway.
	 * IE on the PC platform (but not on Macs) incorrectly
	 * uses (non-empty) alt strings instead of title strings for tooltips.
	 * To make tooltips work and avoid this IE issue,
	 * while still satisfying usability guidelines
	 * that require that there is always an alt string,
	 * we generate just an empty alt string.
	 */
        gvdevice_fputs(job, " alt=\"\"");

        gvdevice_fputs(job, " coords=\"");
        switch (map_shape) {
        case MAP_CIRCLE:
            gvdevice_printf(job, "%d,%d,%d", A[0].x, A[0].y, A[1].x-A[0].x);
            break;
        case MAP_RECTANGLE:
	    /* Y_GOES_DOWN so need UL to LR */
            gvdevice_printf(job, "%d,%d,%d,%d", A[0].x, A[1].y, A[1].x, A[0].y);  
            break;
        case MAP_POLYGON:
            gvdevice_printf(job, "%d,%d", A[0].x, A[0].y);
            for (i = 1; i < nump; i++)
                gvdevice_printf(job, " %d,%d", A[i].x, A[i].y);
            break;
        default:
            break;
        }
        if (job->render.id == FORMAT_CMAPX)
            gvdevice_fputs(job, "\"/>\n");
	else
            gvdevice_fputs(job, "\">\n");
    }
}

static void map_begin_page(GVJ_t * job)
{
    obj_state_t *obj = job->obj;
    char *s;

    switch (job->render.id) {
    case FORMAT_IMAP:
        gvdevice_fputs(job, "base referer\n");
        if (obj->url && obj->url[0]) {
	    gvdevice_fputs(job, "default ");
	    gvdevice_fputs(job, xml_string(obj->url));
	    gvdevice_fputs(job, "\n");
	}
        break;
    case FORMAT_ISMAP:
        if (obj->url && obj->url[0]) {
	    gvdevice_fputs(job, "default ");
	    gvdevice_fputs(job, xml_string(obj->url));
	    gvdevice_fputs(job, " ");
	    gvdevice_fputs(job, xml_string(obj->u.g->name));
	    gvdevice_fputs(job, "\n");
	}
        break;
    case FORMAT_CMAPX:
	s = xml_string(obj->u.g->name);
	gvdevice_fputs(job, "<map id=\"");
	gvdevice_fputs(job, s);
	gvdevice_fputs(job, "\" name=\"");
	gvdevice_fputs(job, s);
	gvdevice_fputs(job, "\">\n");
        break;
    default:
	break;
    }
}

static void map_end_page(GVJ_t * job)
{
    obj_state_t *obj = job->obj;

    switch (job->render.id) {
    case FORMAT_CMAP:
	map_output_shape(job, obj->url_map_shape, obj->url_map_p,obj->url_map_n,
		                    obj->url, obj->tooltip, obj->target);
	break;
    case FORMAT_CMAPX:
	map_output_shape(job, obj->url_map_shape, obj->url_map_p,obj->url_map_n,
		                    obj->url, obj->tooltip, obj->target);
        gvdevice_fputs(job, "</map>\n");
	break;
    default:
	break;
    }
}

#if 0
static void map_begin_cluster(GVJ_t * job)
{
    obj_state_t *obj = job->obj;

    gvdevice_printf(job, "%% %s\n", obj->u.sg->name);

    map_output_shape(job, obj->url_map_shape, obj->url_map_p, obj->url_map_n,
	        obj->url, obj->tooltip, obj->target);
}

static void map_begin_node(GVJ_t * job)
{
    obj_state_t *obj = job->obj;

    map_output_shape(job, obj->url_map_shape, obj->url_map_p,obj->url_map_n,
	        obj->url, obj->tooltip, obj->target);
}

static void
map_begin_edge(GVJ_t * job)
{
    obj_state_t *obj = job->obj;
    int i, j = 0;

    map_output_shape(job, obj->url_map_shape, obj->url_map_p, obj->url_map_n,
	        obj->url, obj->tooltip, obj->target);
    map_output_shape(job, MAP_RECTANGLE, obj->tailurl_map_p, 2,
	        obj->tailurl, obj->tailtooltip, obj->tailtarget);
    map_output_shape(job, MAP_RECTANGLE, obj->headurl_map_p, 2,
	        obj->headurl, obj->headtooltip, obj->headtarget);
    map_output_shape(job, MAP_RECTANGLE, obj->tailendurl_map_p,2, 
	        obj->url, obj->tooltip, obj->target);
    map_output_shape(job, MAP_RECTANGLE, obj->headendurl_map_p, 2,
	        obj->url, obj->tooltip, obj->target);
    for (i = 0; i < obj->url_bsplinemap_poly_n; i++) {
        map_output_shape(job, MAP_POLYGON,
		obj->url_bsplinemap_p+j, obj->url_bsplinemap_n[i],
		obj->url, obj->tooltip, obj->target);
	j += obj->url_bsplinemap_n[i];
    }
}
#endif

static void map_begin_anchor(GVJ_t * job, char *url, char *tooltip, char *target)
{
    obj_state_t *obj = job->obj;

    map_output_shape(job, obj->url_map_shape, obj->url_map_p, obj->url_map_n, 
		url, tooltip, target);
}

static gvrender_engine_t map_engine = {
    0,				/* map_begin_job */
    0,				/* map_end_job */
    0,				/* map_begin_graph */
    0,				/* map_end_graph */
    0,				/* map_begin_layer */
    0,				/* map_end_layer */
    map_begin_page,
    map_end_page,
    0,				/* map_begin_cluster */
    0,				/* map_end_cluster */
    0,				/* map_begin_nodes */
    0,				/* map_end_nodes */
    0,				/* map_begin_edges */
    0,				/* map_end_edges */
    0,				/* map_begin_node */
    0,				/* map_end_node */
    0,				/* map_begin_edge */
    0,				/* map_end_edge */
    map_begin_anchor,
    0,				/* map_end_anchor */
    0,				/* map_textpara */
    0,				/* map_resolve_color */
    0,				/* map_ellipse */
    0,				/* map_polygon */
    0,				/* map_bezier */
    0,				/* map_polyline */
    0,				/* map_comment */
    0,				/* map_library_shape */
};

static gvrender_features_t render_features_map = {
    EMIT_CLUSTERS_LAST
        | GVRENDER_Y_GOES_DOWN
	| GVRENDER_DOES_MAPS
	| GVRENDER_DOES_LABELS
	| GVRENDER_DOES_TOOLTIPS
	| GVRENDER_DOES_TARGETS
	| GVRENDER_DOES_MAP_RECTANGLE, /* flags */
    4.,                         /* default pad - graph units */
    NULL,			/* knowncolors */
    0,				/* sizeof knowncolors */
    0,				/* color_type */
};

static gvdevice_features_t device_features_map = {
    GVRENDER_DOES_MAP_CIRCLE
	| GVRENDER_DOES_MAP_POLYGON, /* flags */
    {0.,0.},			/* default margin - points */
    {0.,0.},                    /* default page width, height - points */
    {96.,96.},			/* default dpi */
};

static gvdevice_features_t device_features_map_nopoly = {
    0,				/* flags */
    {0.,0.},			/* default margin - points */
    {0.,0.},                    /* default page width, height - points */
    {96.,96.},			/* default dpi */
};

gvplugin_installed_t gvrender_map_types[] = {
    {FORMAT_ISMAP, "map", 1, &map_engine, &render_features_map},
    {0, NULL, 0, NULL, NULL}
};

gvplugin_installed_t gvdevice_map_types[] = {
    {FORMAT_ISMAP, "ismap:map", 1, NULL, &device_features_map_nopoly},
    {FORMAT_CMAP, "cmap:map", 1, NULL, &device_features_map},
    {FORMAT_IMAP, "imap:map", 1, NULL, &device_features_map},
    {FORMAT_CMAPX, "cmapx:map", 1, NULL, &device_features_map},
    {FORMAT_IMAP, "imap_np:map", 1, NULL, &device_features_map_nopoly},
    {FORMAT_CMAPX, "cmapx_np:map", 1, NULL, &device_features_map_nopoly},
    {0, NULL, 0, NULL, NULL}
};
