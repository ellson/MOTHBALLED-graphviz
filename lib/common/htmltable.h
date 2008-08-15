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

#define FIXED_FLAG 1
#define HALIGN_RIGHT (1 << 1)
#define HALIGN_LEFT (1 << 2)
#define HALIGN_MASK (HALIGN_RIGHT | HALIGN_LEFT)
#define HALIGN_TEXT HALIGN_MASK
#define VALIGN_TOP (1 << 3)
#define VALIGN_BOTTOM (1 << 4)
#define VALIGN_MASK (VALIGN_TOP | VALIGN_BOTTOM)
#define BORDER_SET (1 << 5)
#define PAD_SET (1 << 6)
#define SPACE_SET (1 << 7)
#define BALIGN_RIGHT (1 << 8)
#define BALIGN_LEFT (1 << 9)
#define BALIGN_MASK (BALIGN_RIGHT | BALIGN_LEFT)

#define UNSET_ALIGN 0

    /* font information
     * If name or color is NULL, or size < 0, that attribute
     * is unspecified. 
     */
    typedef struct {
	char*  name;
	char*  color;
	double size;
	int    cnt;   /* reference count */
    } htmlfont_t;

    /* paras of text within a cell
     * NOTE: As required, the str field in para is utf-8.
     * This translation is done when libexpat scans the input.
     */
#ifdef OLD
    typedef struct {
	textpara_t *para;
	short nparas;
	box box;
	htmlfont_t *font;	/* font info */
    } htmltxt_t;
#endif
	
	/* atomic unit of text emitted using a single htmlfont_t */
    typedef struct {
	char *str;
	PostscriptAlias *postscript_alias;
	void *layout;
	void (*free_layout) (void *layout);
	htmlfont_t *font;
	double size, yoffset_layout, yoffset_centerline;
    } textitem_t;
	
	/* line of textitems_t */
    typedef struct {
	textitem_t *items;
	short nitems;
	char just;
	double size;   /* width of para */
	double lfsize; /* offset from previous baseline to current one */
    } htextpara_t;
	
    typedef struct {
	htextpara_t *paras;
	short nparas;
	boxf box;
    } htmltxt_t;

    typedef struct {
	boxf box;
	char *src;
	char *scale;
    } htmlimg_t;

    typedef struct {
	char *href;		/* pointer to an external resource */
	char *port;
	char *target;
	char *title;
	char *bgcolor;
	char *pencolor;
	signed char space;
	unsigned char border;
	unsigned char pad;
	unsigned char sides;    /* set of sides exposed to field */
	unsigned short flags;
	unsigned short width;
	unsigned short height;
	boxf box;		/* its geometric placement in points */
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
	
    extern htmllabel_t *parseHTML(char *, int *, int);

    extern int make_html_label(graph_t *g, textlabel_t * lp, void *obj);
    extern void emit_html_label(GVJ_t * job, htmllabel_t * lp, textlabel_t *);

    extern void free_html_label(htmllabel_t *, int);
    extern void free_html_data(htmldata_t *);
    extern void free_html_text(htmltxt_t *);
    extern void free_html_font(htmlfont_t*);

    extern boxf *html_port(node_t * n, char *pname, int* sides);
    extern int html_path(node_t * n, port* p, int side, box * rv, int *k);
    extern int html_inside(node_t * n, pointf p, edge_t * e);

#endif

#ifdef __cplusplus
}
#endif
