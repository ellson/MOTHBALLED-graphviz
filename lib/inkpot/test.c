/* $Id$ $Revision$ */
/* vim:set shiftwidth=4 ts=8: */

/***********************************************************
 *      This software is part of the graphviz package      *
 *                http://www.graphviz.org/                 *
 *                                                         *
 *            Copyright (c) 1994-2008 AT&T Corp.           *
 *                and is licensed under the                *
 *            Common Public License, Version 1.0           *
 *                      by AT&T Corp.                      *
 *                                                         *
 *        Information and Software Systems Research        *
 *              AT&T Research, Florham Park NJ             *
 **********************************************************/

#include <stdio.h>
#include <assert.h>

#include "inkpot.h"

int main (int argc, char *argv[])
{
    inkpot_t *inkpot;
    inkpot_status_t rc;
    char *color;
    int i;
    double rgba[4], hsva[4], cmyk[4];

    inkpot = inkpot_init();
    if (!inkpot) {
	fprintf(stderr, "failure to initialize inkpot\n");
    	return 0;
    }

    if (argc < 2)
        color = NULL;
    else
        color = argv[1];

    if (argc < 3) {
        rc = inkpot_translate(inkpot, "x11");
        assert(rc == INKPOT_SUCCESS);
    }
    else {
        rc = inkpot_translate(inkpot, argv[2]);
        if (rc == INKPOT_SCHEME_UNKNOWN)
	    inkpot_debug_error(inkpot);
        else
            assert(rc == INKPOT_SUCCESS);
    }

    if (argc < 4) {
        rc = inkpot_schemes(inkpot, "x11");
        assert(rc == INKPOT_SUCCESS);
    }
    else {
        for (i = 3; i < argc; i++) {
            rc = inkpot_schemes(inkpot, argv[i]);
            if (rc == INKPOT_SCHEME_UNKNOWN)
	        inkpot_debug_error(inkpot);
            else
                assert(rc == INKPOT_SUCCESS);
        }
    }

#if 0
    inkpot_debug_schemes(inkpot);
    
    inkpot_debug_names(inkpot);

    inkpot_debug_out_names(inkpot);

#endif
    inkpot_debug_values(inkpot);

    fprintf(stdout, "%s ", color); /* ' ' after %s so it doesn't crash on NULL */
    rc = inkpot_set(inkpot, color);
    if (rc == INKPOT_COLOR_UNKNOWN)
        fprintf(stdout, "(unknown)");
    fprintf(stdout, "\n text: ");

    rc = inkpot_write(inkpot);
    assert (rc == INKPOT_SUCCESS
	    || rc == INKPOT_COLOR_NONAME
	    || rc == INKPOT_COLOR_NOPALETTE);

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
