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

const char TAB_COLOR_STRINGS[SZT_COLOR_STRINGS] = {
	"black\0"
	"bleu\0"
	"blue\0"
	"green\0"
	"jaune\0"
	"red\0" 
	"rouge\0"
	"vert\0"
	"white\0"
	"yellow\0"
};

const char TAB_SCHEME_STRINGS[SZT_SCHEME_STRINGS] = {
	"french\0"
	"svg\0"
	"x11\0" 
	"xxx\0"
	"yyy\0"
};

inkpot_scheme_name_t TAB_SCHEMES[SZT_SCHEMES] = { /* schemes of named colors */
	{  0, },  /* french */
	{  7, },  /* svg */
	{ 11, },  /* x11 */
};

inkpot_scheme_index_t TAB_SCHEMES_INDEX[SZT_SCHEMES_INDEX] = { /* schemes of indexed colors */
	{ 15, 0, }, /* xxx */
	{ 19, 5, }, /* yyy */
};

inkpot_name_t TAB_NAMES[SZT_NAMES] = { /* Must be LC_ALL=C sort'ed by name with each alt */
	{ 0,  0, 0x6, }, /* black */
	{ 6,  1, 0x1, }, /* bleu */
	{ 11, 1, 0x6, }, /* blue */
	{ 16, 2, 0x6, }, /* green */
	{ 22, 5, 0x1, }, /* jaune */
	{ 28, 3, 0x6, }, /* red */ 
	{ 32, 3, 0x1, }, /* rouge */
	{ 38, 2, 0x1, }, /* vert */
	{ 43, 6, 0x6, }, /* white */
	{ 50, 0,   0, }, /* yellow (w. alts) */
};

inkpot_name_t TAB_ALTS[SZT_ALTS] = {
	{ 1, 5, 0x4, },  /* yellow (x11) */
	{ 0, 4, 0x2, },  /* yellow (svg) */
};

IDX_VALUE_TO TAB_VALUE_TO[SZT_VALUE_TO] = {
        0,   /* black */
	1,   /* bleu, blue */
	3,   /* green, vert */
	5,   /* red, rouge */
	7,   /* yellow (svg) */
	8,   /* yellow (x11), jaune */
	10,  /* white */
};

IDX_NAMES TAB_TO_NAMES[SZT_TO_NAMES] = {
	0,     /* black */
	1, 2,  /* bleu, blue */
	3, 7,  /* green, vert */
	5, 6,  /* red, rouge */
	9,     /* yellow(alt1) */
	4, 9,  /* jaune, yellow */
	8,     /* white */
};

IDX_VALUES TAB_IXVALUES[SZT_IXVALUES] = {
        7, 8, 9, 10, 1,  /* xxx */
	0, 6,            /* yyy */
};
