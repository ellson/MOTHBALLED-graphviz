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

#ifndef TEXTPARA_H
#define TEXTPARA_H

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct _PostscriptAlias {
        char* name;
        char* family;
        char* weight;
        char* stretch;
        char* style;
        int xfig_code;
	char* svg_font_family;
	char* svg_font_weight;
	char* svg_font_style;
    } PostscriptAlias;

    typedef struct textpara_t {
	char *str;      /* stored in utf-8 */
	char *fontname; 
	PostscriptAlias *postscript_alias; 
	void *layout;
	void (*free_layout) (void *layout);   /* FIXME - this is ugly */
	double fontsize, width, height, yoffset_layout, yoffset_centerline;
	char just;
    } textpara_t;

#ifdef __cplusplus
}
#endif
#endif
