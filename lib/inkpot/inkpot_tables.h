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

typedef unsigned int   IDX_STRINGS;
typedef unsigned short MSK_SCHEMES_NAME;
typedef unsigned char  IDX_SCHEMES_NAME;
typedef unsigned char  IDX_SCHEMES_INDEX;
typedef unsigned int   IDX_NAMES;
typedef unsigned char  IDX_NAME_ALTS;
typedef unsigned int   IDX_VALUES;
typedef unsigned int   IDX_IXVALUES;

typedef unsigned int   RGBA;
#define SZB_RED 8
#define MSK_RED 255
#define MAX_RED 255

#include "inkpot_structs.h"

const char TAB_STRINGS[] = {
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
#define SZT_STRINGS sizeof(TAB_STRINGS)/sizeof(TAB_STRINGS[0])

inkpot_scheme_name_t TAB_SCHEMES_NAME[] = { /* schemes of named colors */
	{  0, 1, },  /* french */
	{  7, 0, },  /* svg */
	{ 11, 0, },  /* x11 */
};
#define SZT_SCHEMES_NAME (sizeof(TAB_SCHEMES_NAME)/sizeof(TAB_SCHEMES_NAME[0]))

inkpot_scheme_index_t TAB_SCHEMES_INDEX[] = { /* schemes of indexed colors */
	{ 71, 0, }, /* xxx */
	{ 75, 5, }, /* yyy */
};
#define SZT_SCHEMES_INDEX (sizeof(TAB_SCHEMES_INDEX)/sizeof(TAB_SCHEMES_INDEX[0]))

inkpot_name_t TAB_NAMES[] = { /* Must be LC_ALL=C sort'ed by name with each alt */
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
#define SZT_NAMES (sizeof(TAB_NAMES)/sizeof(TAB_NAMES[0]))

IDX_NAMES TAB_NAME_ALTS[] = {
	0, 10,
};
#define SZT_NAME_ALTS (sizeof(TAB_NAME_ALTS)/sizeof(TAB_NAME_ALTS[0]))

inkpot_value_t TAB_VALUES[] = { /* Must be sort'ed */
	{ 0x000000ff, 0 },  /* black */
	{ 0x0000ffff, 1 },  /* bleu, blue */
	{ 0x00ff00ff, 3 },  /* green, vert */
	{ 0xff0000ff, 5 },  /* red, rouge */
	{ 0xffcf00ff, 7 },  /* yellow (svg) */
	{ 0xffff00ff, 8 },  /* yellow (x11), jaune */
	{ 0xffffffff, 10},  /* white */
}; 
#define SZT_VALUES (sizeof(TAB_VALUES)/sizeof(TAB_VALUES[0]))

inkpot_noname_value_t TAB_NONAME_VALUES[] = { /* Must be sort'ed */
	{ 0x000080ff },
	{ 0x0000a4ff },
	{ 0x0000c8ff },
	{ 0x0000eaff },
}; 
#define SZT_NONAME_VALUES (sizeof(TAB_NONAME_VALUES)/sizeof(TAB_NONAME_VALUES[0]))

IDX_VALUES TAB_IXVALUES[] = {
	7, 8, 9, 10, 1,  /* xxx */
	0, 6,	         /* yyy */
};
#define SZT_IXVALUES sizeof(TAB_IXVALUES)/sizeof(TAB_IXVALUES[0])
