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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "inkpot.h"
#include "inkpot_tables.h"
#include "inkpot_xlate.h"

static int string_cmpf (const char *k, const char *b)
{
    for ( ; *k && *b; k++, b++) {
	if (*k > *b) return 1;
	if (*k < *b) return -1;
    }
    if (*k) return 1;  /* if the next char in key is not '\0', then key is longer */
    if (*b) return -1;  /* if the next char in base is not '\0', then base is longer */
    return 0;
}

static int inkpot_scheme_name_cmpf ( const void *key, const void *base)
{
    const char *k = (const char*)key;
    const char *b = &TAB_STRINGS[((inkpot_scheme_name_t *)base)->string_idx];

    return string_cmpf(k, b);
}

static inkpot_scheme_name_t *inkpot_find_scheme_name ( const char *scheme )
{
    if (scheme == NULL)
        return NULL;
    return (inkpot_scheme_name_t *) bsearch(
            (void*)scheme, (void*)TAB_SCHEMES_NAME,
            SZT_SCHEMES_NAME, sizeof(inkpot_scheme_name_t),
            inkpot_scheme_name_cmpf); 
}

static int inkpot_scheme_index_cmpf ( const void *key, const void *base)
{
    const char *k = (const char*)key;
    const char *b = &TAB_STRINGS[((inkpot_scheme_index_t *)base)->string_idx];

    return string_cmpf(k, b);
}

static inkpot_scheme_index_t *inkpot_find_scheme_index ( const char *scheme )
{
    if (scheme == NULL)
        return NULL;
    return (inkpot_scheme_index_t *) bsearch(
            (void*)scheme, (void*)TAB_SCHEMES_INDEX,
            SZT_SCHEMES_INDEX, sizeof(inkpot_scheme_index_t),
            inkpot_scheme_index_cmpf); 
}

inkpot_status_t inkpot_clear ( inkpot_t *inkpot)
{
    inkpot->scheme_bits = 0;  /* clear schemes */
    inkpot->scheme_index = NULL;
    inkpot->name = NULL;     /* clear cached value */
    return INKPOT_SUCCESS;
}

inkpot_status_t inkpot_init ( inkpot_t **inkpot)
{
    *inkpot = malloc(sizeof(inkpot_t));
    if (*inkpot == NULL)
	return INKPOT_MALLOC_FAIL;
    return inkpot_clear ( *inkpot );
}

inkpot_status_t inkpot_activate ( inkpot_t *inkpot, const char *scheme )
{
    inkpot_scheme_name_t *inkpot_scheme_name;
    inkpot_scheme_index_t *inkpot_scheme_index;
    IDX_SCHEMES_NAME idx;

    if (scheme == NULL)
	return INKPOT_SCHEME_UNKNOWN;
    inkpot_scheme_name = inkpot_find_scheme_name(scheme);
    if (inkpot_scheme_name) {
        idx = inkpot_scheme_name - TAB_SCHEMES_NAME;
        if (! inkpot->scheme_bits) {
            inkpot->default_scheme_name_idx = idx; /* first scheme is default */
            inkpot->default_value_idx = TAB_NAMES[inkpot_scheme_name->default_name_idx].value_idx;
            inkpot->first_name_idx = inkpot_scheme_name->first_name_idx;
            inkpot->last_name_idx = inkpot_scheme_name->last_name_idx;
        }
        else {
                inkpot->first_name_idx = MIN(inkpot->first_name_idx, inkpot_scheme_name->first_name_idx);
                inkpot->last_name_idx = MAX(inkpot->last_name_idx, inkpot_scheme_name->last_name_idx);
        }
	if (! (inkpot->scheme_bits & (1 << idx))) {
        	inkpot->scheme_bits |= 1 << idx;
        	inkpot->name = NULL;     /* clear cached name */
	}
        return INKPOT_SUCCESS;
    }

    inkpot_scheme_index = inkpot_find_scheme_index(scheme);
    if (! inkpot_scheme_index)
        return INKPOT_SCHEME_UNKNOWN;
    if (inkpot->scheme_index != inkpot_scheme_index) {
        inkpot->scheme_index = inkpot_scheme_index;
	if (! inkpot->scheme_bits ) {
	    /* Set a default color from an index scheme only if no
	     * named schemes are currently active */
	    inkpot->default_value_idx = TAB_IXVALUES[inkpot_scheme_index->first_value_idx];
	}
	inkpot->name = NULL;     /* clear cached name */
    }
    return INKPOT_SUCCESS;
}
 
inkpot_status_t inkpot_translate ( inkpot_t *inkpot, const char *scheme )
{
    inkpot_scheme_name_t *inkpot_scheme_name;
    inkpot_scheme_index_t *inkpot_scheme_index;
    IDX_SCHEMES_NAME idx;

    if (scheme == NULL)
	return INKPOT_SCHEME_UNKNOWN;
    inkpot_scheme_name = inkpot_find_scheme_name(scheme);
    if (inkpot_scheme_name) {
        idx = inkpot_scheme_name - TAB_SCHEMES_NAME;
	if (! (inkpot->out_scheme_bit & (1 << idx))) {
        	inkpot->out_scheme_bit = 1 << idx;
		inkpot->out_scheme_index = NULL;
        	inkpot->out_name = NULL;     /* clear cached name */
	}
	return INKPOT_SUCCESS;
    }
    inkpot_scheme_index = inkpot_find_scheme_index(scheme);
    if (! inkpot_scheme_index)
	return INKPOT_SCHEME_UNKNOWN;
    if (inkpot->out_scheme_index != inkpot_scheme_index) {
	inkpot->out_scheme_index = inkpot_scheme_index;
       	inkpot->out_scheme_bit = 0;
	inkpot->out_name = NULL;     /* clear cached name */
    }
    return INKPOT_SUCCESS;
}
 
static int inkpot_name_cmpf ( const void *key, const void *base)
{
    const char *k = (const char*)key;
    const char *b = &TAB_STRINGS[((inkpot_name_t *)base)->string_idx];

    return string_cmpf(k, b);
}

static inkpot_status_t inkpot_set_name ( inkpot_t *inkpot, const char *color )
{
    inkpot_name_t *name;
    const char *last_color;
    IDX_NAMES j;
    IDX_STRINGS k;

    if (inkpot == NULL)
        return INKPOT_SCHEME_UNKNOWN;
    if (color == NULL)
        return INKPOT_COLOR_UNKNOWN;
    if (! inkpot->name  /* if we can't use the last result */
		|| ! ((last_color = &TAB_STRINGS[inkpot->name->string_idx]))
		|| ( last_color[0] != color[0] )
		|| ( strcmp(last_color, color) != 0)) {
        name = (inkpot_name_t *) bsearch(  /* then do a fresh search */
            (void*)color, (void*)(&TAB_NAMES[inkpot->first_name_idx]),
            inkpot->last_name_idx + 1 - inkpot->first_name_idx,
            sizeof(inkpot_name_t), inkpot_name_cmpf); 
	if (name == NULL) 
            return INKPOT_COLOR_UNKNOWN;
	
	j = name - TAB_NAMES;
	k = TAB_NAMES[j].string_idx;
	while ( j < SZT_NAMES && k == TAB_NAMES[j].string_idx && ! (inkpot->scheme_bits & TAB_NAMES[j].scheme_bits)) {
	    /* There can be multiple entries for the same
             * color string with different values. Linearly search
             * through them for the first one in the requested scheme(s) */
            j++;
	}
	if (k != TAB_NAMES[j].string_idx || j == SZT_NAMES)
	    return INKPOT_COLOR_UNKNOWN;
	inkpot->name = &TAB_NAMES[j];  /* cache name resolution */
    }
    inkpot->value_idx = inkpot->name->value_idx;
    return INKPOT_SUCCESS;
}

static inkpot_status_t inkpot_set_index ( inkpot_t *inkpot, int index )
{
    inkpot_scheme_index_t *scheme_index;
    IDX_SCHEMES_INDEX j;
    IDX_IXVALUES first, last;
    IDX_VALUES v;

    scheme_index = inkpot->scheme_index;
    if (!scheme_index)
        return INKPOT_SCHEME_UNKNOWN;

    first = scheme_index->first_value_idx;
    j = scheme_index - TAB_SCHEMES_INDEX;
    if (++j >= SZT_SCHEMES_INDEX)
	last = SZT_IXVALUES;
    else
	last = TAB_SCHEMES_INDEX[j].first_value_idx;

    /* clip user-provided index to fit available range */
    index = MAX(index, 0);
    index = MIN(index, (last-1-first));
    index += first;

    assert(index < SZT_IXVALUES);
    v = inkpot->value_idx = TAB_IXVALUES[index];
    if (v >= SZT_VALUES)
        assert(v < SZT_VALUES + SZT_NONAME_VALUES);
    return INKPOT_SUCCESS;
}

inkpot_status_t inkpot_set( inkpot_t *inkpot, const char *color )
{
    int index, r, g, b, a=255;
    unsigned char rgba[4];
    inkpot_status_t rc = INKPOT_COLOR_UNKNOWN;

    if (!color)
	return INKPOT_COLOR_UNKNOWN;

    if (sscanf(color, "#%2x%2x%2x%2x", &r, &g, &b, &a) >= 3) {
	rgba[0] = r;
	rgba[1] = g;
	rgba[2] = b;
	rgba[3] = a;
	rc = inkpot_set_rgba(inkpot, rgba);
    }

    if (rc != INKPOT_SUCCESS)
        if (sscanf(color, "%d", &index) == 1)
            rc = inkpot_set_index(inkpot, index);

    if (rc != INKPOT_SUCCESS)
        rc = inkpot_set_name(inkpot, color);

    return rc;
}

inkpot_status_t inkpot_set_default( inkpot_t *inkpot )
{
    if (inkpot->value_idx != inkpot->default_value_idx) {
	inkpot->value_idx = inkpot->default_value_idx;
	inkpot->name = NULL;   /* FIXME - should set this */
	inkpot->out_name = NULL;
    }

    return INKPOT_SUCCESS;
}

static int inkpot_rgba_cmpf ( const void *key, const void *base)
{
    unsigned char *rgba_key = (unsigned char *)key;
    unsigned char *rgba_base = (unsigned char *)base;

    if (*rgba_key   > *rgba_base  ) return  1;
    if (*rgba_key++ < *rgba_base++) return -1;
    if (*rgba_key   > *rgba_base  ) return  1;
    if (*rgba_key++ < *rgba_base++) return -1;
    if (*rgba_key   > *rgba_base  ) return  1;
    if (*rgba_key++ < *rgba_base++) return -1;
    if (*rgba_key   > *rgba_base  ) return  1;
    if (*rgba_key++ < *rgba_base++) return -1;
    return 0;
}

inkpot_status_t inkpot_set_rgba ( inkpot_t *inkpot, unsigned char *rgba ) 
{
    inkpot_value_t *value;
    inkpot_name_t *name;
    inkpot_noname_value_t *noname_value;
    IDX_VALUES value_idx;
    IDX_NAMES i;

/*
 * FIXME - implement caching and check here
 */

    value = (inkpot_value_t *) bsearch(
        (void*)(rgba), (void*)TAB_VALUES,
        SZT_VALUES, sizeof(inkpot_value_t),
        inkpot_rgba_cmpf); 

    if (value) {
	value_idx = value - TAB_VALUES;
	if (inkpot->value_idx != value_idx) {
	    inkpot->value_idx = value_idx;
	    inkpot->name = NULL;  /* clear name */
	    inkpot->out_name = NULL;  /* clear translated name */
            for (i = value->toname_idx; i < SZT_NAMES; i++) {
                name = &TAB_NAMES[TAB_NAMES[i].toname_idx];
                if (name->value_idx != value_idx)
                    break;
                if (name->scheme_bits & inkpot->scheme_bits) {
		    inkpot->name = name;  /* if the name is in the current scheme, then record it */
		    break;
                }
            }
	}
	return INKPOT_SUCCESS;
    }

    noname_value = (inkpot_noname_value_t *) bsearch(
        (void*)(rgba), (void*)TAB_NONAME_VALUES,
        SZT_NONAME_VALUES, sizeof(inkpot_noname_value_t),
        inkpot_rgba_cmpf); 

    if (noname_value) {

	value_idx = (noname_value - TAB_NONAME_VALUES) + SZT_VALUES;
	if (inkpot->value_idx != value_idx) {
	    inkpot->value_idx = value_idx;
	    inkpot->name = NULL;  /* clear name */
	    inkpot->out_name = NULL;  /* clear translated name */
	}
	return INKPOT_SUCCESS;
    }

    return INKPOT_COLOR_NONAME;
#if 0
    /* need some sort of btree here so that we can insert rgba
     * values and keep sorted */

    noname_value = (inkpot_noname_value_t *) bsearch(
        (void*)(rgba), (void*)TAB_DYNAMIC_VALUES,
        SZT_DYNAMIC_VALUES, sizeof(inkpot_noname_value_t),
        inkpot_rgba_cmpf); 
    
    /* insert value and keep sorted */

    return INKPOT_SUCCESS;

#endif
}

inkpot_status_t inkpot_get ( inkpot_t *inkpot, const char **color )
{
    inkpot_name_t *out_name;
    IDX_NAMES t;
    IDX_VALUES value_idx;
    
    out_name = inkpot->out_name;
    if (out_name) {  /* if we have a cached name */
    	*color = &TAB_STRINGS[out_name->string_idx];
        return INKPOT_SUCCESS;
    }
    
    if (inkpot->out_scheme_bit) {
        value_idx = inkpot->value_idx;
        if (value_idx < SZT_VALUES) {
            for (t = TAB_VALUES[value_idx].toname_idx; t < SZT_NAMES; t++) {
                out_name = &TAB_NAMES[TAB_NAMES[t].toname_idx];
                if (out_name->value_idx != value_idx)
                    break;
                if (out_name->scheme_bits & inkpot->out_scheme_bit) {
	            inkpot->out_name = out_name;
                    *color = &TAB_STRINGS[out_name->string_idx];
                    return INKPOT_SUCCESS;
                 }
            }
            *color = NULL;
	    return INKPOT_COLOR_NONAME;
        }
        value_idx -= SZT_VALUES;
        if (value_idx < SZT_NONAME_VALUES) {
            *color = NULL;
	    return INKPOT_COLOR_NONAME;
        }
        assert(0);  /* support for dynamic values to go here */
    }

    if (inkpot->out_scheme_index) {
	*color = NULL;
	return INKPOT_COLOR_NONAME;
    }

    *color = NULL;
    return INKPOT_FAIL;
}

inkpot_status_t inkpot_get_rgba ( inkpot_t *inkpot, unsigned char *rgba )
{
    IDX_VALUES value_idx = inkpot->value_idx;
    unsigned char *p = rgba, *q;
    int m;

    if (value_idx < SZT_VALUES)
	q = TAB_VALUES[value_idx].rgba;
    else {
	assert (value_idx < SZT_VALUES + SZT_NONAME_VALUES);
	q = TAB_NONAME_VALUES[value_idx - SZT_VALUES].rgba;
    }
    for (m = 0; m < 4; m++) *p++ = *q++;

    return INKPOT_SUCCESS;
}

inkpot_status_t inkpot_get_hsva ( inkpot_t *inkpot, unsigned char *hsva )
{
    /* FIXME */
    return INKPOT_FAIL;
}

inkpot_status_t inkpot_get_cmyk ( inkpot_t *inkpot, unsigned char *cmyk )
{
    /* FIXME */
    return INKPOT_FAIL;
}

inkpot_status_t inkpot_get_RGBA ( inkpot_t *inkpot, double *RGBA )
{
    /* FIXME */
    return INKPOT_FAIL;
}

inkpot_status_t inkpot_get_HSVA ( inkpot_t *inkpot, double *HSVA )
{
    /* FIXME */
    return INKPOT_FAIL;
}

inkpot_status_t inkpot_get_index ( inkpot_t *inkpot, unsigned int *index )
{
    /* FIXME */
    return INKPOT_FAIL;
}

inkpot_status_t inkpot_print_schemes( inkpot_t *inkpot, FILE *out )
{
    IDX_SCHEMES_NAME i;
    int found;

    fprintf(out, "schemes:\n");
    for (i = 0; i < SZT_SCHEMES_NAME; i++) {
	found = 0;
        if ((1<<i) & inkpot->scheme_bits) {
            fprintf (out, "%s", &TAB_STRINGS[TAB_SCHEMES_NAME[i].string_idx]);
            if (i == inkpot->default_scheme_name_idx) 
                fprintf (out, " (default)");
	    found++;
        }
        if ((1<<i) & inkpot->out_scheme_bit) {
	    if (! found)
                fprintf (out, "%s (out)", &TAB_STRINGS[TAB_SCHEMES_NAME[i].string_idx]);
	    else
                fprintf (out, " (out)");
	    found++;
        }
	if (found)
            fprintf (out, "\n");
    }
    found = 0;
    if (inkpot->scheme_index) {
        fprintf (out, "%s (indexed)", &TAB_STRINGS[inkpot->scheme_index->string_idx]);
	found++;
    }
    if (inkpot->out_scheme_index) {
	if (! found)
            fprintf (out, "%s (indexed) (out)", &TAB_STRINGS[inkpot->out_scheme_index->string_idx]);
        else
            fprintf (out, " (out)");
	found++;
    }
    if (found)
        fprintf(out, "\n");
    fprintf(out, "\n");

    return INKPOT_SUCCESS;
}

static inkpot_status_t inkpot_print_scheme_names( inkpot_t *inkpot, int scheme_bits, FILE *out)
{
    IDX_SCHEMES_NAME i;
    int found = 0;

    fprintf(out, "(");
    for (i = 0; i < SZT_SCHEMES_NAME; i++) {
        if ((1 << i) & scheme_bits) {
            if (found++)
                fprintf(out, " ");
            fprintf (out, "%s", &TAB_STRINGS[TAB_SCHEMES_NAME[i].string_idx]);
        }
    }
    fprintf(out, ")");

    return INKPOT_SUCCESS;
}

static inkpot_status_t inkpot_print_rgba( unsigned char *rgba, FILE *out )
{
    fprintf (out, "%d,", *rgba++);
    fprintf (out, "%d,", *rgba++);
    fprintf (out, "%d,", *rgba++);
    fprintf (out, "%d", *rgba);

    return INKPOT_SUCCESS;
}

inkpot_status_t inkpot_print_names( inkpot_t *inkpot, FILE *out )
{
    inkpot_name_t *name;
    inkpot_scheme_index_t *scheme_index;
    IDX_NAMES i;
    IDX_SCHEMES_INDEX j;
    BIT_SCHEMES_NAME inkpot_scheme_bits, scheme_bits;
    IDX_IXVALUES k, first, last;
    IDX_VALUES v;

    fprintf(out, "names:\n");
    inkpot_scheme_bits = inkpot->scheme_bits;
    if (inkpot_scheme_bits) {
        for (i = 0; i < SZT_NAMES; i++) {
            name = &TAB_NAMES[i];
	    scheme_bits = name->scheme_bits & inkpot_scheme_bits;
            if (scheme_bits) {
                fprintf(out, "%s", &TAB_STRINGS[TAB_NAMES[i].string_idx]);
		inkpot_print_scheme_names(inkpot, scheme_bits, out);
		fprintf(out, " ");
		inkpot_print_rgba(TAB_VALUES[name->value_idx].rgba, out);
		fprintf(out, "\n");
            }
        }
    }
    scheme_index = inkpot->scheme_index;
    if (scheme_index) {

	first = scheme_index->first_value_idx;
	j = scheme_index - TAB_SCHEMES_INDEX;
	if (++j >= SZT_SCHEMES_INDEX)
	    last = SZT_IXVALUES;
	else
	    last = TAB_SCHEMES_INDEX[j].first_value_idx;

	for (k = first; k < last; k++) {
	    v = TAB_IXVALUES[k];
	    fprintf (out, "%d(%s) ", k - first , &TAB_STRINGS[scheme_index->string_idx]);
	    if (v < SZT_VALUES)
	        inkpot_print_rgba(TAB_VALUES[v].rgba, out);
	    else
	        inkpot_print_rgba(TAB_NONAME_VALUES[v - SZT_VALUES].rgba, out);
	    fprintf(out, "\n");
	}
    }
    fprintf(out, "\n");

    return INKPOT_SUCCESS;
}

/* Print all values that are members of the currently activated name schemes, with the names in those schemes */
/*    Does not print the indexes in index schemes that a value may be a member of. */
inkpot_status_t inkpot_print_values( inkpot_t *inkpot, FILE *out )
{
    inkpot_value_t *value;
    inkpot_name_t *name;
    IDX_VALUES i;
    IDX_NAMES t;
    BIT_SCHEMES_NAME scheme_bits;
    int found;

    fprintf(out, "values:\n");
    for (i = 0; i < SZT_VALUES; i++) {
        value = &TAB_VALUES[i];
        found = 0;
        for (t = value->toname_idx; t < SZT_NAMES; t++) {
            name = &TAB_NAMES[TAB_NAMES[t].toname_idx];
            if (name->value_idx != i)
                break;
            scheme_bits = name->scheme_bits & inkpot->scheme_bits;
            if (scheme_bits) {
                if (found++)
                    fprintf(out, " ");
                else
		    inkpot_print_rgba(TAB_VALUES[i].rgba, out);
                fprintf(out, " %s", &TAB_STRINGS[name->string_idx]);
		inkpot_print_scheme_names(inkpot, scheme_bits, out);
            }
        }
        if (found)
            fprintf(out, "\n");
    }
    fprintf(out, "\n");

    return INKPOT_SUCCESS;
}
