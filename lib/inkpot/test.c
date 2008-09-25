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
            fprintf (stderr, "color scheme \"%s\" was not found\n", argv[i]);
        else
            assert(rc == INKPOT_SUCCESS);
    }

    if (argc < 4) {
        rc = inkpot_activate(inkpot, "x11");
        assert(rc == INKPOT_SUCCESS);
    }
    else {
        for (i = 3; i < argc; i++) {
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


    rc = inkpot_set(inkpot, color);
    if (rc == INKPOT_SUCCESS || rc == INKPOT_COLOR_NONAME) {
	inkpot_get_rgba(inkpot, rgba);
	rc = inkpot_get(inkpot, &tocolor);
	if (rc == INKPOT_SUCCESS) 
            fprintf(stderr, "%s", tocolor);
	else if (rc == INKPOT_COLOR_NONAME) 
            fprintf(stderr, "#%02x%02x%02x%02x",
			    rgba[0], rgba[1], rgba[2], rgba[3]);
	else
	    assert(0);
        fprintf(stderr, " %d,%d,%d,%d\n",
			rgba[0], rgba[1], rgba[2], rgba[3]);

    }
    else if (rc == INKPOT_COLOR_UNKNOWN) {
	rc = inkpot_set_default(inkpot);
	assert (rc == INKPOT_SUCCESS);
	inkpot_get_rgba(inkpot, rgba);
        fprintf(stderr, "%s (unknown) %d,%d,%d,%d (default)\n",
		color, rgba[0], rgba[1], rgba[2], rgba[3]);
    }
    else
        assert(0);

    return 0;
}
