typedef unsigned char IDX_STRINGS;
typedef unsigned char BIT_SCHEMES_NAME;
typedef unsigned char IDX_SCHEMES_NAME;
typedef unsigned char IDX_SCHEMES_INDEX;
typedef unsigned char IDX_NAMES;
typedef unsigned char IDX_VALUES;
typedef unsigned char IDX_TONAMES;
typedef unsigned char IDX_IXVALUES;

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
	{  0, 1,  7, 1, },  /* french */
	{  7, 0, 10, 0, },  /* svg */
	{ 11, 0, 10, 0, },  /* x11 */
};
#define SZT_SCHEMES_NAME (sizeof(TAB_SCHEMES_NAME)/sizeof(TAB_SCHEMES_NAME[0]))

inkpot_scheme_index_t TAB_SCHEMES_INDEX[] = { /* schemes of indexed colors */
	{ 71, 0, }, /* xxx */
	{ 75, 5, }, /* yyy */
};
#define SZT_SCHEMES_INDEX (sizeof(TAB_SCHEMES_INDEX)/sizeof(TAB_SCHEMES_INDEX[0]))

inkpot_name_t TAB_NAMES[] = { /* Must be LC_ALL=C sort'ed by name */
	{ 15, 0, 0x6,  0, }, /* black */
	{ 21, 1, 0x1,  1, }, /* bleu */
	{ 26, 1, 0x6,  2, }, /* blue */
	{ 31, 2, 0x6,  3, }, /* green */
	{ 37, 5, 0x1,  7, }, /* jaune */
	{ 43, 3, 0x6,  5, }, /* red */
	{ 47, 3, 0x1,  6, }, /* rouge */
	{ 53, 2, 0x1,  8, }, /* vert */
	{ 58, 4, 0x2,  4, }, /* yellow (svg) */
	{ 58, 5, 0x4,  9, }, /* yellow (x11) */
	{ 65, 6, 0x6, 10, }, /* white */
};
#define SZT_NAMES (sizeof(TAB_NAMES)/sizeof(TAB_NAMES[0]))

inkpot_value_t TAB_VALUES[] = { /* Must be LC_ALL=C sort'ed by r,g,b,a */
	{ 0,	0,	0,	255,	0,  },  /* black */
	{ 0,	0,	255,	255,	1,  },  /* blue, bleu */
	{ 0,	255,	0,	255,	3,  },  /* green, vert */
	{ 255,	0,	0,	255,	5,  },  /* red, rouge */
	{ 255,	192,	0,	255,	7,  },  /* yellow (svg) */
	{ 255,	255,	0,	255,	8,  },  /* yellow (x11), jaune */
	{ 255,	255,	255,	255,	10, },  /* white */
}; 
#define SZT_VALUES (sizeof(TAB_VALUES)/sizeof(TAB_VALUES[0]))

inkpot_noname_value_t TAB_NONAME_VALUES[] = { /* Must be LC_ALL=C sort'ed by r,g,b,a */
	{ 0,	0,	128,	255 },
	{ 0,	0,	160,	255 },
	{ 0,	0,	192,	255 },
	{ 0,	0,	224,	255 },
}; 
#define SZT_NONAME_VALUES (sizeof(TAB_NONAME_VALUES)/sizeof(TAB_NONAME_VALUES[0]))

IDX_VALUES TAB_IXVALUES[] = {
	7, 8, 9, 10, 1,  /* xxx */
	0, 6,	         /* yyy */
};
#define SZT_IXVALUES sizeof(TAB_IXVALUES)/sizeof(TAB_IXVALUES[0])
