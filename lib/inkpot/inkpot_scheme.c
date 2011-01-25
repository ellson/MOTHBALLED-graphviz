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
#include <ctype.h>
#include <assert.h>

#include "inkpot.h"

#include "inkpot_lib_define.h"
#include "inkpot_value.h"
#include "inkpot_scheme.h"
#include "inkpot_xlate.h"
#include "inkpot_lib_scheme.h"
#include "inkpot_lib_string.h"

static size_t inkpot_writer (void *closure, const char *data, size_t length)
{
    return fwrite(data, sizeof(char), length, (FILE *)closure);
}

static inkpot_write_disc_t inkpot_default_writer = { inkpot_writer };

inkpot_t *inkpot_init ( void )
{
    inkpot_t *inkpot = malloc(sizeof(inkpot_t));
    if (inkpot) {
	memset(inkpot, 0, sizeof(inkpot_t));
	inkpot->write_disc = inkpot_default_writer;
	inkpot->write_closure = stdout;
    }
    return inkpot;
}

void inkpot_destroy ( inkpot_t *inkpot )
{
    free(inkpot->canon);
    free(inkpot);
}

inkpot_status_t inkpot_write_disc ( inkpot_t *inkpot, inkpot_write_disc_t disc )
{
    inkpot->write_disc = disc;
    return ((inkpot->status = INKPOT_SUCCESS));
}

inkpot_status_t inkpot_write_closure ( inkpot_t *inkpot, void *closure )
{
    inkpot->write_closure = closure;
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

static int inkpot_scheme_cmpf ( const void *key, const void *base)
{
    const char *k = (const char*)key;
    const char *b = &TAB_STRINGS[((inkpot_scheme_t *)base)->string_idx];

    return string_cmpf(k, b);
}

static inkpot_scheme_t *inkpot_find_scheme ( const char *scheme )
{
    if (scheme == NULL)
        return NULL;

    return (inkpot_scheme_t *) bsearch(
            (void*)scheme, (void*)TAB_SCHEMES,
            SZT_SCHEMES, sizeof(inkpot_scheme_t),
            inkpot_scheme_cmpf); 
}

static int inkpot_icolor_cmpf ( const void *key, const void *base)
{
    const char *k = (const char*)key;
    const char *b = &TAB_STRINGS[((inkpot_icolor_t *)base)->string_idx];

    return string_cmpf(k, b);
}

static inkpot_icolor_t *inkpot_find_icolor ( const char *icolor)
{
    if (icolor == NULL)
        return NULL;

    return (inkpot_icolor_t *) bsearch(
            (void*)icolor, (void*)TAB_ICOLORS,
            SZT_ICOLORS, sizeof(inkpot_icolor_t),
            inkpot_icolor_cmpf); 
}

static inkpot_status_t inkpot_scheme ( inkpot_t *inkpot, const char *scheme )
{
    inkpot_scheme_t *inkpot_scheme;

    inkpot_scheme = inkpot_find_scheme(scheme);
    if (inkpot_scheme == NULL)
        return ((inkpot->status = INKPOT_SCHEME_UNKNOWN));

    inkpot->scheme_bits |= 1 << (inkpot_scheme - TAB_SCHEMES);
    return ((inkpot->status = INKPOT_SUCCESS));
}

#if 0
    IDX_ICOLORS_t i;
    inkpot_icolor_t *inkpot_icolor;
    inkpot_icolor = inkpot_find_icolor(icolor);
    if (! inkpot_icolor)
        return ((inkpot->status = INKPOT_SCHEME_UNKNOWN));
    
    for (i = 0; i < inkpot->active_schemes; i++)
        if (inkpot->scheme_list[i] == (inkpot_icolor - TAB_ICOLORS))
	    return ((inkpot->status = INKPOT_SUCCESS));

    inkpot->scheme_list[i] = (inkpot_icolor - TAB_ICOLORS);
    inkpot->active_schemes++;
    return ((inkpot->status = INKPOT_SUCCESS));
        return ((inkpot->status = INKPOT_SCHEME_UNKNOWN));
}
#endif
 
inkpot_status_t inkpot_schemes_put ( inkpot_t *inkpot, const char *schemes )
{
    inkpot_status_t rc = INKPOT_SUCCESS;
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
 
inkpot_status_t inkpot_scheme_get ( inkpot_t *inkpot, const char *scheme )
{
    inkpot_scheme_t *inkpot_scheme;
    IDX_SCHEMES_t idx;

    inkpot_scheme = inkpot_find_scheme(scheme);
    if (inkpot_scheme == NULL)
	return ((inkpot->status = INKPOT_SCHEME_UNKNOWN));

    idx = inkpot_scheme - TAB_SCHEMES;
    inkpot->out_scheme_bit = 1 << idx;

    return ((inkpot->status = INKPOT_SUCCESS));
}


inkpot_status_t inkpot_icolor_get ( inkpot_t *inkpot, const char *icolor )
{
    inkpot_icolor_t *inkpot_icolor;
    inkpot_icolor = inkpot_find_icolor(icolor);   /* FIXME */
    if (! inkpot_icolor)
        return ((inkpot->status = INKPOT_SCHEME_UNKNOWN));

    if (inkpot->active_out_schemes == 0 
	    || (inkpot->out_scheme_list[0] != (inkpot_icolor - TAB_ICOLORS))) {
	inkpot->out_scheme_list[0] = (inkpot_icolor - TAB_ICOLORS);
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

static inkpot_status_t inkpot_put_value_idx( inkpot_t *inkpot, IDX_VALUES_t value_idx)
{
    if (inkpot->value.index != value_idx) {
	inkpot->value.index = value_idx;
	inkpot->out_name = NULL;  /* value changed so invalidate out_name */
    }
    return ((inkpot->status = INKPOT_SUCCESS));
}

static inkpot_status_t inkpot_cache_get( inkpot_t *inkpot )
{
    IDX_MRU_CACHE_t i;
    IDX_NAMES_t cache_name_idx;
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
        return inkpot_put_value_idx(inkpot, TAB_NAMES[cache_name_idx].value_idx);
    }
    return ((inkpot->status = INKPOT_COLOR_UNKNOWN));
}

static inkpot_status_t inkpot_cache_put ( inkpot_t *inkpot, IDX_NAMES_t name_idx )
{
    IDX_MRU_CACHE_t i;

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

static inkpot_status_t inkpot_put_name ( inkpot_t *inkpot )
{
    inkpot_status_t rc;
    char *color;
    inkpot_name_t *name;
    IDX_ALTS_t i;

    if (inkpot == NULL || ! inkpot->scheme_bits)
        return ((inkpot->status = INKPOT_SCHEME_UNKNOWN));
    if ((color = inkpot->canon) == NULL)
        return ((inkpot->status = INKPOT_COLOR_UNKNOWN));

    rc = inkpot_cache_get(inkpot);
    if (rc == INKPOT_SUCCESS)
	return rc;

    name = (inkpot_name_t *) bsearch(
        (void*)color, (void*)TAB_NAMES,
	SZT_NAMES, sizeof(inkpot_name_t),
	inkpot_name_cmpf); 
    if (name == NULL) 
        return ((inkpot->status = INKPOT_COLOR_UNKNOWN));
	
    if (name->scheme_bits) {
        if (inkpot->scheme_bits & name->scheme_bits) {
	    rc = inkpot_put_value_idx(inkpot, name->value_idx);
	    assert(rc == INKPOT_SUCCESS);
	    return inkpot_cache_put(inkpot, (name - TAB_NAMES) );
	}
    }
    else {
	for (i = name->value_idx; i < SZT_ALTS; i++) {
	    if (inkpot->scheme_bits & TAB_ALTS[i].scheme_bits) {
		rc = inkpot_put_value_idx(inkpot, TAB_ALTS[i].value_idx);
		assert(rc == INKPOT_SUCCESS);
		return inkpot_cache_put(inkpot, (name - TAB_NAMES) );
	    }
	    if (! TAB_ALTS[i].string_idx)
		break;
	}
    }
    return ((inkpot->status = INKPOT_COLOR_UNKNOWN));
}

static inkpot_status_t inkpot_put_index ( inkpot_t *inkpot, int index )
{
    IDX_ICOLORS_t j;
    IDX_INDEXES_t first, last;
    IDX_VALUES_t value_idx;

    if (!inkpot->active_schemes)
        return ((inkpot->status = INKPOT_SCHEME_UNKNOWN));

    j = inkpot->scheme_list[0];
    first = TAB_ICOLORS[j].range_idx;
    if (++j >= SZT_ICOLORS)
	last = SZT_INDEXES;
    else
	last = TAB_ICOLORS[j].range_idx;
    last = last-1-first;

    /* clip user-provided index to fit available range */
    index = (index < 0)    ? 0    : index;
    index = (index > last) ? last : index;
    index += first;

    assert(index < SZT_INDEXES);
    value_idx = TAB_INDEXES[index];
    if (value_idx >= SZT_VALUES)
        assert(value_idx < SZT_VALUES + SZT_NONAME_VALUES);

    return inkpot_put_value_idx(inkpot, value_idx);
}

inkpot_status_t inkpot_put_rgba ( inkpot_t *inkpot, double rgba[4] )
{
    inkpot_status_t rc;
    inkpot_value_t value;
    double v;
    int i;

    for (i = 0; i < 4; i++) {
	v = rgba[i];
	v = (v < 0.0) ? 0.0 : v;
	v = (v > 1.0) ? 1.0 : v;
	value.value[i] = (unsigned short)(v * 65535);
    }
    value.vtype = VTYPE_rgba;
    
    rc = inkpot_value_set ( &inkpot->values, &value );
    if (rc == INKPOT_SUCCESS)
	inkpot->value = value;
    return (inkpot->status = rc);
}

inkpot_status_t inkpot_put_hsva ( inkpot_t *inkpot, double hsva[4] )
{
    double rgba[4];

    hsva2rgba( hsva, rgba );

    return inkpot_put_rgba ( inkpot, rgba );
}

inkpot_status_t inkpot_put ( inkpot_t *inkpot, const char *color )
{
    inkpot_status_t rc = INKPOT_COLOR_UNKNOWN;
    IDX_ICOLORS_t j;
    IDX_INDEXES_t first, last;
    IDX_VALUES_t value_idx;
    char *q, *s;
    const char *p;
    int i, c, len, index;
    unsigned int r, g, b, a;
    inkpot_value_t value;
    double hsva[4];

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
	    value.value[0] = r;
	    value.value[1] = g;
	    value.value[2] = b;
	    value.value[3] = a;
	}
	else if (len < 3) {
	    a = 255;
	    if ((len = sscanf(inkpot->canon, "#%2x%2x%2x%2x", &r, &g, &b, &a)) >= 3) {
	        value.value[0] = r | r << 8;
	        value.value[1] = g | r << 8;
	        value.value[2] = b | r << 8;
	        value.value[3] = a | r << 8;
   	    } 
	}
	if (len >= 3) {
	    value.vtype = VTYPE_rgba;
	    rc = inkpot_value_set ( &inkpot->values, &value );
	    if (rc ==  INKPOT_SUCCESS)
	        inkpot->value = value;
	    inkpot->status = rc;
	}
    }
    else if (((c = *inkpot->canon) == '.') || isdigit(c)) {
	hsva[3] = 1.0;
        if (sscanf(inkpot->canon, "%lf%lf%lf%lf", &hsva[0], &hsva[1], &hsva[2], &hsva[3]) >= 3)
	    rc = inkpot_put_hsva(inkpot, hsva);  /* FIXME */
	else 
            if (sscanf(inkpot->canon, "%d", &index) == 1)   /* simple indexes */
                rc = inkpot_put_index(inkpot, index);
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
	    p = &TAB_STRINGS[TAB_ICOLORS[j].string_idx];
	    if (*p != *s || strcmp(p, s) != 0) 
		continue;
	    /* FIXME - deal with subschemes */
	    first = TAB_ICOLORS[j].range_idx;
            if (++j >= SZT_ICOLORS)
	        last = SZT_INDEXES;
	    else
		last = TAB_ICOLORS[j].range_idx;
	    last = last-1-first;

	    /* clip user-provided index to fit available range */
	    index = (index < 0)    ? 0    : index;
	    index = (index > last) ? last : index;
	    index += first;

	    assert(index < SZT_INDEXES);
	    value_idx = TAB_INDEXES[index];
	    if (value_idx >= SZT_VALUES)
		assert(value_idx < SZT_VALUES + SZT_NONAME_VALUES);

	    rc = inkpot_put_value_idx(inkpot, value_idx);
	    break;
	}
	*q = c;  /* repair the damage to inkpot->canon */
    }

    if (rc != INKPOT_SUCCESS) {
	/* remove embedded ws and convert to lower case*/
        for (p = q = inkpot->canon; (c = *p) && ! ( c == ' ' || c == '\t' ); p++) { *q++ = tolower(c); };
	*q = '\0';
        rc = inkpot_put_name(inkpot);
    }

    return rc;
}

inkpot_status_t inkpot_get ( inkpot_t *inkpot, const char **color )
{
    inkpot_name_t *out_name;
    IDX_NAMES_t t;
    IDX_ALTS_t i;
    IDX_VALUES_t value_idx;
    int maybe;

    /* FIXME - why isn't this checking the current value then the 4 level cache? */
    out_name = inkpot->out_name;
    if (out_name) {  /* if we have a cached name */
    	*color = &TAB_STRINGS[out_name->string_idx];
        return ((inkpot->status = INKPOT_SUCCESS));
    }
    

    /* FIXME - most of this should go */

    if (inkpot->out_scheme_bit) {
        value_idx = inkpot->value.index;
        if (value_idx < SZT_VALUES) {
            for (t = TAB_VALUE_TO[value_idx]; t < SZT_TO_NAMES; t++) {
                out_name = &TAB_NAMES[TAB_TO_NAMES[t]];
		if (out_name->scheme_bits) {
                    if (out_name->value_idx != value_idx)
                        break;
                    if (out_name->scheme_bits & inkpot->out_scheme_bit) {
	                inkpot->out_name = out_name;
                        *color = &TAB_STRINGS[out_name->string_idx];
		        return ((inkpot->status = INKPOT_SUCCESS));
		    }
                }
		else {
		    maybe = 0;
		    for (i = out_name->value_idx; i < SZT_ALTS; i++) {
			if ( TAB_ALTS[i].value_idx == value_idx) {
			    maybe++;
			    if (inkpot->out_scheme_bit & TAB_ALTS[i].scheme_bits) {
			        inkpot->out_name = out_name;
                                *color = &TAB_STRINGS[out_name->string_idx];
		                return ((inkpot->status = INKPOT_SUCCESS));
			    }
			}
			if (! TAB_ALTS[i].string_idx)
			    break;
		    }
		    if (!maybe)
		        break;
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
	    return ((inkpot->status = INKPOT_NOPALETTE));
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
    rc = inkpot->status = inkpot_value_get( &inkpot->values, &value );
    if (rc == INKPOT_SUCCESS) {
	for (i = 0; i < 4; i++) {
	    rgba[i] = value.value[i];
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
    rc = inkpot->status = inkpot_value_get( &inkpot->values, &value );
    if (rc == INKPOT_SUCCESS) {
	for (i = 0; i < 4; i++) {
	    rgba[i] = value.value[i] / (double)65535;
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

static void inkpot_puts(inkpot_t *inkpot, const char *s)
{
    inkpot->write_disc.writer(inkpot->write_closure, s, strlen(s));
}

inkpot_status_t inkpot_write ( inkpot_t *inkpot )
{
    inkpot_status_t rc;
    const char *color;

    rc = inkpot_get(inkpot, &color);
    if (rc == INKPOT_SUCCESS)
	inkpot->write_disc.writer(inkpot->write_closure, color, strlen(color));
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
        inkpot->write_disc.writer(inkpot->write_closure, buf, len);
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
        inkpot->write_disc.writer(inkpot->write_closure, buf, len);
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
        inkpot->write_disc.writer(inkpot->write_closure, buf, len);
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
        inkpot->write_disc.writer(inkpot->write_closure, buf, len);
    }
    return rc;
}

inkpot_status_t inkpot_debug_schemes( inkpot_t *inkpot )
{
    IDX_SCHEMES_t i;
    IDX_ICOLORS_t j;
    int found;

    inkpot_puts(inkpot, "schemes:\n");
    for (i = 0; i < SZT_SCHEMES; i++) {
	found = 0;
        if ((1<<i) & inkpot->scheme_bits) {
            inkpot_puts(inkpot, " ");
            inkpot_puts(inkpot, &TAB_STRINGS[TAB_SCHEMES[i].string_idx]);
            inkpot_puts(inkpot, "(in)");
	    found++;
        }
        if ((1<<i) & inkpot->out_scheme_bit) {
	    if (! found) {
                inkpot_puts(inkpot, " ");
                inkpot_puts(inkpot, &TAB_STRINGS[TAB_SCHEMES[i].string_idx]);
	        found++;
	    }
            inkpot_puts(inkpot, "(out)");
        }
	if (found) 
    	    inkpot_puts(inkpot, "\n");
    }
    for (j = 0; j < inkpot->active_schemes; j++) {
        inkpot_puts(inkpot, " ");
        inkpot_puts(inkpot, &TAB_STRINGS[TAB_ICOLORS[inkpot->scheme_list[j]].string_idx]);
        inkpot_puts(inkpot, "#(in)");
    }
    for (j = 0; j < inkpot->active_out_schemes; j++) {
        inkpot_puts(inkpot, " ");
        inkpot_puts(inkpot, &TAB_STRINGS[TAB_ICOLORS[inkpot->out_scheme_list[j]].string_idx]);
        inkpot_puts(inkpot, "#(out)");
    }
    inkpot_puts(inkpot, "\n");

    return ((inkpot->status = INKPOT_SUCCESS));
}

#if 0
static inkpot_status_t inkpot_debug_scheme_names( inkpot_t *inkpot, int scheme_bits )
{
    IDX_SCHEMES_NAME_t i;
    int found = 0;

    inkpot_puts(inkpot, "(");
    for (i = 0; i < SZT_SCHEMES_NAME; i++) {
        if ((1 << i) & scheme_bits) {
            if (found++)
                inkpot_puts(inkpot, " ");
            inkpot_puts(inkpot, &TAB_STRINGS[TAB_SCHEMES_NAME[i].string_idx]);
        }
    }
    inkpot_puts(inkpot, ")");

    return INKPOT_SUCCESS;
}

static void inkpot_debug_rgba( inkpot_t *inkpot, VALUE value )
{
    char buf[22];
    unsigned short rgba[4];
    int i;

    for (i = 3; i >= 0; i--) {
	rgba[i] = (value & 65535);
	value >>= 16;
    }
    sprintf(buf, "#%04x%04x%04x%04x", rgba[0], rgba[1], rgba[2], rgba[3]);
    inkpot_puts(inkpot, buf);
}
#endif

#if 0
static inkpot_status_t inkpot_debug_names_schemes( inkpot_t *inkpot, MSK_SCHEMES_t scheme_bits, IDX_ICOLORS_t active, IDX_SCHEMES_NAME_t *scheme_index )
{
    inkpot_name_t *name;
    IDX_NAMES_t i;
#if 0
    IDX_ICOLORS_t j;
    IDX_INDEXES_t k, first, last;
    IDX_VALUES_t v;
    char buf[20];
#endif

    if (scheme_bits) {
        for (i = 0; i < SZT_NAMES; i++) {
            name = &TAB_NAMES[i];
            if (scheme_bits & name->scheme_bits) {
                inkpot_puts(inkpot, &TAB_STRINGS[TAB_NAMES[i].string_idx]);
		inkpot_debug_scheme_names(inkpot, scheme_bits);
		inkpot_puts(inkpot, " ");
		inkpot_debug_rgba(inkpot, TAB_VALUES[name->value_idx].value);
		inkpot_puts(inkpot, "\n");
            }
        }
    }
#if 0
    for (j=0; j < active; j++) {
	first = scheme_index->range_idx;
	j = scheme_index - TAB_ICOLORS;
	if (++j >= SZT_ICOLORS)
	    last = SZT_INDEXES;
	else
	    last = TAB_ICOLORS[j].range_idx;
    
	for (k = first; k < last; k++) {
	    v = TAB_INDEXES[k];
	    sprintf(buf, "%d(", k - first);
	    inkpot_puts(inkpot, buf);
	    inkpot_puts(inkpot, &TAB_STRINGS[scheme_index->string_idx]);
	    inkpot_puts(inkpot, ") ");
	    if (v < SZT_VALUES)
	        inkpot_debug_rgba(inkpot, TAB_VALUES[v].value);
	    else
	        inkpot_debug_rgba(inkpot, TAB_NONAME_VALUES[v - SZT_VALUES].value);
	    inkpot_puts(inkpot, "\n");
	}
    }
#endif
    inkpot_puts(inkpot, "\n");

    return ((inkpot->status = INKPOT_SUCCESS));
}

inkpot_status_t inkpot_debug_names( inkpot_t *inkpot )
{
    inkpot_puts(inkpot, "names (in):\n");
    return inkpot_debug_names_schemes(inkpot, inkpot->scheme_bits, inkpot->active_schemes, inkpot->scheme_list);
}

inkpot_status_t inkpot_debug_out_names( inkpot_t *inkpot )
{
    inkpot_puts(inkpot, "names (out):\n");
    return inkpot_debug_names_schemes(inkpot, inkpot->out_scheme_bit, 1, inkpot->out_scheme_list);
}
#endif

#if 0
/* Print all values that are members of the currently listed
 * name schemes, with the names in those schemes.
 * Does not print the indexes in index schemes that a value may
 * be a member of. */
inkpot_status_t inkpot_debug_values( inkpot_t *inkpot )
{
    inkpot_status_t rc;
    inkpot_value_t *value;
    inkpot_name_t *name;
    IDX_VALUES_t i;
    IDX_NAMES_t t;
    MSK_SCHEMES_t scheme_bits;
    int found;

    inkpot_puts(inkpot, "values:\n");
    for (rc = inkpot_value_get_first(inkpot, &value);
	    rc == INKPOT_SUCCESS
	    rc = inkpot_value_get_next(inkpot, &value)) {

        found = 0;
        for (t = value->toname_idx; t < SZT_NAMES; t++) {
            name = &TAB_NAMES[TAB_NAMES[t].toname_idx];
            if (name->value_idx != i)
                break;
            scheme_bits = name->scheme_bits & inkpot->scheme_bits;
            if (scheme_bits) {
                if (found++)
                    inkpot_puts(inkpot, " ");
                else
		    inkpot_debug_rgba(inkpot, TAB_VALUES[i].value);
                inkpot_puts(inkpot, " ");
                inkpot_puts(inkpot, &TAB_STRINGS[name->string_idx]);
		inkpot_debug_scheme_names(inkpot, scheme_bits);
            }
        }
        if (found)
            inkpot_puts(inkpot, "\n");
    }
    inkpot_puts(inkpot, "\n");

    return ((inkpot->status = INKPOT_SUCCESS));
}
#endif

inkpot_status_t inkpot_debug_error ( inkpot_t *inkpot )
{
    const char *m = "\nINKPOT_FAIL\n";

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
	case INKPOT_SCHEME_UNKNOWN:
	    m = "\nINKPOT_SCHEME_UNKNOWN\n"; break;
	case INKPOT_NOPALETTE:
	    m = "\nINKPOT_PALETTE\n"; break;
	case INKPOT_NOSUCH_INDEX:
	    m = "\nINKPOT_NOSUCH_INDEX\n"; break;
    }
    inkpot->write_disc.writer(inkpot->write_closure, m, strlen(m));

    return ((inkpot->status = INKPOT_SUCCESS));
};
