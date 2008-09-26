#include <stdio.h>
#include <assert.h>

#include "inkpot.h"

int main (int argc, char *argv[])
{
    inkpot_t *inkpot;
    inkpot_status_t rc;
    char *color;
    int i;
    unsigned char rgba[4];

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
	    inkpot_error(inkpot);
        else
            assert(rc == INKPOT_SUCCESS);
    }

    if (argc < 4) {
        rc = inkpot_schemes(inkpot, "x11", NULL);
        assert(rc == INKPOT_SUCCESS);
    }
    else {
        for (i = 3; i < argc; i++) {
            rc = inkpot_schemes(inkpot, argv[i]);
            if (rc == INKPOT_SCHEME_UNKNOWN)
	        inkpot_error(inkpot);
            else
                assert(rc == INKPOT_SUCCESS);
        }
    }

/* ------------- */

    inkpot_debug_schemes(inkpot);
    
    inkpot_debug_names(inkpot);

    inkpot_debug_names_out(inkpot);

    inkpot_debug_values(inkpot);

/* ------------- */

    fprintf(stdout, "%s ", color);
    rc = inkpot_set(inkpot, color);
    if (rc == INKPOT_COLOR_UNKNOWN) {
        fprintf(stdout, "(unknown) ");
	rc = inkpot_set_default(inkpot);
	assert (rc == INKPOT_SUCCESS);
    }

    rc = inkpot_write(inkpot);
    assert (rc == INKPOT_SUCCESS || rc == INKPOT_COLOR_NONAME);

    rc = inkpot_get_rgba(inkpot, rgba);
    assert (rc == INKPOT_SUCCESS);
    fprintf(stdout, " %d,%d,%d,%d\n",
		rgba[0], rgba[1], rgba[2], rgba[3]);

    return 0;
}
