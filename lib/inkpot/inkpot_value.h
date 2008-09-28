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

#ifndef INKPOT_VALUE_H
#define INKPOT_VALUE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct inkpot_value_s {	/* Numeric color values used by the set
				 * of inkpot_name_t and indexes from
				 * indexed color schemes ( total presumed
				 * to be less than all possible color values).
				 * inkpot_value_t instances are unique and
				 * are numerically sorted by rgb value
				 * in TAB_VALUES[] */

	VALUE value;

        IDX_NAMES  /* FIXME - searate this in to a separate table */
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
	VALUE value;

} inkpot_noname_value_t;

extern inkpot_status_t inkpot_set_value ( inkpot_t *inkpot, VALUE value );
extern inkpot_status_t inkpot_get_value ( inkpot_t *inkpot, VALUE *value );

extern inkpot_value_t TAB_VALUES[];
extern inkpot_noname_value_t TAB_NONAME_VALUES[];
extern IDX_VALUES TAB_IXVALUES[];

#ifdef __cplusplus
}
#endif
#endif /* INKPOT_VALUE_H */

