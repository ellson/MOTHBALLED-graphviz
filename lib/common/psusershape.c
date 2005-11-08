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

#ifndef MSWIN32
#include <unistd.h>
#endif

#include <sys/stat.h>
#include "render.h"

static int N_EPSF_files;
static Dict_t *EPSF_contents;

static void ps_image_free(Dict_t * dict, ps_image_t * p, Dtdisc_t * disc)
{
    free(p->contents);
}

static Dtdisc_t ImageDictDisc = {
    offsetof(ps_image_t, name),	/* key */
    -1,				/* size */
    0,				/* link offset */
    NIL(Dtmake_f),
    (Dtfree_f) ps_image_free,
    NIL(Dtcompar_f),
    NIL(Dthash_f),
    NIL(Dtmemory_f),
    NIL(Dtevent_f)
};

static ps_image_t *user_init(char *str)
{
    char *contents;
    char line[BUFSIZ];
    FILE *fp;
    struct stat statbuf;
    int saw_bb, must_inline;
    int lx, ly, ux, uy;
    ps_image_t *val;
    size_t cnt;

    if (!EPSF_contents)
	EPSF_contents = dtopen(&ImageDictDisc, Dtoset);

    val = dtmatch(EPSF_contents, str);
    if (val)
	return val;

    if (!(fp = fopen(str, "r"))) {
	agerr(AGWARN, "couldn't open epsf file %s\n", str);
	return NULL;
    }
    /* try to find size */
    saw_bb = must_inline = FALSE;
    while (fgets(line, sizeof(line), fp)) {
	if (sscanf
	    (line, "%%%%BoundingBox: %d %d %d %d", &lx, &ly, &ux,
	     &uy) == 4) {
	    saw_bb = TRUE;
	}
	if ((line[0] != '%') && strstr(line,"read")) must_inline = TRUE;
	if (saw_bb && must_inline) break;
    }

    if (saw_bb) {
	val = GNEW(ps_image_t);
	val->origin.x = lx;
	val->origin.y = ly;
	val->size.x = ux - lx;
	val->size.y = uy - ly;
	val->name = str;
	val->macro_id = N_EPSF_files++;
	fstat(fileno(fp), &statbuf);
	contents = val->contents = N_GNEW(statbuf.st_size + 1, char);
	fseek(fp, 0, SEEK_SET);
	cnt = fread(contents, statbuf.st_size, 1, fp);
	contents[statbuf.st_size] = '\0';
	fclose(fp);
	dtinsert(EPSF_contents, val);
	val->must_inline = must_inline;
	return val;
    } else {
	agerr(AGWARN, "BoundingBox not found in epsf file %s\n", str);
	return NULL;
    }
}

void epsf_init(node_t * n)
{
    epsf_t *desc;
    char *str;
    ps_image_t *img;
    int dx, dy;

    if ((str = safefile(agget(n, "shapefile")))) {
	img = user_init(str);
	if (!img)
	    return;
	dx = img->size.x;
	dy = img->size.y;
	ND_width(n) = PS2INCH(dx);
	ND_height(n) = PS2INCH(dy);
	ND_shape_info(n) = desc = NEW(epsf_t);
	desc->macro_id = img->macro_id;
	desc->offset.x = -img->origin.x - (dx) / 2;
	desc->offset.y = -img->origin.y - (dy) / 2;
    } else
	agerr(AGWARN, "shapefile not set for epsf node %s\n", n->name);
}

void epsf_free(node_t * n)
{

    if (ND_shape_info(n))
	free(ND_shape_info(n));
}

#define FILTER_EPSF 1
#ifdef FILTER_EPSF
/* this removes EPSF DSC comments that, when nested in another
 * document, cause errors in Ghostview and other Postscript
 * processors (although legal according to the Adobe EPSF spec).                 */
void epsf_emit_body(ps_image_t *img, FILE *of)
{
	char *p;
	p = img->contents;
	while (*p) {
		/* skip %%EOF lines */
		if ((p[0] == '%') && (p[1] == '%')
			&& (!strncasecmp(&p[2], "EOF", 3)
			|| !strncasecmp(&p[2], "BEGIN", 5)
			|| !strncasecmp(&p[2], "END", 3)
			|| !strncasecmp(&p[2], "TRAILER", 7)
		)) {
			/* check for *p since last line might not end in '\n' */
			while (*p && (*p++ != '\n'));
			continue;
		}
		do {
			fputc(*p, of);
		} while (*p++ != '\n');
	}
}
#else
void epsf_emit_body(ps_image_t *img, FILE *of)
{
	if (fputs(img->contents, of) == EOF) {
	    perror("epsf_define()->fputs");
	    exit(EXIT_FAILURE);
	}
}
#endif

void epsf_define(FILE * of)
{
    ps_image_t *img;

    if (!EPSF_contents)
	return;
    for (img = dtfirst(EPSF_contents); img;
	 img = dtnext(EPSF_contents, img)) {
	 if (img->must_inline) continue;
	fprintf(of, "/user_shape_%d {\n", img->macro_id);

	if (fputs("%%BeginDocument:\n", of) == EOF) {
	    perror("epsf_define()->fputs");
	    exit(EXIT_FAILURE);
	}
	epsf_emit_body(img,of);

	if (fputs("%%EndDocument\n", of) == EOF) {
	    perror("epsf_define()->fputs");
	    exit(EXIT_FAILURE);
	}

	if (fputs("} bind def\n", of) == EOF) {
	    perror("epsf_define()->fputs");
	    exit(EXIT_FAILURE);
	}
#if 0
	fprintf(of, "} bind def\n");
#endif
    }
}

point ps_image_size(graph_t * g, char *shapeimagefile)
{
    point rv;
    ps_image_t *img;

    rv.x = rv.y = -1;		/* assume error */
    if (shapeimagefile && *shapeimagefile) {
	img = user_init(shapeimagefile);
	if (img) {
	    rv.x = img->size.x;
	    rv.y = img->size.y;
	}
    } else
	rv.x = rv.y = 0;
    return rv;
}

void ps_freeusershapes(void)
{
    if (EPSF_contents) {
	dtclose(EPSF_contents);
	EPSF_contents = 0;
	N_EPSF_files = 0;
    }
}

ps_image_t *ps_usershape(char *shapeimagefile)
{
    if (EPSF_contents) {
	return dtmatch(EPSF_contents, shapeimagefile);
    }
    return NULL;
}
