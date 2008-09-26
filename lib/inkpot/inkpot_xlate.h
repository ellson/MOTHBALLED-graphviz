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

#ifndef INKPOT_XLATE_H
#define INKPOT_XLATE_H

#ifdef __cplusplus
extern "C" {
#endif

/* possible representations of color in gvcolor_t */
typedef enum { HSVA_DOUBLE, RGBA_BYTE, RGBA_WORD, CMYK_BYTE,
                RGBA_DOUBLE, COLOR_STRING, COLOR_INDEX } color_type_t;

/* gvcolor_t can hold a color spec in a choice or representations */
typedef struct color_s {
    union {
        double RGBA[4];
        double HSVA[4];
        unsigned char rgba[4];
        unsigned char cmyk[4];
        int rrggbbaa[4];
        char *string;
        int index;
    } u;
    color_type_t type;
} gvcolor_t;


#ifdef __cplusplus
}
#endif
#endif /* INKPOT_XLATE_H */
