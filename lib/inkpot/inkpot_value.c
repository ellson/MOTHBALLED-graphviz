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

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "inkpot.h"
#include "inkpot_lib_define.h"
#include "inkpot_value.h"
#include "inkpot_lib_value.h"

inkpot_status_t inkpot_value_set ( inkpot_values_t *values, inkpot_value_t *value )
{
    value->vtype = VTYPE_rgba;  /* FIXME */
    value->index = SZT_VALUES + SZT_NONAME_VALUES;

    return INKPOT_SUCCESS;
}

inkpot_status_t inkpot_value_get ( inkpot_values_t *values, inkpot_value_t *value )
{
    IDX_VALUES_24_t i;
    unsigned short v;

    /* FIXME - this routine must check for 8 bit values and properly scale to 16 for the api */

    if (value->index < SZT_VALUES) {
	i = 3 * value->index;

	v = TAB_VALUES_24[i++]; value->value[0] = v | v << 8;
	v = TAB_VALUES_24[i++]; value->value[1] = v | v << 8;
	v = TAB_VALUES_24[i];   value->value[2] = v | v << 8;
	                        value->value[3] = 0xffff;

	value->vtype = VTYPE_rgba;  /* FIXME */
    }
    else if (value->index - SZT_VALUES < SZT_NONAME_VALUES) {
	i = 3 * value->index - SZT_VALUES;

	v = TAB_NONAME_VALUES_24[i++]; value->value[0] = v | v << 8;
	v = TAB_NONAME_VALUES_24[i++]; value->value[1] = v | v << 8;
	v = TAB_NONAME_VALUES_24[i];   value->value[2] = v | v << 8;
	                               value->value[3] = 0xffff;

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
