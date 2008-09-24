#include <stdio.h>
#include <assert.h>

#include "inkpot.h"

int main (int argc, char *argv[])
{
    inkpot_t *inkpot;
    inkpot_status_t rc;
    char *color;
    int i;
    unsigned int r, g, b, a;

    rc = inkpot_init(&inkpot);
    if (rc == INKPOT_MALLOC_FAIL) {
        fprintf (stderr, "malloc fail\n");
    	return rc;
    }

    if (argc < 3) {
        rc = inkpot_add(inkpot, "x11");
        assert(rc == INKPOT_SUCCESS);
    }
    else {
        for (i = 2; i < argc; i++) {
            rc = inkpot_add(inkpot, argv[i]);
            if (rc == INKPOT_SCHEME_UNKNOWN) {
                fprintf (stderr, "color scheme \"%s\" was not found\n", argv[i]);
            }
            else if (rc == INKPOT_MAX_ONE_INDEXED_SCHEME) {
                fprintf (stderr, "only one indexed color scheme can be used, \"%s\" was ignored\n", argv[i]);
            }
            else {
                assert(rc == INKPOT_SUCCESS);
            }
        }
    }

    inkpot_print_schemes(inkpot);
    
    inkpot_print_names(inkpot);

    inkpot_print_values(inkpot);

    if (argc < 2)
        color = NULL;
    else
        color = argv[1];

    rc = inkpot_find(inkpot, color);
    if (rc == INKPOT_SUCCESS) {
	inkpot_get_rgba(inkpot, &r, &g, &b, &a);
        fprintf(stderr, "%s %d,%d,%d,%d\n", color, r, g, b, a);
    }
    else {
	rc = inkpot_find_default(inkpot);
	assert (rc == INKPOT_SUCCESS);
	inkpot_get_rgba(inkpot, &r, &g, &b, &a);
        fprintf(stderr, "%s (not found) %d,%d,%d,%d (default)\n", color, r, g, b, a);
    }

    return 0;
}
