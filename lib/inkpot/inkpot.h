/* $Id$ $Revision$ */
/* vim:set shiftwidth=4 ts=8: */

/**********************************************************
*      This software is part of the graphviz package      *
*                http://www.graphviz.org/                 *
*                                                         *
*            Copyright (c) 1994-2004 AT&T Corp.           *
*                and is licensed under the                *
*            Common Public License, Version 1.0           *
*                      by AT&T Corp.                      *
*                                                         *
*        Information and Software Systems Research        *
*              AT&T Research, Florham Park NJ             *
**********************************************************/

#ifndef COLOR_H
#define COLOR_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MIN
#define MIN(a,b)    ((a)<(b)?(a):(b))
#endif

#ifndef MAX
#define MAX(a,b)    ((a)>(b)?(a):(b))
#endif

typedef enum {
	INKPOT_SUCCESS=0,
	INKPOT_MALLOC_FAIL,
	INKPOT_COLOR_UNKNOWN,
	INKPOT_SCHEME_UNKNOWN,
	INKPOT_MAX_ONE_INDEXED_SCHEME
} inkpot_status_t;

typedef struct inkpot_s inkpot_t;

/* possible representations of color in gvcolor_t */
typedef enum {
	HSVA_DOUBLE,
	RGBA_DOUBLE,
	RGBA_WORD,
	RGBA_BYTE,
	CMYK_BYTE,
	COLOR_STRING,
	COLOR_INDEX
} color_type_t;

/* gvcolor_t can hold a color spec in a choice or representations */
typedef struct gvcolor_s {
    union {
	double HSVA[4];
	double RGBA[4];
	int rrggbbaa[4];
	unsigned char rgba[4];
	unsigned char cmyk[4];
	char *string;
	int index;
    } u;
    color_type_t type;
} gvcolor_t;

extern inkpot_status_t inkpot_init( inkpot_t **inkpot );
extern inkpot_status_t inkpot_clear( inkpot_t *inkpot );
extern inkpot_status_t inkpot_add( inkpot_t *inkpot, const char *scheme );

extern inkpot_status_t inkpot_find( inkpot_t *inkpot, const char *color );
extern inkpot_status_t inkpot_find_default( inkpot_t *inkpot);
extern void inkpot_get_rgba( inkpot_t *inkpot,
	unsigned int *r, unsigned int *g, unsigned int *b, unsigned int *a);
extern void inkpot_get_hsva( inkpot_t *inkpot,
	unsigned int *h, unsigned int *s, unsigned int *v, unsigned int *a);

extern void inkpot_print_schemes( inkpot_t *inkpot );
extern void inkpot_print_names( inkpot_t *inkpot );
extern void inkpot_print_values( inkpot_t *inkpot );
extern void inkpot_print_rgba( inkpot_t *inkpot );

extern int setColorScheme (const char* s);
extern inkpot_status_t colorxlate(const char *str, gvcolor_t * color, color_type_t target_type);
extern char *canontoken(const char *str);

#ifdef __cplusplus
}
#endif
#endif /* COLOR_H */
