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

#ifndef INKPOT_STRUCTS_H
#define INKPOT_STRUCTS_H

typedef unsigned char IDX_MRU_CACHE_t;
#define SZT_MRU_CACHE 4

typedef struct inkpot_scheme_s {
    IDX_STRINGS_t
	string_idx;        /* The scheme name in TAB_STRINGS.  (e.g. "x11") */
} inkpot_scheme_t;

typedef struct inkpot_range_s {
    IDX_INDEXES_t
	size;		/* 1,0 for alts,  all but last alt=1, last alt is 0,
			 * n>=2 for ranges
			 */
    MSK_SCHEMES_t
	scheme_bits;	/* A bit for each inkpot_scheme_name_t
			 * that includes this inkpot_name_t
			 */
    IDX_INDEXES_t
	first_value_idx; /* index of first in range, or first alternative */
} inkpot_range_t;

typedef struct inkpot_name_s {    /* Color_name used by one or more
			 * inkpot_scheme_name_t. Each instance has
			 * just one color string and one index_value_t
			 * but another instance may use the same
			 * color string for a different inkpot_value_t.
			 * inkpot_names_t are alpha sorted in
			 * TAB_NAMES[] */

    IDX_STRINGS_t
	string_idx;         /* The color name in TAB_STRINGS.
			 * (e.g. "green") */

    MSK_SCHEMES_t
	scheme_bits;	/* A bit for each inkpot_scheme_name_t
			 * that includes this inkpot_name_t.
			  if = 0, then name has multiple values  */

    IDX_VALUES_t
	value_idx;	/* An index into TAB_VALUE for this
			 * inkpot_name_t, or, if scheme_bits == 0
			 * an index into TAB_RANGES */
} inkpot_name_t;

typedef struct inkpot_cache_element_s {
    IDX_MRU_CACHE_t
	next_recently_used_idx;
    MSK_SCHEMES_t
	scheme_bits;
    IDX_NAMES_t
	name_idx;
} inkpot_cache_element_t;

/* typedef struct inkpot_s inkpot_t; */  /* public opaque type in inkpot.h */

struct inkpot_s {    	/* The Ink Pot */
    MSK_SCHEMES_t
	scheme_bits,	/* One bit per inkpot_scheme_name_t */
	out_scheme_bit;     /* One scheme only for output. */

    IDX_INDEXES_t
	index,		/* The index for the current value in active_schemes_index */
	out_index;		/* The index for the current value in active_out_schemes_index */

    int
	active_schemes,     /* The number of active index schemes. */
	active_out_schemes; /* The number of active index schemes. */
	
    inkpot_name_t
	*out_name;		/* The current output name, or NULL. */

    char *canon;		/* malloc'ed, reused storage for canonicalizing color request strings */
    int canon_alloc;
    
    inkpot_values_t values; /* The values store */
    inkpot_value_t value;	/* The current value of the inkpot */

    inkpot_cache_element_t
	cache[SZT_MRU_CACHE];/* MRU chache of successful color lookups */
    IDX_MRU_CACHE_t
	most_recently_used_idx;

    inkpot_write_disc_t
	write_disc;		/* User changeable function for writing output
			 *    Defaults to an internal function that writes
			 *    to FILE* streams */
    void
	*write_closure;	/* A parameter to pass to the writer function.
			 *    Defaults to (FILE*)stdout */

    inkpot_status_t
	status;		/* The status after the last operation */
};

#endif /* INKPOT_STRUCTS_H */
