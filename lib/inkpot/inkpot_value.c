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

inkpot_status_t inkpot_value_set ( inkpot_values_t *values, inkpot_value_t *value )
{
    value->vtype = VTYPE_rgba;  /* FIXME */
    value->index = SZT_VALUES + SZT_NONAME_VALUES;

    return INKPOT_SUCCESS;
}

inkpot_status_t inkpot_value_get ( inkpot_values_t *values, inkpot_value_t *value )
{
    IDX_VALUES_24 i;

    /* FIXME - this routine must check for 8 bit values and properly scale to 16 for the api */

    if (value->index < SZT_VALUES) {
	i = 3 * value->index;

	value->value  = TAB_VALUES_24[i++] << 16;
	value->value |= TAB_VALUES_24[i++] << 16;
	value->value |= TAB_VALUES_24[i] << 16;
	value->value |= 0xff;
	value->value |= value->value << 8;

	value->vtype = VTYPE_rgba;  /* FIXME */
    }
    else if (value->index - SZT_VALUES < SZT_NONAME_VALUES) {
	i = 3 * value->index;

	value->value  = TAB_NONAME_VALUES_24[i++] << 16;
	value->value |= TAB_NONAME_VALUES_24[i++] << 16;
	value->value |= TAB_NONAME_VALUES_24[i] << 16;
	value->value |= 0xff;
	value->value |= value->value << 8;

	value->vtype = VTYPE_rgba;  /* FIXME */
    }
    else if (value->index == SZT_VALUES + SZT_NONAME_VALUES) {
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
