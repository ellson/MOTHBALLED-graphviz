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
#include <assert.h>

#include "inkpot.h"
#include "inkpot_define.h"
#include "inkpot_value.h"
#include "inkpot_struct.h"
#include "inkpot_value_table.h"

#if 0
static int inkpot_value_cmpf ( const void *key, const void *base)
{
    VALUE value_key = *(VALUE*)key;
    VALUE value_base = *(VALUE*)base;

    if (value_key > value_base) return  1;
	if (value_key < value_base) return -1;
	return 0;
}
#endif

inkpot_status_t inkpot_set_value ( inkpot_t *inkpot, VALUE value ) 
{

    inkpot->no_palette_value = value;
    inkpot->value_idx = SZT_VALUES + SZT_NONAME_VALUES;  /* indicate presence of no_palette_value */


#if 0
    inkpot_value_t *value;
    inkpot_noname_value_t *noname_value;

/*
 * FIXME - implement caching and check here
 */

    value = (inkpot_value_t *) bsearch(
        (void*)(value), (void*)TAB_VALUES,
        SZT_VALUES, sizeof(inkpot_value_t),
        inkpot_value_cmpf); 

    if (value)
	return inkpot_set_value_idx(inkpot, (value - TAB_VALUES));

    noname_value = (inkpot_noname_value_t *) bsearch(
        (void*)(value), (void*)TAB_NONAME_VALUES,
        SZT_NONAME_VALUES, sizeof(inkpot_noname_value_t),
        inkpot_value_cmpf); 

    if (noname_value)
        return inkpot_set_value_idx(inkpot, ((noname_value - TAB_NONAME_VALUES) + SZT_VALUES));

    return ((inkpot->status = INKPOT_COLOR_NONAME));
    /* need some sort of btree here so that we can insert value
     * values and keep sorted */

    noname_value = (inkpot_noname_value_t *) bsearch(
        (void*)(value), (void*)TAB_DYNAMIC_VALUES,
        SZT_DYNAMIC_VALUES, sizeof(inkpot_noname_value_t),
        inkpot_value_cmpf); 
    
    /* insert value and keep sorted */

#endif
    return ((inkpot->status = INKPOT_SUCCESS));
}

inkpot_status_t inkpot_get_value ( inkpot_t *inkpot, VALUE *value )
{
    IDX_VALUES value_idx = inkpot->value_idx;

    if (value_idx < SZT_VALUES)
	*value = TAB_VALUES[value_idx].value;
    else if (value_idx - SZT_VALUES < SZT_NONAME_VALUES)
	*value = TAB_NONAME_VALUES[value_idx - SZT_VALUES].value;
    else if (value_idx == SZT_VALUES + SZT_NONAME_VALUES)
	*value = inkpot->no_palette_value;
    else
	assert(0);

    return ((inkpot->status = INKPOT_SUCCESS));
}
