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

const char TAB_STRINGS[SZT_STRINGS] = {
	"french\0"
	"svg\0"
	"x11\0" 
	"black\0"
	"bleu\0"
	"blue\0"
	"green\0"
	"jaune\0"
	"red\0" 
	"rouge\0"
	"vert\0"
	"yellow\0"
	"white\0"
	"xxx\0"
	"yyy\0"
};

inkpot_scheme_name_t TAB_SCHEMES_NAME[SZT_SCHEMES_NAME] = { /* schemes of named colors */
	{  0, },  /* french */
	{  7, },  /* svg */
	{ 11, },  /* x11 */
};

inkpot_scheme_index_t TAB_SCHEMES_INDEX[SZT_SCHEMES_INDEX] = { /* schemes of indexed colors */
	{ 71, 0, }, /* xxx */
	{ 75, 5, }, /* yyy */
};

inkpot_name_t TAB_NAMES[SZT_NAMES] = { /* Must be LC_ALL=C sort'ed by name with each alt */
	{ 15, 0, 0x6, /* black */  0,  /* black */ 	   }, /* 0  */
	{ 21, 1, 0x1, /* bleu */   1,  /* bleu, blue */    }, /* 1  */
	{ 26, 1, 0x6, /* blue */   2,			   }, /* 2  */
	{ 31, 2, 0x6, /* green */  3,  /* green, vert */   }, /* 3  */
	{ 37, 5, 0x1, /* jaune */  7,			   }, /* 4  */
	{ 43, 3, 0x6, /* red */    5,  /* red, rouge */    }, /* 5  */
	{ 47, 3, 0x1, /* rouge */  6,			   }, /* 6  */
	{ 53, 2, 0x1, /* vert */   10, /* yellow(alt1) */  }, /* 7  */
	{ 65, 6, 0x6, /* white */  4,  /* jaune, yellow */ }, /* 8  */
	{ 58, 5, 0x4, /* yellow */ 9,			   }, /* 9  */
/* alt1 */
	{ 58, 4, 0x2, /* yellow */ 8,  /* white */	   }, /* 10 */
};

IDX_NAMES TAB_NAME_ALTS[SZT_NAME_ALTS] = {
	0, 10,
};

IDX_TONAMES TAB_TONAMES[SZT_TONAMES] = {
        0,   /* black */
	1,   /* bleu, blue */
	3,   /* green, vert */
	5,   /* red, rouge */
	7,   /* yellow (svg) */
	8,   /* yellow (x11), jaune */
	10,  /* white */
};

IDX_VALUES TAB_IXVALUES[SZT_IXVALUES] = {
        7, 8, 9, 10, 1,  /* xxx */
	0, 6,            /* yyy */
};
