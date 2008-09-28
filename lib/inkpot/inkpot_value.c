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
#include <stdlib.h>
#include <assert.h>

#include "inkpot.h"
#include "inkpot_define.h"
#include "inkpot_value.h"
#include "inkpot_value_table.h"

struct inkpot_values_s {
    VALUE
	no_palette_value;
    BIT_VTYPE 
	no_palette_vtype;
};

inkpot_values_t* inkpot_values_init ( void )
{
    inkpot_values_t *values;
   
    values = (inkpot_values_t*)malloc(sizeof(inkpot_values_t));
    if (values) {

    }
    return values;
}

void inkpot_values_destroy ( inkpot_values_t *values )
{
    free(values);
}

inkpot_status_t inkpot_value_set ( inkpot_values_t *values, inkpot_value_t *value )
{
    values->no_palette_value = value->value;
    values->no_palette_vtype = value->vtype;
    value->index = SZT_VALUES + SZT_NONAME_VALUES;

    return INKPOT_SUCCESS;
}

inkpot_status_t inkpot_value_get ( inkpot_values_t *values, inkpot_value_t *value )
{
    IDX_VALUES index;
   
    index  = value->index;
    if (index < SZT_VALUES) {
	value->value = TAB_VALUES[index].value;
	value->vtype = BIT_VTYPE_size_16 | BIT_VTYPE_code_VALUE | BIT_VTYPE_alpha_yes;
    }
    else if (index - SZT_VALUES < SZT_NONAME_VALUES) {
	value->value = TAB_NONAME_VALUES[index - SZT_VALUES].value;
	value->vtype = BIT_VTYPE_size_16 | BIT_VTYPE_code_VALUE | BIT_VTYPE_alpha_yes;
    }
    else if (index == SZT_VALUES + SZT_NONAME_VALUES) {
	value->value = values->no_palette_value;
	value->vtype = values->no_palette_vtype;
    }
    else
	assert(0);

    return INKPOT_SUCCESS;
}
