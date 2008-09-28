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

typedef enum {
    VTYPE_rgba = 0,
    VTYPE_hsva,
    VTYPE_cmyk,
    SZT_VTYPE
} inkpot_value_vtype_t;

typedef struct inkpot_values_s {
#if 0
/* FIXME - not sure about all this ... */
    unsigned long *         named[SZT_VTYPE];
    unsigned long *       unnamed[SZT_VTYPE];
    unsigned long *       palette[SZT_VTYPE];
    size_t          named_64_size[SZT_VTYPE];
    size_t          named_48_size[SZT_VTYPE];
    size_t          named_32_size[SZT_VTYPE];
    size_t          named_24_size[SZT_VTYPE];
    size_t        unnamed_64_size[SZT_VTYPE];
    size_t        unnamed_48_size[SZT_VTYPE];
    size_t        unnamed_32_size[SZT_VTYPE];
    size_t        unnamed_24_size[SZT_VTYPE];
    size_t        palette_64_size[SZT_VTYPE];
    size_t        palette_48_size[SZT_VTYPE];
    size_t        palette_32_size[SZT_VTYPE];
    size_t        palette_24_size[SZT_VTYPE];
    size_t          palette_alloc[SZT_VTYPE];
#endif
} inkpot_values_t;

typedef struct inkpot_value_s {
    IDX_VALUES index;	 /* write by set,  read by get, write by first, read/write by next */
    BIT_VTYPE  vtype;	 /*  read by set, write by get,  read by first,       read by next */
    unsigned long value; /*  read by set, write by get, write by first,      write by next */
} inkpot_value_t;

extern inkpot_status_t  inkpot_value_set       ( inkpot_values_t *values, inkpot_value_t *value );
extern inkpot_status_t  inkpot_value_get       ( inkpot_values_t *values, inkpot_value_t *value );

extern inkpot_status_t  inkpot_value_get_first ( inkpot_values_t *values, inkpot_value_t *value );
extern inkpot_status_t  inkpot_value_get_next  ( inkpot_values_t *values, inkpot_value_t *value );

#ifdef __cplusplus
}
#endif
#endif /* INKPOT_VALUE_H */

