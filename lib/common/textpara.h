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

/* Bold, Italic, Underline */
#define HTML_BF 1
#define HTML_IF 2
#define HTML_UL 4

    /* font information
     * If name or color is NULL, or size < 0, that attribute
     * is unspecified. 
     */
    typedef struct {
	char*  name;
	char*  color;
        int    flags:7;  /* HTML_UL, HTML_IF, HTML_BF */
	int    cnt;   /* reference count */
	double size;
    } htmlfont_t;

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

    /* atomic unit of text emitted using a single htmlfont_t */
    typedef struct {
	char *str;      /* stored in utf-8 */
	PostscriptAlias *postscript_alias;
	void *layout;
	void (*free_layout) (void *layout);   /* FIXME - this is ugly */
	htmlfont_t *font;
	char *fontname; /* FIXME - use htmlfont_t */
	double fontsize; /* FIXME - use htmlfont_t */
	double size, yoffset_layout, yoffset_centerline;
	double width, height; /* FIXME */
	char just;	/* 'l' 'n' 'r' */ /* FIXME */
    } textpara_t;

#ifdef __cplusplus
}
#endif
#endif
