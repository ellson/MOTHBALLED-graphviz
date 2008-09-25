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

#ifndef INKPOT_H
#define INKPOT_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	INKPOT_SUCCESS=0,
	INKPOT_MALLOC_FAIL,
	INKPOT_COLOR_UNKNOWN,
	INKPOT_COLOR_NONAME,
	INKPOT_SCHEME_UNKNOWN,
	INKPOT_FAIL
} inkpot_status_t;

typedef struct inkpot_s inkpot_t;

extern inkpot_status_t inkpot_init	      ( inkpot_t **inkpot );
extern inkpot_status_t inkpot_clear	      ( inkpot_t *inkpot );
extern inkpot_status_t inkpot_activate	      ( inkpot_t *inkpot, const char *scheme );
extern inkpot_status_t inkpot_translate	      ( inkpot_t *inkpot, const char *scheme );

extern inkpot_status_t inkpot_set	      ( inkpot_t *inkpot, const char *color );
extern inkpot_status_t inkpot_set_default     ( inkpot_t *inkpot );
extern inkpot_status_t inkpot_set_rgba	      ( inkpot_t *inkpot, unsigned char rgba[4] );

extern inkpot_status_t inkpot_get	      ( inkpot_t *inkpot, const char **color );
extern inkpot_status_t inkpot_get_rgba	      ( inkpot_t *inkpot, unsigned char *rgba );
extern inkpot_status_t inkpot_get_hsva	      ( inkpot_t *inkpot, unsigned char *hsva );
extern inkpot_status_t inkpot_get_cmyk	      ( inkpot_t *inkpot, unsigned char *cmyk );
extern inkpot_status_t inkpot_get_RGBA	      ( inkpot_t *inkpot, double *RGBA );
extern inkpot_status_t inkpot_get_HSVA	      ( inkpot_t *inkpot, double *HSVA );
extern inkpot_status_t inkpot_get_index	      ( inkpot_t *inkpot, unsigned int *index );

extern inkpot_status_t inkpot_print_schemes   ( inkpot_t *inkpot, FILE *out );
extern inkpot_status_t inkpot_print_names     ( inkpot_t *inkpot, FILE *out );
extern inkpot_status_t inkpot_print_names_out ( inkpot_t *inkpot, FILE *out );
extern inkpot_status_t inkpot_print_values    ( inkpot_t *inkpot, FILE *out );

#ifdef __cplusplus
}
#endif
#endif /* INKPOT_H */
