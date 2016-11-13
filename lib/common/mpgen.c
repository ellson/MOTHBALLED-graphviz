/* $Id$ $Revision$ */
/* vim:set shiftwidth=4 ts=8: */

/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: See CVS logs. Details at http://www.graphviz.org/
 *************************************************************************/

/* mpgen.c 1999-Feb-23 Jim Hefferon jim@joshua.smcvt.edu 
 *  Adapted from psgen.c.  See 1st_read.mp.
 */
#include "render.h"
#ifndef _WIN32
#include <unistd.h>
#endif
#include <sys/stat.h>


#define	NONE	0
#define	NODE	1
#define	EDGE	2
#define	CLST	3

/* static 	point	Pages; */
/* static	box		PB; */
static int onetime = TRUE;

static const char	**U_lib;

typedef struct grcontext_t {
    char *color, *font;
    double size;
} grcontext_t;

#define STACKSIZE 32		/* essentially infinite? */
static grcontext_t S[STACKSIZE];
static int SP = 0;

static void mp_reset(void)
{
    onetime = TRUE;
}

/* mp_cat_libfile:
 * Write library files onto the given file pointer.
 * arglib is an NULL-terminated array of char*
 * Each non-trivial entry should be the name of a file to be included.
 * stdlib is an NULL-terminated array of char*
 * Each of these is a line of a standard library to be included.
 * If any item in arglib is the empty string, the stdlib is not used.
 * The stdlib is printed first, if used, followed by the user libraries.
 * We check that for web-safe file usage.
 */
static void mp_cat_libfile(FILE * ofp, const char **arglib, const char **stdlib)
{
    FILE *fp;
    const char **s, *bp, *p, *path;
    int i;
    boolean use_stdlib = TRUE;

    /* check for empty string to turn off stdlib */
    if (arglib) {
	for (i = 0; use_stdlib && ((p = arglib[i])); i++) {
	    if (*p == '\0')
		use_stdlib = FALSE;
	}
    }
    if (use_stdlib)
	for (s = stdlib; *s; s++) {
	    fputs(*s, ofp);
	    fputc('\n', ofp);
	}
    if (arglib) {
	for (i = 0; (p = arglib[i]) != 0; i++) {
	    if (*p == '\0')
		continue;	/* ignore empty string */
	    path = safefile(p);	/* make sure filename is okay */
	    if (!path) {
		agerr(AGWARN, "can't find library file %s\n", p);
	    }
	    else if ((fp = fopen(path, "r"))) {
		while ((bp = Fgets(fp)))
		    fputs(bp, ofp);
		fputc('\n', ofp); /* append a newline just in case */
		fclose (fp);
	    } else
		agerr(AGWARN, "can't open library file %s\n", path);
	}
    }
}

static void
mp_begin_job(FILE * ofp, graph_t * g, const char **lib, char *info[], point pages)
{
    /* pages and libraries not here (yet?) */
    /* Pages = pages; */
    U_lib = lib;
    /* N_pages = pages.x * pages.y; */
    /* Cur_page = 0; */

    fprintf(Output_file, "%%--- graphviz MetaPost input\n");
    fprintf(Output_file, "%% Created by program: %s version %s (%s)\n",
	    info[0], info[1], info[2]);
    fprintf(Output_file, "%% Title: %s\n", agnameof(g));
    fprintf(Output_file,
	    "%%  Put this between beginfig and endfig.  See 1st_read.mp.\n");
    fprintf(Output_file, "%% \n");
}

static void mp_end_job(void)
{
    fprintf(Output_file, "%%  End of graphviz MetaPost input\n");
    fprintf(Output_file, "%%  \n");
}

static void mp_comment(char *str)
{
    fprintf(Output_file, "%% %s\n", str);
}

static void mp_begin_graph(GVC_t * gvc, graph_t * g, box bb, point pb)
{
    /* PB = bb; */
    static const char *mp_lib[] = {0};
    if (onetime) {
	fprintf(Output_file, "%% BoundingBox: %d %d %d %d\n",
		bb.LL.x, bb.LL.y, bb.UR.x + 1, bb.UR.y + 1);
	        mp_cat_libfile(Output_file,U_lib,mp_lib);
	onetime = FALSE;
    }
}

static void
mp_begin_page(graph_t * g, point page, double scale, int rot, point offset)
{
    assert(SP == 0);
    S[SP].font = "";
    S[SP].color = "black";
    S[SP].size = 0.0;
}

static void mp_begin_context(void)
{
    if (SP == STACKSIZE - 1)
	agerr(AGWARN, "mpgen stack overflow\n");
    else {
	SP++;
	S[SP] = S[SP - 1];
    }
}

static void mp_end_context(void)
{
    if (SP == 0)
	agerr(AGWARN, "mpgen stack underflow\n");
    else
	SP--;
}

static void mp_set_font(char *name, double size)
{
    if (strcmp(S[SP].font, name) || (size != S[SP].size)) {
	fprintf(Output_file, "%% GV set font: %.2f /%s ignored\n", size,
		name);
	S[SP].font = name;
	S[SP].size = size;
    }
}

static void mp_set_color(char *name)
{
    static char *op[] = { "graph", "node", "edge", "sethsb" };
    gvcolor_t color;

    if (strcmp(name, S[SP].color)) {
	colorxlate(name, &color, HSVA_DOUBLE);
	fprintf(Output_file, "%% GV set color: %.3f %.3f %.3f %scolor\n",
		color.u.HSVA[0], color.u.HSVA[1], color.u.HSVA[2], op[Obj]);
    }
    S[SP].color = name;
}

static void mp_set_style(char **s)
{
    char *line, *p;

    while ((p = line = *s++)) {
	while (*p)
	    p++;
	p++;
	while (*p) {
	    fprintf(Output_file, "%% GV set style: %s \n", p);
	    while (*p)
		p++;
	    p++;
	}
	fprintf(Output_file, "%% GV set style:: %s\n", line);
    }
}

static char *mp_string(char *s)
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
    while (*s) {
	if (pos > (bufsize - 8)) {
	    bufsize *= 2;
	    buf = grealloc(buf, bufsize);
	    p = buf + pos;
	}
	if ((*s == LPAREN) || (*s == RPAREN)) {
	    *p++ = '\\';
	    pos++;
	}
	*p++ = *s++;
	pos++;
    }
    *p = '\0';
    return buf;
}

static void mp_textpara(point p, textpara_t * para)
{
    fprintf(Output_file, "label(btex %s etex,(%dbp,%dbp)) withcolor %s;\n",
	    mp_string(para->str), p.x, p.y, S[SP].color);
}

static void
mp_bezier(point * A, int n, int arrow_at_start, int arrow_at_end, int filled)
{
    int j;
    if (arrow_at_start || arrow_at_end)
	agerr(AGERR, "mp_bezier illegal arrow args\n");
    fprintf(Output_file, "draw (%dbp,%dbp) ", A[0].x, A[0].y);
    for (j = 1; j < n; j += 3)
	fprintf(Output_file,
		"\n  ..controls (%dbp,%dbp) and (%dbp,%dbp).. (%dbp,%dbp)",
		A[j].x, A[j].y, A[j + 1].x, A[j + 1].y, A[j + 2].x,
		A[j + 2].y);
    fprintf(Output_file, " withcolor %s;\n", S[SP].color);
}

static void mp_polygon(point * A, int n, int filled)
{
    int j;
    if (filled) {
	fprintf(Output_file, "  fill (%dbp,%dbp)", A[0].x, A[0].y);
	for (j = 1; j < n; j++)
	    fprintf(Output_file, "\n  --(%dbp,%dbp)", A[j].x, A[j].y);
	fprintf(Output_file, "\n  --cycle withcolor %s;\n", S[SP].color);
    }
    fprintf(Output_file, "draw (%dbp,%dbp)  ", A[0].x, A[0].y);
    for (j = 1; j < n; j++)
	fprintf(Output_file, "\n  --(%dbp,%dbp)", A[j].x, A[j].y);
    fprintf(Output_file, "\n  --cycle withcolor %s;\n", S[SP].color);
}

static void mp_ellipse(point p, int rx, int ry, int filled)
{
    if (filled)
	fprintf(Output_file,
		"  fill fullcircle xscaled %dbp yscaled %dbp shifted (%dbp,%dbp) withcolor %s;\n",
		2 * rx, 2 * ry, p.x, p.y, S[SP].color);
    fprintf(Output_file,
	    "draw fullcircle xscaled %dbp yscaled %dbp shifted (%dbp,%dbp);\n",
	    2 * rx, 2 * ry, p.x, p.y);
}

static void mp_polyline(point * A, int n)
{
    int j;

    fprintf(Output_file, "draw (%dbp,%dbp) ", A[0].x, A[0].y);
    for (j = 1; j < n; j++)
	fprintf(Output_file, "\n  --(%dbp,%dbp)", A[j].x, A[j].y);
    fprintf(Output_file, " withcolor %s;\n", S[SP].color);
}

static void mp_usershape(usershape_t *us, boxf b, point *A, int n, boolean filled)
{
    int j;
    fprintf(Output_file, "%%GV USER SHAPE [ ");
    for (j = 0; j < n; j++)
	fprintf(Output_file, "%d %d ", A[j].x, A[j].y);
    fprintf(Output_file, "%d %d ", A[0].x, A[0].y);
    fprintf(Output_file, "]  %d %s %s ignored\n", n,
	    (filled ? "true" : "false"), us->name);
}

codegen_t MP_CodeGen = {
    mp_reset,
    mp_begin_job, mp_end_job,
    mp_begin_graph, 0,		/* mp_end_graph */
    mp_begin_page, 0,		/* mp_end_page */
    0, /* mp_begin_layer */ 0,	/* mp_end_layer */
    0, /* mp_begin_cluster */ 0,	/* mp_end_cluster */
    0, /* mp_begin_nodes */ 0,	/* mp_end_nodes */
    0, /* mp_begin_edges */ 0,	/* mp_end_edges */
    0, /* mp_begin_node */  0,	/* mp_end_node */
    0, /* mp_begin_edge */  0,	/* mp_end_edge */
    mp_begin_context, mp_end_context,
    0, /* mp_begin_anchor */ 0,	/* mp_end_anchor */
    mp_set_font, mp_textpara,
    mp_set_color, mp_set_color, mp_set_style,
    mp_ellipse, mp_polygon,
    mp_bezier, mp_polyline,
    0,				/* bezier_has_arrows */
    mp_comment,
    mp_usershape
};
