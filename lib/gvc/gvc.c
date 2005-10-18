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

#include "render.h"
#include "gvplugin.h"
#include "gvcint.h"
#include "gvcproc.h"

extern GVC_t *gvNEWcontext(char **info, char *user);
extern char *gvUsername(void);

char *LibInfo[] = {
    "libgvc",		/* Program */
    VERSION,		/* Version */
    BUILDDATE		/* Build Date */
};

GVC_t *gvContext(void)
{
    GVC_t *gvc;

    aginit();
    agnodeattr(NULL, "label", NODENAME_ESC);
    gvc = gvNEWcontext(LibInfo, gvUsername());
    gvconfig(gvc, FALSE); /* configure for available plugins and codegens */
    return gvc;
}

int gvLayout(GVC_t *gvc, graph_t *g, char *engine)
{
    char buf[256];
    Agsym_t *a;
    int rc;

    g = g->root;
    if (GD_drawing(g)) {        /* only cleanup once between layouts */
        gvFreeLayout(gvc, g);
        GD_drawing(g) = NULL;
    }
    rc = gvlayout_select(gvc, engine);
    if (rc == NO_SUPPORT) {
        agerr (AGERR, "Layout type: \"%s\" not recognized. Use one of:%s\n",
                engine, gvplugin_list(gvc, API_layout, engine));
        return -1;
    }

    gvLayoutJobs(gvc, g);

/* set bb attribute for basic layout.
 * doesn't yet include margins, scaling or page sizes because
 * those depend on the renderer being used. */
    if (GD_drawing(g)->landscape)
        sprintf(buf, "%d %d %d %d",
                ROUND(GD_bb(g).LL.y), ROUND(GD_bb(g).LL.x),
                ROUND(GD_bb(g).UR.y), ROUND(GD_bb(g).UR.x));
    else
        sprintf(buf, "%d %d %d %d",
                ROUND(GD_bb(g).LL.x), ROUND(GD_bb(g).LL.y),
                ROUND(GD_bb(g).UR.x), ROUND(GD_bb(g).UR.y));
    if (!(a = agfindattr(g, "bb"))) {
        a = agraphattr(g, "bb", "");
    }
    agxset(g, a->index, buf);

    return 0;
}

/* Render layout in a specified format to an open FILE */
int gvRender(GVC_t *gvc, graph_t *g, char *format, FILE *out)
{
    int rc;
    GVJ_t *job;

    g = g->root;

    /* create a job for the required format */
    rc = gvrender_output_langname_job(gvc, format);
    if (rc == NO_SUPPORT) {
        agerr (AGERR, "Renderer type: \"%s\" not recognized. Use one of:%s\n",
                format, gvplugin_list(gvc, API_render, format));
        return -1;
    }

    job = gvc->job;
    job->output_lang = gvrender_select(job, job->output_langname);
    if (!GD_drawing(g) && job->output_lang != CANONICAL_DOT) {
	fprintf(stderr, "Layout was not done\n");
	return -1;
    }
    job->output_file = out;
    gvRenderJobs(gvc, g);
    gvrender_delete_jobs(gvc);

    return 0;
}

/* Render layout in a specified format to an open FILE */
int gvRenderFilename(GVC_t *gvc, graph_t *g, char *format, char *filename)
{
    int rc;
    GVJ_t *job;

    g = g->root;

    /* create a job for the required format */
    rc = gvrender_output_langname_job(gvc, format);
    if (rc == NO_SUPPORT) {
	agerr(AGERR, "Renderer type: \"%s\" not recognized. Use one of:%s\n",                format, gvplugin_list(gvc, API_render, format));
	return -1;
    }

    job = gvc->job;
    job->output_lang = gvrender_select(job, job->output_langname);
    if (!GD_drawing(g) && job->output_lang != CANONICAL_DOT) {
	fprintf(stderr, "Layout was not done\n");
	return -1;
    }
    gvrender_output_filename_job(gvc, filename);
    gvRenderJobs(gvc, g);
    gvrender_delete_jobs(gvc);

    return 0;
}
