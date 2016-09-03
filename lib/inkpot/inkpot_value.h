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

#ifndef INKPOT_VALUE_H
#define INKPOT_VALUE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef int BIT_VTYPE_t;

typedef enum {
    VTYPE_rgba = 0,
    VTYPE_hsva,
    VTYPE_cmyk,
    SZT_VTYPE
} inkpot_value_vtype_t;

typedef struct inkpot_values_s {
#if 0
/* FIXME - not sure about all this ... */
    uint64_t *         named[SZT_VTYPE];
    uint64_t *       unnamed[SZT_VTYPE];
    uint64_t *       palette[SZT_VTYPE];
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
    IDX_VALUES_t index;      /* write by set,  read by get, write by first, read/write by next */
    BIT_VTYPE_t  vtype;      /*  read by set, write by get,  read by first,       read by next */
    unsigned short value[4]; /*  read by set, write by get, write by first,      write by next */
} inkpot_value_t;

extern inkpot_status_t  inkpot_value_set       ( inkpot_values_t *values, inkpot_value_t *value );
extern inkpot_status_t  inkpot_value_get       ( inkpot_values_t *values, inkpot_value_t *value );

extern inkpot_status_t  inkpot_value_get_first ( inkpot_values_t *values, inkpot_value_t *value );
extern inkpot_status_t  inkpot_value_get_next  ( inkpot_values_t *values, inkpot_value_t *value );

#ifdef __cplusplus
}
#endif
#endif /* INKPOT_VALUE_H */

