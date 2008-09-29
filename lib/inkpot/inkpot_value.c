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

inkpot_status_t inkpot_value_set ( inkpot_values_t *values, inkpot_value_t *value )
{
    value->vtype = VTYPE_rgba;  /* FIXME */
    value->index = SZT_VALUES + SZT_NONAME_VALUES;

    return INKPOT_SUCCESS;
}

inkpot_status_t inkpot_value_get ( inkpot_values_t *values, inkpot_value_t *value )
{
    IDX_VALUES index = value->index;

    /* FIXME - this routine must check for 8 bit values and properly scale to 16 for the api */

    if (index < SZT_VALUES) {
	value->value = TAB_VALUES[index];
	value->vtype = VTYPE_rgba;  /* FIXME */
    }
    else if (index - SZT_VALUES < SZT_NONAME_VALUES) {
	value->value = TAB_NONAME_VALUES[index - SZT_VALUES];
	value->vtype = VTYPE_rgba;  /* FIXME */
    }
    else if (index == SZT_VALUES + SZT_NONAME_VALUES) {
	value->vtype = VTYPE_rgba;  /* FIXME */
        return INKPOT_NOPALETTE;
    }
    else
	return INKPOT_NOSUCH_INDEX;

    return INKPOT_SUCCESS;
}

inkpot_status_t  inkpot_value_get_first ( inkpot_values_t *values, inkpot_value_t *value )
{
    value->index = 0;
    return inkpot_value_get ( values, value );
}

inkpot_status_t  inkpot_value_get_next ( inkpot_values_t *values, inkpot_value_t *value )
{
    value->index++;
    return inkpot_value_get ( values, value );
}
