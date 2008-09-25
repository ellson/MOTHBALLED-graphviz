#include <stdio.h>
#include <assert.h>

#include "inkpot.h"

int main (int argc, char *argv[])
{
    inkpot_t *inkpot;
    inkpot_status_t rc;
    char *color;
    const char *tocolor;
    int i;
    unsigned char rgba[4];

    rc = inkpot_init(&inkpot);
    if (rc == INKPOT_MALLOC_FAIL) {
        fprintf (stderr, "malloc fail\n");
    	return rc;
    }

    if (argc < 3) {
        rc = inkpot_activate(inkpot, "x11");
        assert(rc == INKPOT_SUCCESS);
    }
    else {
        for (i = 2; i < argc; i++) {
            rc = inkpot_activate(inkpot, argv[i]);
            if (rc == INKPOT_SCHEME_UNKNOWN)
                fprintf (stderr, "color scheme \"%s\" was not found\n", argv[i]);
            else
                assert(rc == INKPOT_SUCCESS);
        }
    }

    inkpot_print_schemes(inkpot, stderr);
    
    inkpot_print_names(inkpot, stderr);

    inkpot_print_values(inkpot, stderr);

    if (argc < 2)
        color = NULL;
    else
        color = argv[1];

    rc = inkpot_set(inkpot, color);
    if (rc == INKPOT_SUCCESS || rc == INKPOT_COLOR_UNNAMED) {
	inkpot_get_rgba(inkpot, rgba);
	inkpot_get(inkpot, "x11", &tocolor);
        fprintf(stderr, "%s %d,%d,%d,%d\n",
		tocolor, rgba[0], rgba[1], rgba[2], rgba[3]);
    }
    else {
	rc = inkpot_set_default(inkpot);
	assert (rc == INKPOT_SUCCESS);
	inkpot_get_rgba(inkpot, rgba);
        fprintf(stderr, "%s (not found) %d,%d,%d,%d (default)\n",
		color, rgba[0], rgba[1], rgba[2], rgba[3]);
    }

    return 0;
}
