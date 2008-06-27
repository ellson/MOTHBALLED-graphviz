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

#include "builddate.h"
#include "types.h"
#include "graph.h"
#include "const.h"
#include "gvplugin.h"
#include "gvcjob.h"
#include "gvcint.h"
#include "gvcproc.h"

extern GVC_t *gvNEWcontext(char **info, char *user);
extern char *gvUsername(void);
extern int gvRenderJobs (GVC_t * gvc, graph_t * g);
extern char *gvplugin_list(GVC_t * gvc, api_t api, char *str);

static char *LibInfo[] = {
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

/* gvLayout:
 * Selects layout based on engine and binds it to gvc;
 * does the layout and sets the graph's bbox.
 * Return 0 on success.
 */
int gvLayout(GVC_t *gvc, graph_t *g, char *engine)
{
    char buf[256];
    int rc;

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
    agsafeset(g, "bb", buf, "");

    return 0;
}

/* Render layout in a specified format to an open FILE */
int gvRender(GVC_t *gvc, graph_t *g, char *format, FILE *out)
{
    int rc;
    GVJ_t *job;

    g = g->root;

    /* create a job for the required format */
    rc = gvjobs_output_langname(gvc, format);
    job = gvc->job;
    if (rc == NO_SUPPORT) {
        agerr (AGERR, "Format: \"%s\" not recognized. Use one of:%s\n",
                format, gvplugin_list(gvc, API_device, format));
        return -1;
    }

    job->output_lang = gvrender_select(job, job->output_langname);
    if (!GD_drawing(g) && !(job->flags & LAYOUT_NOT_REQUIRED)) {
	fprintf(stderr, "Layout was not done\n");
	return -1;
    }
    job->output_file = out;
    if (out == NULL)
	job->flags |= OUTPUT_NOT_REQUIRED;
    gvRenderJobs(gvc, g);
    gvrender_end_job(job);
    gvdevice_finalize(job);
    fflush(job->output_file);
    gvjobs_delete(gvc);

    return 0;
}

/* Render layout in a specified format to an open FILE */
int gvRenderFilename(GVC_t *gvc, graph_t *g, char *format, char *filename)
{
    int rc;
    GVJ_t *job;

    g = g->root;

    /* create a job for the required format */
    rc = gvjobs_output_langname(gvc, format);
    job = gvc->job;
    if (rc == NO_SUPPORT) {
	agerr(AGERR, "Format: \"%s\" not recognized. Use one of:%s\n",
                format, gvplugin_list(gvc, API_device, format));
	return -1;
    }

    job->output_lang = gvrender_select(job, job->output_langname);
    if (!GD_drawing(g) && !(job->flags & LAYOUT_NOT_REQUIRED)) {
	fprintf(stderr, "Layout was not done\n");
	return -1;
    }
    gvjobs_output_filename(gvc, filename);
    gvRenderJobs(gvc, g);
    gvrender_end_job(job);
    gvdevice_finalize(job);
    fflush(job->output_file);
    gvjobs_delete(gvc);

    return 0;
}

/* Render layout in a specified format to a malloc'ed string */
int gvRenderData(GVC_t *gvc, graph_t *g, char *format, char **result, unsigned int *length)
{
    int rc;
    GVJ_t *job;

    g = g->root;

    /* create a job for the required format */
    rc = gvjobs_output_langname(gvc, format);
    job = gvc->job;
    if (rc == NO_SUPPORT) {
	agerr(AGERR, "Format: \"%s\" not recognized. Use one of:%s\n",
                format, gvplugin_list(gvc, API_device, format));
	return -1;
    }

    job->output_lang = gvrender_select(job, job->output_langname);
    if (!GD_drawing(g) && !(job->flags & LAYOUT_NOT_REQUIRED)) {
	fprintf(stderr, "Layout was not done\n");
	return -1;
    }

/* page size on Linux, Mac OS X and Windows */
#define OUTPUT_DATA_INITIAL_ALLOCATION 4096

    if(!result || !(*result = malloc(OUTPUT_DATA_INITIAL_ALLOCATION))) {
	agerr(AGERR, "failure malloc'ing for result string");
	return -1;
    }

    job->output_data = *result;
    job->output_data_allocated = OUTPUT_DATA_INITIAL_ALLOCATION;
    job->output_data_position = 0;

    gvRenderJobs(gvc, g);
    gvrender_end_job(job);
    gvdevice_finalize(job);

    *result = job->output_data;
    *length = job->output_data_position;
    gvjobs_delete(gvc);

    return 0;
}

char **gvcInfo(GVC_t* gvc) { return gvc->common.info; }
char *gvcUsername(GVC_t* gvc) { return gvc->common.user; }
char *gvcVersion(GVC_t* gvc) { return gvc->common.info[1]; }
char *gvcBuildDate(GVC_t* gvc) { return gvc->common.info[2]; }
