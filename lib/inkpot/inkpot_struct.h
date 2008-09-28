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

#ifndef INKPOT_STRUCTS_H
#define INKPOT_STRUCTS_H

typedef struct inkpot_scheme_name_s {
	IDX_STRINGS
	     string_idx;        /* The scheme name in TAB_STRINGS.  (e.g. "x11") */
} inkpot_scheme_name_t;

typedef struct inkpot_scheme_index_s { 
	IDX_STRINGS
	     string_idx;        /* The indexed scheme name in TAB_STRINGS. 
				 * e.g "blues7" from Brewer */
	IDX_IXVALUES
	    first_value_idx;	/* The first index to the color values in
				 * TAB_IXVALUES. Terminated by the
				 * first_value_idx of the next
				 * inkpot_scheme_index_t in TAB_SCHEMES_INDEX
				 * or by SZT_SCHEMES_INDEX */
} inkpot_scheme_index_t;

typedef struct inkpot_name_s {	/* Color_name used by one or more
				 * inkpot_scheme_name_t. Each instance has
				 * just one color string and one index_value_t
				 * but another instance may use the same
				 * color string for a different inkpot_value_t.
				 * inkpot_names_t are alpha sorted in
				 * TAB_NAMES[] */

	IDX_STRINGS
	    string_idx;         /* The color name in TAB_STRINGS.
				 * (e.g. "green") */

	IDX_VALUES
	    value_idx;		/* An index into TAB_VALUE for this
				 * inkpot_name_t */

	MSK_SCHEMES_NAME
	    scheme_bits;	/* A bit for each inkpot_scheme_name_t
				 * that includes this inkpot_name_t  */

	IDX_NAMES
	    toname_idx;		/* This is the element of TAB_TONAME that
				 * indexes an element in TAB_NAME.
				 * TAB_TONAME is a logically separate table
				 * from TAB_NAMES, but it happens to be
				 * exactly the same SZT, so we are trying to
				 * get better packing by this merging */
} inkpot_name_t;

typedef struct inkpot_cache_element_s {
	IDX_MRU_CACHE
	    next_recently_used_idx;
	MSK_SCHEMES_NAME
	    scheme_bits;
	IDX_NAMES
	    name_idx;
} inkpot_cache_element_t;

/* typedef struct inkpot_s inkpot_t; */  /* public opaque type in inkpot.h */

struct inkpot_s {		/* The Ink Pot */
	MSK_SCHEMES_NAME
	    scheme_bits,	/* One bit per inkpot_scheme_name_t */
	    out_scheme_bit;     /* One scheme only for output. */

	IDX_VALUES
	    value_idx;		/* The current color */

	IDX_IXVALUES
	    index,		/* The index for the current value in active_schemes_index */
	    out_index;		/* The index for the current value in active_out_schemes_index */

	IDX_SCHEMES_INDEX
	    active_schemes,                 /* The number of active index schemes. */
	    scheme_list[SZT_SCHEMES_INDEX], /* The list of active index schemes. */
	    active_out_schemes,             /* The number of active index schemes. */
	    out_scheme_list[1];             /* The list of active index schemes. */

	inkpot_name_t
	    *out_name;		/* The current output name, or NULL. */

	char *canon;		/* malloc'ed, reused storage for canonicalizing color request strings */
	int canon_alloc;
	
	VALUE
	    no_palette_value;

	inkpot_noname_value_t *palette;   /* malloc'ed paletter storage */
	int palette_alloc;
	int palette_fill;

	inkpot_cache_element_t cache[SZT_MRU_CACHE];  /* MRU chache of successfull color lookups */
	IDX_MRU_CACHE
	    most_recently_used_idx;

	inkpot_disc_t disc;     /* writers and closures for out and err */
	void *out_closure, *err_closure;

	inkpot_status_t status; /* The status after the last operation */
};

#endif /* INKPOT_STRUCTS_H */
