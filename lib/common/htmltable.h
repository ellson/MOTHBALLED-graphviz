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

#ifdef __cplusplus
extern "C" {
#endif

#ifndef TABLE_H
#define TABLE_H

#include "render.h"
#include "gvrender.h"

#define FIXED_FLAG 1
#define HALIGN_RIGHT (1 << 1)
#define HALIGN_LEFT (1 << 2)
#define HALIGN_MASK (HALIGN_RIGHT | HALIGN_LEFT)
#define VALIGN_TOP (1 << 3)
#define VALIGN_BOTTOM (1 << 4)
#define VALIGN_MASK (VALIGN_TOP | VALIGN_BOTTOM)
#define BORDER_SET (1 << 5)
#define PAD_SET (1 << 6)
#define SPACE_SET (1 << 7)

    /* font information
     * If name or color is NULL, or size < 0, that attribute
     * is unspecified. 
     */
    typedef struct {
	char *name;
	char *color;
	double size;
    } htmlfont_t;

    /* lines of text within a cell */
    typedef struct {
	textline_t *line;
	short nlines;
	box box;
	htmlfont_t *font;	/* font info */
    } htmltxt_t;

    typedef struct {
	box box;
	char *src;
    } htmlimg_t;

    typedef struct {
	char *href;		/* pointer to an external resource */
	char *port;
	char *target;
	char *bgcolor;
	char *pencolor;
	signed char space;
	unsigned char border;
	unsigned char pad;
	unsigned char flags;
	unsigned short width;
	unsigned short height;
	box box;		/* its geometric placement in points */
    } htmldata_t;

#define HTML_UNSET 0
#define HTML_TBL 1
#define HTML_TEXT 2
#define HTML_IMAGE 3

    typedef struct htmlcell_t htmlcell_t;
    typedef struct htmltbl_t htmltbl_t;

    struct htmltbl_t {
	htmldata_t data;
	union {
	    struct {
		htmlcell_t *parent;	/* enclosing cell */
		htmlcell_t **cells;	/* cells */
	    } n;
	    struct {
		htmltbl_t *prev;	/* stack */
		Dt_t *rows;	/* cells */
	    } p;
	} u;
	signed char cb;		/* cell border */
	int *heights;		/* heights of the rows */
	int *widths;		/* widths of the columns */
	int rc;			/* number of rows */
	int cc;			/* number of columns */
	htmlfont_t *font;	/* font info */
    };

    struct htmllabel_t {
	union {
	    htmltbl_t *tbl;
	    htmltxt_t *txt;
	    htmlimg_t *img;
	} u;
	char kind;
    };

    struct htmlcell_t {
	htmldata_t data;
	unsigned short cspan;
	unsigned short rspan;
	unsigned short col;
	unsigned short row;
	htmllabel_t child;
	htmltbl_t *parent;
    };

/* During parsing, table contents are stored as rows of cells.
 * A row is a list of cells
 * Rows is a list of rows.
 * pitems are used for both lists.
 */
    typedef struct {
	Dtlink_t link;
	union {
	    Dt_t *rp;
	    htmlcell_t *cp;
	} u;
    } pitem;

    extern htmllabel_t *parseHTML(char *, int *);

    extern int make_html_label(textlabel_t * lp, void *obj);
    extern void emit_html_label(GVC_t * gvc, htmllabel_t * lp,
				textlabel_t *, void *obj);

    extern void free_html_label(htmllabel_t *, int);
    extern void free_html_data(htmldata_t *);
    extern void free_html_text(htmltxt_t *);

    extern box *html_port(node_t * n, char *pname);
    extern int html_path(node_t * n, edge_t * e, int pt, box * rv, int *k);
    extern int html_inside(node_t * n, pointf p, edge_t * e);

#endif

#ifdef __cplusplus
}
#endif
