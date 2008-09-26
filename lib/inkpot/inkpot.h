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

typedef struct inkpot_disc_s {
    size_t (*out_writer) (void *out_closure, const char *data, size_t length);
    size_t (*err_writer) (void *err_closure, const char *data, size_t length);
} inkpot_disc_t;



extern inkpot_t *      inkpot_init	      ( void );
extern void            inkpot_destroy	      ( inkpot_t *inkpot );

extern inkpot_status_t inkpot_disciplines     ( inkpot_t *inkpot, inkpot_disc_t disc, void *out_closure, void *err_closure );

extern inkpot_status_t inkpot_schemes	      ( inkpot_t *inkpot, const char *scheme, ... );
extern inkpot_status_t inkpot_translate       ( inkpot_t *inkpot, const char *scheme );

extern inkpot_status_t inkpot_set	      ( inkpot_t *inkpot, const char *color );
extern inkpot_status_t inkpot_set_default     ( inkpot_t *inkpot );
extern inkpot_status_t inkpot_set_rgba	      ( inkpot_t *inkpot, double rgba[4] );
extern inkpot_status_t inkpot_set_hsva	      ( inkpot_t *inkpot, double hsva[4] );

extern inkpot_status_t inkpot_get	      ( inkpot_t *inkpot, const char **color );
extern inkpot_status_t inkpot_get_rgba_i      ( inkpot_t *inkpot, unsigned int rgba[4] );
extern inkpot_status_t inkpot_get_rgba	      ( inkpot_t *inkpot, double rgba[4] );
extern inkpot_status_t inkpot_get_hsva	      ( inkpot_t *inkpot, double hsva[4] );
extern inkpot_status_t inkpot_get_cmyk	      ( inkpot_t *inkpot, double cmyk[4] );
extern inkpot_status_t inkpot_get_index	      ( inkpot_t *inkpot, unsigned int *index );

extern inkpot_status_t inkpot_write	      ( inkpot_t *inkpot );

extern inkpot_status_t inkpot_debug_schemes   ( inkpot_t *inkpot );
extern inkpot_status_t inkpot_debug_names     ( inkpot_t *inkpot );
extern inkpot_status_t inkpot_debug_out_names ( inkpot_t *inkpot );
extern inkpot_status_t inkpot_debug_values    ( inkpot_t *inkpot );

extern inkpot_status_t inkpot_error	      ( inkpot_t *inkpot );

#ifdef __cplusplus
}
#endif
#endif /* INKPOT_H */
