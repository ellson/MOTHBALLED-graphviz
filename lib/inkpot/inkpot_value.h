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

typedef int BIT_VTYPE;

#define MSK_VTYPE_size       0x01
#define BIT_VTYPE_size_16    0x00
#define BIT_VTYPE_size_8     0x01

#define MSK_VTYPE_code       0x0e
#define BIT_VTYPE_code_VALUE 0x00
#define BIT_VTYPE_code_rgba  0x02
#define BIT_VTYPE_code_hsva  0x04
#define BIT_VTYPE_code_cmyk  0x06

#define MSK_VTYPE_alpha      0x10
#define BIT_VTYPE_alpha_yes  0x00
#define BIT_VTYPE_alpha_no   0x10

typedef struct inkpot_values_s {
    VALUE no_palette_value;
    BIT_VTYPE no_palette_vtype;
} inkpot_values_t;

typedef struct inkpot_value_s {
    IDX_VALUES index;		/* write by set,  read by get, write by first, read/write by next */
    BIT_VTYPE vtype;		/*  read by set, write by get,  read by first,       read by next */
    VALUE value;		/*  read by set, write by get, write by first,      write by next */
} inkpot_value_t;

extern inkpot_status_t  inkpot_value_set       ( inkpot_values_t *values, inkpot_value_t *value );
extern inkpot_status_t  inkpot_value_get       ( inkpot_values_t *values, inkpot_value_t *value );

extern inkpot_status_t  inkpot_value_get_first ( inkpot_values_t *values, inkpot_value_t *value );
extern inkpot_status_t  inkpot_value_get_next  ( inkpot_values_t *values, inkpot_value_t *value );

#ifdef __cplusplus
}
#endif
#endif /* INKPOT_VALUE_H */

