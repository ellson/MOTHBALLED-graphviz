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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <assert.h>

#include "inkpot.h"

char *Info[] = {
    "inkpot",                   /* Program */
    "1","2008"
#if 0
    VERSION,                    /* Version */
    BUILDDATE                   /* Build Date */
#endif
};

static const char *usage =
    ": inkpot [<color>]\n\
    -f <schemes> - input color scheme, or ',' separated list of schemes\n\
    -t <scheme>  - output solor scheme\n\
    -V           - print version info\n\
    -?           - print usage info\n\
    If no color is specified, a stdin is processed for color tokens.\n";

static void scanArgs(inkpot_t *inkpot, int argc, char **argv)
{
    int c;

    while ((c = getopt(argc, argv, ":?Vf:t:o:")) != -1) {
	switch (c) {
	case 'f':
	    inkpot_schemes_put(inkpot, optarg);
	    break;
	case 't':
	    inkpot_scheme_get(inkpot, optarg);
	    break;
	case 'V':
	    fprintf(stderr, "%s version %s (%s)\n",
		    Info[0], Info[1], Info[2]);
	    exit(0);
	    break;
	case '?':
	    fprintf(stderr,"%s\n", usage);
	    exit(0);
	    break;
	case ':':
	    fprintf (stderr, "missing argument for option -%c\n", optopt);
	    break;
	default:
	    fprintf(stderr,"option -%c unrecognized\n", optopt);
	}
    }
}


int main (int argc, char *argv[])
{
    inkpot_t *inkpot;
    inkpot_status_t rc;
    char *color;
    double rgba[4], hsva[4], cmyk[4];

    inkpot = inkpot_init();
    if (!inkpot) {
	fprintf(stderr, "failure to initialize inkpot\n");
    	return 0;
    }
 
    inkpot_schemes_put(inkpot, "x11");
    inkpot_scheme_get(inkpot, "x11");

    scanArgs(inkpot, argc, argv);

    /* requested color */
    if (argc < 2)
        color = NULL;
    else
        color = argv[1];

    inkpot_debug_schemes(inkpot);
    
#if 0
    inkpot_debug_names(inkpot);

    inkpot_debug_out_names(inkpot);

    inkpot_debug_values(inkpot);
#endif

    fprintf(stdout, "%s ", color); /* ' ' after %s so it doesn't crash on NULL */
    rc = inkpot_put(inkpot, color);
    if (rc == INKPOT_COLOR_UNKNOWN)
        fprintf(stdout, "(unknown)");
    fprintf(stdout, "\n text: ");

    rc = inkpot_write(inkpot);
    if (rc == INKPOT_COLOR_NONAME || rc == INKPOT_NOPALETTE) {
        fprintf(stdout, "#");
        rc = inkpot_write_rgba8(inkpot);
    }
    assert (rc == INKPOT_SUCCESS);

    rc = inkpot_get_rgba(inkpot, rgba);
    assert (rc == INKPOT_SUCCESS);
    fprintf(stdout, "\n rgba: %.3f,%.3f,%.3f,%.3f\n",
		rgba[0], rgba[1], rgba[2], rgba[3]);

    rc = inkpot_get_hsva(inkpot, hsva);
    assert (rc == INKPOT_SUCCESS);
    fprintf(stdout, " hsva: %.3f,%.3f,%.3f,%.3f\n",
		hsva[0], hsva[1], hsva[2], hsva[3]);

    rc = inkpot_get_cmyk(inkpot, cmyk);
    assert (rc == INKPOT_SUCCESS);
    fprintf(stdout, " cmyk: %.3f,%.3f,%.3f,%.3f\n",
		cmyk[0], cmyk[1], cmyk[2], cmyk[3]);

    inkpot_destroy(inkpot);
    return 0;
}
