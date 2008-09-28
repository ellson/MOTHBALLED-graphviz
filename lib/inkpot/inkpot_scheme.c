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
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "inkpot.h"
#include "inkpot_define.h"
#include "inkpot_value.h"
#include "inkpot_struct.h"
#include "inkpot_name_table.h"
#include "inkpot_xlate.h"

static size_t inkpot_writer (void *closure, const char *data, size_t length)
{
    return fwrite(data, sizeof(char), length, (FILE *)closure);
}

static inkpot_disc_t inkpot_default_disc = { inkpot_writer, inkpot_writer };

inkpot_t *inkpot_init ( void )
{
    inkpot_t *inkpot;
    IDX_MRU_CACHE i;
   
    inkpot = malloc(sizeof(inkpot_t));
    if (inkpot) {

	inkpot->values = inkpot_values_init ();
	if (! inkpot->values) {
	    free(inkpot);
	    return NULL;
	}

	inkpot->canon = NULL;
	inkpot->canon_alloc = 0;

	inkpot->disc = inkpot_default_disc;
	inkpot->out_closure = stdout;
	inkpot->err_closure = stderr;

	inkpot->most_recently_used_idx = 0;
        for (i = 0; i < SZT_MRU_CACHE; i++)
	    inkpot->cache[i].next_recently_used_idx = i + 1;

        inkpot->scheme_bits = 0;  /* clear schemes */
        inkpot->active_schemes = 0;
        inkpot->active_out_schemes = 0;
        inkpot->out_name = NULL; 
    }
    return inkpot;
}

void inkpot_destroy ( inkpot_t *inkpot )
{
    inkpot_values_destroy ( inkpot->values );
    free(inkpot->canon);
    free(inkpot);
}

inkpot_status_t inkpot_disciplines ( inkpot_t *inkpot, inkpot_disc_t disc, void *out, void *err )
{
    inkpot->disc = disc;
    inkpot->out_closure = out;
    inkpot->err_closure = err;

    return ((inkpot->status = INKPOT_SUCCESS));
}

/* FIXME - this needs to work with UTF-8 strings */
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

static inkpot_status_t inkpot_scheme ( inkpot_t *inkpot, const char *scheme )
{
    inkpot_scheme_name_t *inkpot_scheme_name;
    inkpot_scheme_index_t *inkpot_scheme_index;
    IDX_SCHEMES_INDEX i;

    if (scheme == NULL)
        return ((inkpot->status = INKPOT_SCHEME_UNKNOWN));

    inkpot_scheme_name = inkpot_find_scheme_name(scheme);
    if (inkpot_scheme_name) {
        inkpot->scheme_bits |= 1 << (inkpot_scheme_name - TAB_SCHEMES_NAME);
        return ((inkpot->status = INKPOT_SUCCESS));
    }

    inkpot_scheme_index = inkpot_find_scheme_index(scheme);
    if (! inkpot_scheme_index)
        return ((inkpot->status = INKPOT_SCHEME_UNKNOWN));
    
    for (i = 0; i < inkpot->active_schemes; i++)
        if (inkpot->scheme_list[i] == (inkpot_scheme_index - TAB_SCHEMES_INDEX))
	    return ((inkpot->status = INKPOT_SUCCESS));

    inkpot->scheme_list[i] = (inkpot_scheme_index - TAB_SCHEMES_INDEX);
    inkpot->active_schemes++;
    return ((inkpot->status = INKPOT_SUCCESS));
}
 
inkpot_status_t inkpot_schemes ( inkpot_t *inkpot, const char *schemes )
{
    inkpot_status_t rc;
    const char *q;
    char *p, *s, c;
    int len;

    if (schemes == NULL)
        return ((inkpot->status = INKPOT_SCHEME_UNKNOWN));

    inkpot->scheme_bits = 0;  /* clear old input schemes */
    inkpot->active_schemes = 0;
    inkpot->out_name = NULL;

    len = strlen(schemes);
    if (len >= inkpot->canon_alloc) {
	inkpot->canon_alloc = len + 1 + 10;
	inkpot->canon = realloc(inkpot->canon, inkpot->canon_alloc);
	if (! inkpot->canon)
	    return ((inkpot->status = INKPOT_MALLOC_FAIL));
    }

    q = schemes;
    p = inkpot->canon;
    for (c = *q; c;) {
        s = p;
        while ((c = *q++) && c != ' ' && c != '\t' && c != ',') {*p++ = c;}
	*p++ = '\0';
	if (*s) {
	    rc = inkpot_scheme(inkpot, s);
            if (rc != INKPOT_SUCCESS)
	        return rc;
	}
    }
    return rc;
}
 
inkpot_status_t inkpot_translate ( inkpot_t *inkpot, const char *scheme )
{
    inkpot_scheme_name_t *inkpot_scheme_name;
    inkpot_scheme_index_t *inkpot_scheme_index;
    IDX_SCHEMES_NAME idx;

    if (scheme == NULL)
        return ((inkpot->status = INKPOT_SCHEME_UNKNOWN));

    inkpot_scheme_name = inkpot_find_scheme_name(scheme);
    if (inkpot_scheme_name) {
        idx = inkpot_scheme_name - TAB_SCHEMES_NAME;
	if (! (inkpot->out_scheme_bit & (1 << idx))) {
        	inkpot->out_scheme_bit = 1 << idx;
		inkpot->active_out_schemes = 0;
        	inkpot->out_name = NULL;     /* clear cached name */
	}
        return ((inkpot->status = INKPOT_SUCCESS));
    }

    inkpot_scheme_index = inkpot_find_scheme_index(scheme);
    if (! inkpot_scheme_index)
        return ((inkpot->status = INKPOT_SCHEME_UNKNOWN));

    if (inkpot->active_out_schemes == 0 
	    || (inkpot->out_scheme_list[0] != (inkpot_scheme_index - TAB_SCHEMES_INDEX))) {
	inkpot->out_scheme_list[0] = (inkpot_scheme_index - TAB_SCHEMES_INDEX);
	inkpot->active_out_schemes = 1;
       	inkpot->out_scheme_bit = 0;
	inkpot->out_name = NULL;     /* clear cached name */
    }
    return ((inkpot->status = INKPOT_SUCCESS));
}

#if 0
inkpot_status_t inkpot_palette( inkpot_t *inkpot, int size )
{
    if (inkpot->palette_alloc < size * sizeof(inkpot_noname_value_t)) {
        inkpot->palette_alloc = size * sizeof(inkpot_noname_value_t);
	inkpot->palette = realloc(inkpot->palette, inkpot->palette_alloc);
        if (! inkpot->palette)
            return ((inkpot->status = INKPOT_MALLOC_FAIL));
    }
    return ((inkpot->status = INKPOT_SUCCESS));
}
#endif
 
static int inkpot_name_cmpf ( const void *key, const void *base)
{
    const char *k = (const char*)key;
    const char *b = &TAB_STRINGS[((inkpot_name_t *)base)->string_idx];

    return string_cmpf(k, b);
}

static inkpot_status_t inkpot_set_value_idx( inkpot_t *inkpot, IDX_VALUES value_idx)
{
    if (inkpot->value.index != value_idx) {
	inkpot->value.index = value_idx;
	inkpot->out_name = NULL;  /* value changed so invalidate out_name */
    }
    return ((inkpot->status = INKPOT_SUCCESS));
}

static inkpot_status_t inkpot_cache_get( inkpot_t *inkpot )
{
    IDX_MRU_CACHE i;
    IDX_NAMES cache_name_idx;
    const char *cache_color;
    char *color;

    /* The cached value is valid if:
     *     The name schemes are unchanged
     *     The color requested matches the cached color.
     *
     * Its not sufficient for the scheme of the cached value to still be present in the current
     * schemes since the correct resolution may have the same name but different value.
     */
    color = inkpot->canon;
    for (i = 0; i < SZT_MRU_CACHE; i++) {  
	if (inkpot->cache[i].scheme_bits != inkpot->scheme_bits)
	    continue;

	cache_name_idx = inkpot->cache[i].name_idx;
	cache_color = &TAB_STRINGS[TAB_NAMES[cache_name_idx].string_idx];
	if (cache_color[0] != color[0] || (strcmp(cache_color, color) != 0))
	    continue;

	/* found */
	if (i) { /* if it is not already MRU then reorder to make it so */
	    inkpot->cache[i-1].next_recently_used_idx = inkpot->cache[i].next_recently_used_idx;
	    inkpot->cache[i].next_recently_used_idx = inkpot->most_recently_used_idx;
	    inkpot->most_recently_used_idx = i;
	}
        return inkpot_set_value_idx(inkpot, TAB_NAMES[cache_name_idx].value_idx);
    }
    return ((inkpot->status = INKPOT_COLOR_UNKNOWN));
}

static inkpot_status_t inkpot_cache_put ( inkpot_t *inkpot, IDX_NAMES name_idx )
{
    IDX_MRU_CACHE i;

    for (i = 0; i < SZT_MRU_CACHE; i++) {  
	if (inkpot->cache[i].next_recently_used_idx == SZT_MRU_CACHE) {
	    inkpot->cache[i].next_recently_used_idx = inkpot->most_recently_used_idx;
	    inkpot->most_recently_used_idx = i;

	    inkpot->cache[i].scheme_bits = inkpot->scheme_bits;
	    inkpot->cache[i].name_idx = name_idx;
	}
	else
	    inkpot->cache[i].next_recently_used_idx += 1;
    }
    return ((inkpot->status = INKPOT_SUCCESS));
}

static inkpot_status_t inkpot_set_name ( inkpot_t *inkpot )
{
    inkpot_status_t rc;
    char *color;
    inkpot_name_t *name;
    IDX_NAME_ALTS i;
    IDX_NAMES base, top;

    if (inkpot == NULL || ! inkpot->scheme_bits)
        return ((inkpot->status = INKPOT_SCHEME_UNKNOWN));
    if ((color = inkpot->canon) == NULL)
        return ((inkpot->status = INKPOT_COLOR_UNKNOWN));

    rc = inkpot_cache_get(inkpot);
    if (rc == INKPOT_SUCCESS)
	return rc;

    for (i = 0; i < SZT_NAME_ALTS; ) {
	base = TAB_NAME_ALTS[i++];
	if (i == SZT_NAME_ALTS)
	    top = SZT_NAMES;
	else
	    top = TAB_NAME_ALTS[i];

        name = (inkpot_name_t *) bsearch(
            (void*)color, (void*)(&TAB_NAMES[base]),
	    top-base, sizeof(inkpot_name_t),
	    inkpot_name_cmpf); 
	if (name == NULL) 
            return ((inkpot->status = INKPOT_COLOR_UNKNOWN));
	
	if (inkpot->scheme_bits & name->scheme_bits) {
	    rc = inkpot_set_value_idx(inkpot, name->value_idx);
	    assert(rc == INKPOT_SUCCESS);
	    return inkpot_cache_put(inkpot, (name - TAB_NAMES) );
	}
    }
    return ((inkpot->status = INKPOT_COLOR_UNKNOWN));
}

static inkpot_status_t inkpot_set_index ( inkpot_t *inkpot, int index )
{
    IDX_SCHEMES_INDEX j;
    IDX_IXVALUES first, last;
    IDX_VALUES value_idx;

    if (!inkpot->active_schemes)
        return ((inkpot->status = INKPOT_SCHEME_UNKNOWN));

    j = inkpot->scheme_list[0];
    first = TAB_SCHEMES_INDEX[j].first_value_idx;
    if (++j >= SZT_SCHEMES_INDEX)
	last = SZT_IXVALUES;
    else
	last = TAB_SCHEMES_INDEX[j].first_value_idx;
    last = last-1-first;

    /* clip user-provided index to fit available range */
    index = (index < 0)    ? 0    : index;
    index = (index > last) ? last : index;
    index += first;

    assert(index < SZT_IXVALUES);
    value_idx = TAB_IXVALUES[index];
    if (value_idx >= SZT_VALUES)
        assert(value_idx < SZT_VALUES + SZT_NONAME_VALUES);

    return inkpot_set_value_idx(inkpot, value_idx);
}

inkpot_status_t inkpot_set_rgba ( inkpot_t *inkpot, double rgba[4] )
{
    inkpot_status_t rc;
    inkpot_value_t value;
    VALUE v;
    int i;

    for (i = 0; i < 4; i++) {
	value.value <<= SZB_RED;
	v = rgba[i];
	v = (v < 0.0) ? 0.0 : v;
	v = (v > 1.0) ? 1.0 : v;
	value.value |= (int)(v * MAX_RED);
    }
    value.vtype = BIT_VTYPE_size_16 | BIT_VTYPE_code_VALUE | BIT_VTYPE_alpha_yes;
    
    rc = inkpot_value_set ( inkpot->values, &value );
    if (rc == INKPOT_SUCCESS)
	inkpot->value = value;
    return (inkpot->status = rc);
}

inkpot_status_t inkpot_set_hsva ( inkpot_t *inkpot, double hsva[4] )
{
    double rgba[4];

    hsva2rgba( hsva, rgba );

    return inkpot_set_rgba ( inkpot, rgba );
}

inkpot_status_t inkpot_set ( inkpot_t *inkpot, const char *color )
{
    IDX_SCHEMES_INDEX j;
    IDX_IXVALUES first, last;
    IDX_VALUES value_idx;
    char *q, *s;
    const char *p;
    int i, c, len, index;
    unsigned int r, g, b, a;
    inkpot_value_t value;
    double hsva[4];
    inkpot_status_t rc = INKPOT_COLOR_UNKNOWN;

    if (!color)
        return ((inkpot->status = INKPOT_COLOR_UNKNOWN));

    len = strlen(color);
    if (len >= inkpot->canon_alloc) {
	inkpot->canon_alloc = len + 1 + 20;
	inkpot->canon = realloc(inkpot->canon, inkpot->canon_alloc);
	if (! inkpot->canon)
            return ((inkpot->status = INKPOT_MALLOC_FAIL));
    }

    /* canonicalize input string */
    for (p = color, q = inkpot->canon;
		(c = *p) && ( c == ' ' || c == '\t' );
		p++) { }; /* remove leading ws */

    /* change ',' to ' ' */
    while ((c = *p++)) {
	if (c == ',') c = ' ';
	*q++ = c;
    }
    *q = '\0';

    if (*inkpot->canon == '#') {
	a = 65535;
        if ((len = sscanf(inkpot->canon, "#%4x%4x%4x%4x", &r, &g, &b, &a)) >= 3) {
	    r *= MAX_RED/65535; g *= MAX_RED/65535; b *= MAX_RED/65535; a *= MAX_RED/65535;
	}
	if (len < 3) {
	    a = 255;
	    if ((len = sscanf(inkpot->canon, "#%2x%2x%2x%2x", &r, &g, &b, &a)) >= 3) {
	        r *= MAX_RED/255; g *= MAX_RED/255; b *= MAX_RED/255; a *= MAX_RED/255;
   	    } 
	}
	if (len >= 3) {
	    value.value = (((((((VALUE)r) << SZB_RED) | (VALUE)g) << SZB_RED) | (VALUE)b) << SZB_RED) | (VALUE)a;
	    value.vtype = BIT_VTYPE_size_16 | BIT_VTYPE_code_VALUE | BIT_VTYPE_alpha_yes;

	    rc = inkpot_value_set ( inkpot->values, &value );
	    if (rc ==  INKPOT_SUCCESS)
	        inkpot->value = value;
	    inkpot->status = rc;
	}
    }
    else if (((c = *inkpot->canon) == '.') || isdigit(c)) {
	hsva[3] = 1.0;
        if (sscanf(inkpot->canon, "%lf%lf%lf%lf", &hsva[0], &hsva[1], &hsva[2], &hsva[3]) >= 3)
	    rc = inkpot_set_hsva(inkpot, hsva);
	else 
            if (sscanf(inkpot->canon, "%d", &index) == 1)   /* simple indexes */
                rc = inkpot_set_index(inkpot, index);
    }
    if (rc != INKPOT_SUCCESS && inkpot->active_schemes) {   /* pseudo names from scheme_name+index */
        len = strlen(inkpot->canon);
        s = inkpot->canon;
	index = 0;
	i = 1;
	for (q = s + len -1; q >= s; q--) {
	    c = *q;
	    if (! isdigit(c)) break;
	    index += (c - '0') * i;
	    i *= 10; 
	}
	*++q = '\0';
	for (i=0; i < inkpot->active_schemes; i++) {
	    j = inkpot->scheme_list[i];
	    p = &TAB_STRINGS[TAB_SCHEMES_INDEX[j].string_idx];
	    if (*p != *s || strcmp(p, s) != 0) 
		continue;
	    first = TAB_SCHEMES_INDEX[j].first_value_idx;
            if (++j >= SZT_SCHEMES_INDEX)
	        last = SZT_IXVALUES;
	    else
		last = TAB_SCHEMES_INDEX[j].first_value_idx;
	    last = last-1-first;

	    /* clip user-provided index to fit available range */
	    index = (index < 0)    ? 0    : index;
	    index = (index > last) ? last : index;
	    index += first;

	    assert(index < SZT_IXVALUES);
	    value_idx = TAB_IXVALUES[index];
	    if (value_idx >= SZT_VALUES)
		assert(value_idx < SZT_VALUES + SZT_NONAME_VALUES);

	    rc = inkpot_set_value_idx(inkpot, value_idx);
	    break;
	}
	*q = c;  /* repair the damage to inkpot->canon */
    }

    if (rc != INKPOT_SUCCESS) {
	/* remove embedded ws and convert to lower case*/
        for (p = q = inkpot->canon; (c = *p) && ! ( c == ' ' || c == '\t' ); p++) { *q++ = tolower(c); };
	*q = '\0';
        rc = inkpot_set_name(inkpot);
    }

    return rc;
}

inkpot_status_t inkpot_get ( inkpot_t *inkpot, const char **color )
{
    inkpot_name_t *out_name;
    IDX_NAMES t;
    IDX_VALUES value_idx;
    
    out_name = inkpot->out_name;
    if (out_name) {  /* if we have a cached name */
    	*color = &TAB_STRINGS[out_name->string_idx];
        return ((inkpot->status = INKPOT_SUCCESS));
    }
    
    if (inkpot->out_scheme_bit) {
        value_idx = inkpot->value.index;
        if (value_idx < SZT_VALUES) {
            for (t = TAB_VALUES[value_idx].toname_idx; t < SZT_NAMES; t++) {
                out_name = &TAB_NAMES[TAB_NAMES[t].toname_idx];
                if (out_name->value_idx != value_idx)
                    break;
                if (out_name->scheme_bits & inkpot->out_scheme_bit) {
	            inkpot->out_name = out_name;
                    *color = &TAB_STRINGS[out_name->string_idx];
		    return ((inkpot->status = INKPOT_SUCCESS));
                 }
            }
            *color = NULL;
	    return ((inkpot->status = INKPOT_COLOR_NONAME));
        }
        value_idx -= SZT_VALUES;
        if (value_idx < SZT_NONAME_VALUES) {
            *color = NULL;
	    return ((inkpot->status = INKPOT_COLOR_NONAME));
        }
        if (value_idx == SZT_NONAME_VALUES) {
            *color = NULL;
	    return ((inkpot->status = INKPOT_COLOR_NOPALETTE));
        }
        assert(0);  /* support for dynamic values to go here */
    }

    if (inkpot->active_out_schemes) {
	/* FIXME - indexed output scheme - eg. palette */
	*color = NULL;
	return ((inkpot->status = INKPOT_COLOR_NONAME));
    }

    *color = NULL;
    return ((inkpot->status = INKPOT_COLOR_NONAME));
}

inkpot_status_t inkpot_get_rgba_i ( inkpot_t *inkpot, unsigned short rgba[4] )
{
    inkpot_status_t rc;
    inkpot_value_t value;
    int i;

    value = inkpot->value;
    rc = inkpot->status = inkpot_value_get( inkpot->values, &value );
    if (rc == INKPOT_SUCCESS) {
	for (i = 3; i >= 0; i--) {
	    rgba[i] = (value.value & MSK_RED);
	    value.value >>= SZB_RED;
	}
    }
    return rc;
}

inkpot_status_t inkpot_get_rgba ( inkpot_t *inkpot, double rgba[4] )
{
    inkpot_status_t rc;
    inkpot_value_t value;
    int i;

    value = inkpot->value;
    rc = inkpot->status = inkpot_value_get( inkpot->values, &value );
    if (rc == INKPOT_SUCCESS) {
	for (i = 3; i >= 0; i--) {
	    rgba[i] = (value.value & MSK_RED) / (double)MAX_RED;
	    value.value >>= SZB_RED;
	}
    }
    return rc;
}

inkpot_status_t inkpot_get_hsva ( inkpot_t *inkpot, double hsva[4] )
{
    inkpot_status_t rc;
    double rgba[4];

    rc = inkpot_get_rgba(inkpot, rgba);
    if (rc == INKPOT_SUCCESS)
	rgba2hsva( rgba, hsva );
 
    return rc;
}

inkpot_status_t inkpot_get_cmyk ( inkpot_t *inkpot, double cmyk[4] )
{
    inkpot_status_t rc;
    double rgba[4];

    rc = inkpot_get_rgba(inkpot, rgba);
    if (rc == INKPOT_SUCCESS)
	rgba2cmyk( rgba, cmyk );
 
    return rc;
}

#if 0
/* FIXME - requires palette collection and transformation */
inkpot_status_t inkpot_get_index ( inkpot_t *inkpot, unsigned int *index )
{
    /* FIXME */
    return ((inkpot->status = INKPOT_FAIL));
}
#endif

inkpot_status_t inkpot_write ( inkpot_t *inkpot )
{
    inkpot_status_t rc;
    const char *color;

    rc = inkpot_get(inkpot, &color);
    if (rc == INKPOT_SUCCESS)
	inkpot->disc.out_writer(inkpot->out_closure, color, strlen(color));
    return rc;
}

inkpot_status_t inkpot_write_rgba16 ( inkpot_t *inkpot )
{
    inkpot_status_t rc;
    unsigned short rgba[4];
    char buf[17];
    int len;

    rc = inkpot_get_rgba_i ( inkpot, rgba );
    if (rc == INKPOT_SUCCESS) {
        len = sprintf(buf, "%04x%04x%04x%04x", rgba[0], rgba[1], rgba[2], rgba[3]);
	assert(len==16);
        inkpot->disc.out_writer(inkpot->out_closure, buf, len);
    }
    return rc;
}

inkpot_status_t inkpot_write_rgb16 ( inkpot_t *inkpot )
{
    inkpot_status_t rc;
    unsigned short rgba[4];
    char buf[13];
    int len;

    rc = inkpot_get_rgba_i ( inkpot, rgba );
    if (rc == INKPOT_SUCCESS) {
        len = sprintf(buf, "%04x%04x%04x", rgba[0], rgba[1], rgba[2]);
	assert(len==12);
        inkpot->disc.out_writer(inkpot->out_closure, buf, len);
    }
    return rc;
}

inkpot_status_t inkpot_write_rgba8 ( inkpot_t *inkpot )
{
    inkpot_status_t rc;
    unsigned short rgba[4];
    char buf[9];
    int len;

    rc = inkpot_get_rgba_i ( inkpot, rgba );
    if (rc == INKPOT_SUCCESS) {
        len = sprintf(buf, "%02x%02x%02x%02x", rgba[0]>>8, rgba[1]>>8, rgba[2]>>8, rgba[3]>>8);
	assert(len==8);
        inkpot->disc.out_writer(inkpot->out_closure, buf, len);
    }
    return rc;
}

inkpot_status_t inkpot_write_rgb8 ( inkpot_t *inkpot )
{
    inkpot_status_t rc;
    unsigned short rgba[4];
    char buf[7];
    int len;

    rc = inkpot_get_rgba_i ( inkpot, rgba );
    if (rc == INKPOT_SUCCESS) {
        len = sprintf(buf, "%02x%02x%02x", rgba[0]>>8, rgba[1]>>8, rgba[2]>>8);
	assert(len==6);
        inkpot->disc.out_writer(inkpot->out_closure, buf, len);
    }
    return rc;
}

static void errputs(inkpot_t *inkpot, const char *s)
{
    inkpot->disc.err_writer(inkpot->err_closure, s, strlen(s));
}

inkpot_status_t inkpot_debug_schemes( inkpot_t *inkpot )
{
    IDX_SCHEMES_NAME i;
    IDX_SCHEMES_INDEX j;
    int found;

    errputs(inkpot, "schemes:\n");
    for (i = 0; i < SZT_SCHEMES_NAME; i++) {
	found = 0;
        if ((1<<i) & inkpot->scheme_bits) {
            errputs(inkpot, " ");
            errputs(inkpot, &TAB_STRINGS[TAB_SCHEMES_NAME[i].string_idx]);
            errputs(inkpot, " (in)");
	    found++;
        }
        if ((1<<i) & inkpot->out_scheme_bit) {
	    if (! found) {
                errputs(inkpot, " ");
                errputs(inkpot, &TAB_STRINGS[TAB_SCHEMES_NAME[i].string_idx]);
	    }
            errputs(inkpot, " (out)");
        }
	if (found) 
            errputs(inkpot, " (names)\n");
    }
    for (j = 0; j < inkpot->active_schemes; j++) {
        errputs(inkpot, " ");
        errputs(inkpot, &TAB_STRINGS[TAB_SCHEMES_INDEX[inkpot->scheme_list[j]].string_idx]);
        errputs(inkpot, " (in) (index)\n");
    }
    for (j = 0; j < inkpot->active_out_schemes; j++) {
        errputs(inkpot, " ");
        errputs(inkpot, &TAB_STRINGS[TAB_SCHEMES_INDEX[inkpot->out_scheme_list[j]].string_idx]);
        errputs(inkpot, " (out) (index)\n");
    }
    errputs(inkpot, "\n");

    return ((inkpot->status = INKPOT_SUCCESS));
}

static inkpot_status_t inkpot_debug_scheme_names( inkpot_t *inkpot, int scheme_bits )
{
    IDX_SCHEMES_NAME i;
    int found = 0;

    errputs(inkpot, "(");
    for (i = 0; i < SZT_SCHEMES_NAME; i++) {
        if ((1 << i) & scheme_bits) {
            if (found++)
                errputs(inkpot, " ");
            errputs(inkpot, &TAB_STRINGS[TAB_SCHEMES_NAME[i].string_idx]);
        }
    }
    errputs(inkpot, ")");

    return INKPOT_SUCCESS;
}

static void inkpot_debug_rgba( inkpot_t *inkpot, VALUE value )
{
    char buf[22];
    unsigned short rgba[4];
    int i;

    for (i = 3; i >= 0; i--) {
	rgba[i] = (value & MSK_RED);
	value >>= SZB_RED;
    }
    sprintf(buf, "#%04x%04x%04x%04x", rgba[0], rgba[1], rgba[2], rgba[3]);
    errputs(inkpot, buf);
}

static inkpot_status_t inkpot_debug_names_schemes( inkpot_t *inkpot, MSK_SCHEMES_NAME scheme_bits, IDX_SCHEMES_INDEX active, IDX_SCHEMES_NAME *scheme_index )
{
    inkpot_name_t *name;
    IDX_NAMES i;
#if 0
    IDX_SCHEMES_INDEX j;
    IDX_IXVALUES k, first, last;
    IDX_VALUES v;
    char buf[20];
#endif

    if (scheme_bits) {
        for (i = 0; i < SZT_NAMES; i++) {
            name = &TAB_NAMES[i];
            if (scheme_bits & name->scheme_bits) {
                errputs(inkpot, &TAB_STRINGS[TAB_NAMES[i].string_idx]);
		inkpot_debug_scheme_names(inkpot, scheme_bits);
		errputs(inkpot, " ");
		inkpot_debug_rgba(inkpot, TAB_VALUES[name->value_idx].value);
		errputs(inkpot, "\n");
            }
        }
    }
#if 0
    for (j=0; j < active; j++) {
	first = scheme_index->first_value_idx;
	j = scheme_index - TAB_SCHEMES_INDEX;
	if (++j >= SZT_SCHEMES_INDEX)
	    last = SZT_IXVALUES;
	else
	    last = TAB_SCHEMES_INDEX[j].first_value_idx;
    
	for (k = first; k < last; k++) {
	    v = TAB_IXVALUES[k];
	    sprintf(buf, "%d(", k - first);
	    errputs(inkpot, buf);
	    errputs(inkpot, &TAB_STRINGS[scheme_index->string_idx]);
	    errputs(inkpot, ") ");
	    if (v < SZT_VALUES)
	        inkpot_debug_rgba(inkpot, TAB_VALUES[v].value);
	    else
	        inkpot_debug_rgba(inkpot, TAB_NONAME_VALUES[v - SZT_VALUES].value);
	    errputs(inkpot, "\n");
	}
    }
#endif
    errputs(inkpot, "\n");

    return ((inkpot->status = INKPOT_SUCCESS));
}

inkpot_status_t inkpot_debug_names( inkpot_t *inkpot )
{
    errputs(inkpot, "names (in):\n");
    return inkpot_debug_names_schemes(inkpot, inkpot->scheme_bits, inkpot->active_schemes, inkpot->scheme_list);
}

inkpot_status_t inkpot_debug_out_names( inkpot_t *inkpot )
{
    errputs(inkpot, "names (out):\n");
    return inkpot_debug_names_schemes(inkpot, inkpot->out_scheme_bit, 1, inkpot->out_scheme_list);
}

#if 0
/* Print all values that are members of the currently listed
 * name schemes, with the names in those schemes.
 * Does not print the indexes in index schemes that a value may
 * be a member of. */
inkpot_status_t inkpot_debug_values( inkpot_t *inkpot )
{
    inkpot_value_t *value;
    inkpot_name_t *name;
    IDX_VALUES i;
    IDX_NAMES t;
    MSK_SCHEMES_NAME scheme_bits;
    int found;

    errputs(inkpot, "values:\n");
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
                    errputs(inkpot, " ");
                else
		    inkpot_debug_rgba(inkpot, TAB_VALUES[i].value);
                errputs(inkpot, " ");
                errputs(inkpot, &TAB_STRINGS[name->string_idx]);
		inkpot_debug_scheme_names(inkpot, scheme_bits);
            }
        }
        if (found)
            errputs(inkpot, "\n");
    }
    errputs(inkpot, "\n");

    return ((inkpot->status = INKPOT_SUCCESS));
}
#endif

inkpot_status_t inkpot_debug_error ( inkpot_t *inkpot )
{
    const char *m;

    assert(inkpot);
    switch (inkpot->status) {
	case INKPOT_SUCCESS:
	    m = "\nINKPOT_SUCCESS\n"; break;
	case INKPOT_MALLOC_FAIL:
	    m = "\nINKPOT_MALLOC_FAIL\n"; break;
	case INKPOT_COLOR_UNKNOWN:
	    m = "\nINKPOT_COLOR_UNKNOWN\n"; break;
	case INKPOT_COLOR_NONAME:
	    m = "\nINKPOT_COLOR_NONAME\n"; break;
	case INKPOT_COLOR_NOPALETTE:
	    m = "\nINKPOT_COLOR_PALETTE\n"; break;
	case INKPOT_SCHEME_UNKNOWN:
	    m = "\nINKPOT_SCHEME_UNKNOWN\n"; break;
    }
    inkpot->disc.err_writer(inkpot->err_closure, m, strlen(m));

    return ((inkpot->status = INKPOT_SUCCESS));
};
