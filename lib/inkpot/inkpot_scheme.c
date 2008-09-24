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

inkpot_status_t inkpot_add ( inkpot_t *inkpot, const char *scheme )
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
        	inkpot->name = NULL;     /* clear cached value */
	}
    }
    else {
	inkpot_scheme_index = inkpot_find_scheme_index(scheme);
	if (! inkpot_scheme_index)
            return INKPOT_SCHEME_UNKNOWN;
	if (inkpot->scheme_index != inkpot_scheme_index) {
            if (inkpot->scheme_index)
                return INKPOT_MAX_ONE_INDEXED_SCHEME;
            inkpot->scheme_index = inkpot_scheme_index;
	    if (! inkpot->scheme_bits ) {
		/* Set a default color from an index scheme only if no named schemes
		 * have been specified yet. Will be overwritten by the default of the
		 * first name scheme if given later */
	        inkpot->default_value_idx = TAB_IXVALUES[inkpot_scheme_index->first_value_idx];
	    }
	    inkpot->name = NULL;     /* clear cached value */
	}
    }
    return INKPOT_SUCCESS;
}
 
static int inkpot_name_cmpf ( const void *key, const void *base)
{
    const char *k = (const char*)key;
    const char *b = &TAB_STRINGS[((inkpot_name_t *)base)->string_idx];

    return string_cmpf(k, b);
}

static inkpot_status_t inkpot_find_name ( inkpot_t *inkpot, const char *color )
{
    inkpot_name_t *name;
    const char *last_name;
    IDX_NAMES j;
    IDX_STRINGS k;

    if (inkpot == NULL)
        return INKPOT_SCHEME_UNKNOWN;
    if (name == NULL)
        return INKPOT_COLOR_UNKNOWN;
    if (! inkpot->name  /* if we can't use the last result */
		|| ! ((last_name = &TAB_STRINGS[inkpot->name->string_idx]))
		|| ( last_name[0] != color[0] )
		|| ( strcmp(last_name, color) != 0)) {
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
    inkpot->value = &TAB_VALUES[inkpot->name->value_idx];
    return INKPOT_SUCCESS;
}

static inkpot_status_t inkpot_find_index ( inkpot_t *inkpot, int index )
{
    inkpot_scheme_index_t *scheme_index;
    IDX_SCHEMES_INDEX j;
    IDX_IXVALUES first, last;

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
    assert(TAB_IXVALUES[index] < SZT_VALUES);
    inkpot->value = &TAB_VALUES[TAB_IXVALUES[index]];
    return INKPOT_SUCCESS;
}

inkpot_status_t inkpot_find( inkpot_t *inkpot, const char *color )
{
    int index;
    inkpot_status_t rc = INKPOT_COLOR_UNKNOWN;

    if (color && sscanf(color, "%d", &index) == 1)
        rc = inkpot_find_index(inkpot, index);

    if (rc != INKPOT_SUCCESS)
        rc = inkpot_find_name(inkpot, color);

    return rc;
}

inkpot_status_t inkpot_find_default( inkpot_t *inkpot )
{
    inkpot->value = &TAB_VALUES[inkpot->default_value_idx];
    return INKPOT_SUCCESS;
}

static int inkpot_value_cmpf ( const void *key, const void *base)
{
    inkpot_value_t *cv_key = (inkpot_value_t*)key;
    inkpot_value_t *cv_base = (inkpot_value_t*)base;

    if (cv_key->r > cv_base->r) return 1;
    if (cv_key->r < cv_base->r) return -1;
    if (cv_key->g > cv_base->g) return 1;
    if (cv_key->g < cv_base->g) return -1;
    if (cv_key->b > cv_base->b) return 1;
    if (cv_key->b < cv_base->b) return -1;
    if (cv_key->a > cv_base->a) return 1;
    if (cv_key->a < cv_base->a) return -1;
    return 0;
}

static int inkpot_find_value ( inkpot_t *inkpot ) 
{
    inkpot_value_t *rc, *value = inkpot->value;
    inkpot_name_t *name;
    inkpot_scheme_index_t *scheme_index;
    IDX_VALUES value_idx;
    IDX_NAMES i;
    IDX_IXVALUES k, first, last;
    IDX_SCHEMES_INDEX j;

    if (! inkpot->value->is_named) {
    	rc = (inkpot_value_t *) bsearch(
            (void*)(value), (void*)TAB_VALUES,
            SZT_VALUES, sizeof(inkpot_value_t),
            inkpot_value_cmpf); 
        if (rc)
	    inkpot->value->is_named = 1;
    }
    if (inkpot->value->is_named) {
	value_idx = value - TAB_VALUES;
        for (i = value->toname_idx; i < SZT_NAMES; i++) {
            name = &TAB_NAMES[TAB_NAMES[i].toname_idx];
            if (name->value_idx != value_idx)
                break;
            if (name->scheme_bits & inkpot->scheme_bits) {
		inkpot->name = name;
		return INKPOT_SUCCESS;
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
                if (TAB_IXVALUES[k] == value_idx) {
		    inkpot->name = NULL;
		    inkpot->index = k - first;
                    return INKPOT_SUCCESS;
                }
            }
        }
    }
    return INKPOT_COLOR_UNKNOWN;
}

void inkpot_get_rgba ( inkpot_t *inkpot, unsigned int *r, unsigned int *g, unsigned int *b, unsigned int *a)
{
    unsigned char *t = &(inkpot->value->r);

    if (r) *r = *t; t++;
    if (g) *g = *t; t++;
    if (b) *b = *t; t++;
    if (a) *a = *t; t++;
}

void inkpot_get_hsva ( inkpot_t *inkpot, unsigned int *h, unsigned int *s, unsigned int *v, unsigned int *a)
{
    unsigned char *t = &(inkpot->value->a);

    if (a) *a = *t; t++;
    if (h) *h = *t; t++;
    if (s) *s = *t; t++;
    if (v) *v = *t; t++;
}

inkpot_status_t inkpot_xlate(inkpot_t *from_scheme, inkpot_t *to_scheme)
{
    return inkpot_find_value(to_scheme);
}

void inkpot_print_schemes( inkpot_t *inkpot )
{
    IDX_SCHEMES_NAME i;

    fprintf(stderr, "schemes:\n");
    for (i = 0; i < SZT_SCHEMES_NAME; i++) {
        if ((1<<i) & inkpot->scheme_bits) {
            fprintf (stderr, "%s", &TAB_STRINGS[TAB_SCHEMES_NAME[i].string_idx]);
            if (i == inkpot->default_scheme_name_idx) 
                fprintf (stderr, " (default)");
            fprintf (stderr, "\n");
        }
    }
    if (inkpot->scheme_index)
        fprintf (stderr, "%s (indexed)\n", &TAB_STRINGS[inkpot->scheme_index->string_idx]);
    fprintf(stderr, "\n");
}

static void inkpot_print_scheme_names( inkpot_t *inkpot, int scheme_bits)
{
    IDX_SCHEMES_NAME i;
    int found = 0;

    fprintf(stderr, "(");
    for (i = 0; i < SZT_SCHEMES_NAME; i++) {
        if ((1 << i) & scheme_bits) {
            if (found++)
                fprintf(stderr, " ");
            fprintf (stderr, "%s", &TAB_STRINGS[TAB_SCHEMES_NAME[i].string_idx]);
        }
    }
    fprintf(stderr, ")");
}

static void inkpot_print_rgba_value( inkpot_value_t *value )
{
    fprintf (stderr, "%d,%d,%d,%d",
	value->r, value->g, value->b, value->a);
}

void inkpot_print_rgba( inkpot_t *inkpot )
{
    inkpot_print_rgba_value( inkpot->value );
}

static void inkpot_print_hsva_value(inkpot_value_t *value )
{
    fprintf (stderr, " %d,%d,%d,%d\n",
	value->h, value->s, value->v, value->a);
}

void inkpot_print_hsva( inkpot_t *inkpot )
{
    inkpot_print_hsva_value( inkpot->value );
}

void inkpot_print_names( inkpot_t *inkpot )
{
    inkpot_name_t *name;
    inkpot_value_t *value;
    inkpot_scheme_index_t *scheme_index;
    IDX_NAMES i;
    IDX_SCHEMES_INDEX j;
    BIT_SCHEMES_NAME inkpot_scheme_bits, scheme_bits;
    IDX_IXVALUES k, first, last;

    fprintf(stderr, "names:\n");
    inkpot_scheme_bits = inkpot->scheme_bits;
    if (inkpot_scheme_bits) {
        for (i = 0; i < SZT_NAMES; i++) {
            name = &TAB_NAMES[i];
	    scheme_bits = name->scheme_bits & inkpot_scheme_bits;
            if (scheme_bits) {
                value = &TAB_VALUES[name->value_idx];
                fprintf(stderr, "%s", &TAB_STRINGS[TAB_NAMES[i].string_idx]);
		inkpot_print_scheme_names(inkpot, scheme_bits);
		fprintf(stderr, " ");
		inkpot_print_rgba_value(value);
		fprintf(stderr, "\n");
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
	    value = &TAB_VALUES[TAB_IXVALUES[j]];
	    fprintf (stderr, "%d(%s) ", k - first , &TAB_STRINGS[scheme_index->string_idx]);
	    inkpot_print_rgba_value(value);
	    fprintf(stderr, "\n");
	}
    }
    fprintf(stderr, "\n");
}

void inkpot_print_values( inkpot_t *inkpot )
{
    inkpot_value_t *value;
    inkpot_name_t *name;
    inkpot_scheme_index_t *scheme_index;
    IDX_VALUES i;
    IDX_NAMES t;
    BIT_SCHEMES_NAME scheme_bits;
    IDX_SCHEMES_INDEX j;
    IDX_IXVALUES k, first, last;
    int found;

    fprintf(stderr, "values:\n");
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
                    fprintf(stderr, " ");
                else
		    inkpot_print_rgba_value(value);
                fprintf(stderr, " %s", &TAB_STRINGS[name->string_idx]);
		inkpot_print_scheme_names(inkpot, scheme_bits);
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
                if (TAB_IXVALUES[k] == i) {
                    if (found++)
                        fprintf(stderr, " ");
                    else {
			inkpot_print_rgba_value(value);
                        fprintf(stderr, " ");
		    }
                    fprintf(stderr, "%d(%s)", k - first, &TAB_STRINGS[scheme_index->string_idx]);
                    break;
                }
            }
        }
        if (found)
            fprintf(stderr, "\n");
    }
    fprintf(stderr, "\n");
}
