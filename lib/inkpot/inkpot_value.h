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

#if 1
/* FIXME - this stuff needs to go */

typedef struct inkpot_oldvalue_s {	/* Numeric color values used by the set
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
} inkpot_oldvalue_t;

typedef struct inkpot_noname_value_s {	/* Numeric color values used by the remainder
				 * of indexes from indexed color schemes
				 * inkpot_noname_value_t instances are unique and
				 * are numerically sorted by rgba value
				 * in TAB_NONAME_VALUES[] */
	VALUE value;

} inkpot_noname_value_t;

extern inkpot_oldvalue_t TAB_VALUES[];
extern inkpot_noname_value_t TAB_NONAME_VALUES[];
#endif
extern IDX_VALUES TAB_IXVALUES[];

/* proper api starts here */

typedef struct inkpot_values_s inkpot_values_t;

typedef int BIT_VTYPE;

#define MSK_VTYPE_size       0x01
#define BIT_VTYPE_size_8     0x00
#define BIT_VTYPE_size_16    0x01

#define MSK_VTYPE_code       0x0e
#define BIT_VTYPE_code_VALUE 0x00
#define BIT_VTYPE_code_rgba  0x02
#define BIT_VTYPE_code_hsva  0x04
#define BIT_VTYPE_code_cmyk  0x06

#define MSK_VTYPE_alpha      0x10
#define BIT_VTYPE_alpha_no   0x00
#define BIT_VTYPE_alpha_yes  0x10

typedef struct inkpot_value_s {
    IDX_VALUES index;		/* write by set,  read by get, write by first/next */
    BIT_VTYPE vtype;		/*  read by set, write by get,  read by first/next */
    VALUE value;		/*  read by set, write by get, write by first/next */
} inkpot_value_t;

extern inkpot_values_t* inkpot_values_init     ();
extern void 		inkpot_values_destroy  ( inkpot_values_t *values );

extern inkpot_status_t  inkpot_value_set       ( inkpot_values_t *values, inkpot_value_t *value );
extern inkpot_status_t  inkpot_value_get       ( inkpot_values_t *values, inkpot_value_t *value );

extern inkpot_status_t  inkpot_value_get_first ( inkpot_values_t *values, inkpot_value_t *value );
extern inkpot_status_t  inkpot_value_get_next  ( inkpot_values_t *values, inkpot_value_t *value );

#ifdef __cplusplus
}
#endif
#endif /* INKPOT_VALUE_H */

