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

#include <dotneato.h>

char *Info[] = {
    "libdotneato",		/* Program */
    VERSION,			/* Version */
    BUILDDATE			/* Build Date */
};

GVC_t *gvContext(void)
{
    GVC_t *gvc;

    aginit();
    agnodeattr(NULL, "label", NODENAME_ESC);
    gvc = gvNEWcontext(Info, username());
    gvconfig(gvc); /* configure for available plugins and codegens */
    return gvc;
}

int gvLayout(GVC_t *gvc, graph_t *g, char *engine)
{
    char buf[256];
    Agsym_t *a;
    int rc;

    g = g->root;
    if (GD_drawing(g)) {        /* only cleanup once between layouts */
        gvlayout_cleanup(gvc, g);
        GD_drawing(g) = NULL;
    }
    rc = gvlayout_select(gvc, engine);
    if (rc == NO_SUPPORT) {
        fprintf(stderr, "Layout type: \"%s\" not recognized. Use one of:%s\n",
                engine, gvplugin_list(gvc, API_layout, engine));
        return -1;
    }

    gvlayout_layout(gvc, g);

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

int gvRender(GVC_t *gvc, graph_t *g, char *format, FILE *out)
{
    int rc;
    GVJ_t *job;

    g = g->root;

    /* create a job for the required format */
    rc = gvrender_output_langname_job(gvc, format);
    if (rc == NO_SUPPORT) {
        fprintf(stderr, "Renderer type: \"%s\" not recognized. Use one of:%s\n",
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

    emit_jobs(gvc, g);
    gvrender_delete_jobs(gvc);

    return 0;
}


void gvCleanup(GVC_t *gvc)
{
    gvFREEcontext(gvc);
}
