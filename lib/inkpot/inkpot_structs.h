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

#ifndef INKPOT_STRUCTS_H
#define INKPOT_STRUCTS_H

typedef struct inkpot_scheme_name_s {
	IDX_STRINGS
	     string_idx;        /* The scheme name in TAB_STRINGS.  (e.g. "x11") */

	IDX_NAMES
	    default_name_idx;	/* The index of the name of the default color
				 * for this scheme. (Each scheme can have
				 * a different default.  The default must
				 * be a member of the scheme */
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
				
				/* (The default value for indexed schemes is
				 * always the color indexed by "0" */
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

	BIT_SCHEMES_NAME
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

typedef struct inkpot_value_s {	/* Numeric color values used by the set
				 * of inkpot_name_t and indexes from
				 * indexed color schemes ( total presumed
				 * to be less than all possible color values).
				 * inkpot_value_t instances are unique and
				 * are numerically sorted by rgb value
				 * in TAB_VALUES[] */

	unsigned char rgba[4];	/* rgba */

	IDX_NAMES
	    toname_idx;		/* An index into TAB_NAMES to the toname
				 * which indexes the first inkpot_name_t
				 * in TAB_NAMES that maps to this color,
				 * followed in TAB_NAMES by the toname indexes
				 * of other inkpot_name_t that also map to
				 * this inkpot_value_t, until a toname index
				 * in TAB_NAMES indexes a inkpot_name_t
				 * that doesn't map to this inkpot_value_t,
				 * or until the end of TAB_NAMES is reached.
				 * (Phew!) */
} inkpot_value_t;

typedef struct inkpot_noname_value_s {	/* Numeric color values used by the remainder
				 * of indexes from indexed color schemes
				 * inkpot_noname_value_t instances are unique and
				 * are numerically sorted by rgba value
				 * in TAB_NONAME_VALUES[] */

	unsigned char rgba[4];	/* rgba */
} inkpot_noname_value_t;

/* typedef struct inkpot_s inkpot_t; */  /* public opaque type in inkpot.h */

struct inkpot_s {		/* The Ink Pot */
	BIT_SCHEMES_NAME
	    scheme_bits,	/* One bit per inkpot_scheme_name_t */
	    out_scheme_bit;     /* One scheme only for output. */

	IDX_SCHEMES_NAME
	    default_scheme_name_idx; /* The index of the scheme which provides
				 * the default color. It was the first
				 * scheme added. */

	IDX_VALUES
	    default_value_idx, 	/* The default color */
	    value_idx;          /* The current color value. */

	IDX_IXVALUES
	    index,		/* The index for the current value, if indexed scheme not NULL */
	    out_index;		/* The index for the current value, if indexed output scheme not NULL */

	inkpot_scheme_index_t
	    *scheme_index,	/* Indexed input scheme, or NULL. */
	    *out_scheme_index;	/* Indexed output scheme, or NULL */

	inkpot_name_t
	    *name,		/* The current input name, or NULL. */
	    *out_name;		/* The current output name, or NULL. */

	inkpot_disc_t disc;     /* writers and closures for out and err */
	void *out_closure, *err_closure;

	inkpot_status_t status; /* The status after the last operation */
};

#endif /* INKPOT_STRUCTS_H */
