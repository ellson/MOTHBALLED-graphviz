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


#include "render.h"
#include "htmltable.h"
#include "htmlparse.h"
#include "htmllex.h"
#include <ctype.h>

#ifdef HAVE_LIBEXPAT
#include <expat.h>
#endif

#ifndef XML_STATUS_ERROR
#define XML_STATUS_ERROR 0
#endif

typedef struct {
#ifdef HAVE_LIBEXPAT
    XML_Parser parser;
#endif
    char* ptr;			/* input source */
    int tok;			/* token type   */
    agxbuf* xb;			/* buffer to gather T_string data */
    agxbuf  lb;			/* buffer for translating lexical data */
    char warn;			/* set if warning given */
    char error;			/* set if error given */
    char inCell;		/* set if in TD to allow T_string */
    char mode;			/* for handling artificial <HTML>..</HTML> */
    char *currtok;		/* for error reporting */
    char *prevtok;		/* for error reporting */
    int currtoklen;
    int prevtoklen;
} lexstate_t;
static lexstate_t state;

/* error_context:
 * Print the last 2 "token"s seen.
 */
static void error_context(void)
{
    agxbclear(state.xb);
    if (state.prevtoklen > 0)
	agxbput_n(state.xb, state.prevtok, state.prevtoklen);
    agxbput_n(state.xb, state.currtok, state.currtoklen);
    agerr(AGPREV, "... %s ...\n", agxbuse(state.xb));
}

/* htmlerror:
 * yyerror - called by yacc output
 */
void htmlerror(const char *msg)
{
    if (state.error)
	return;
    state.error = 1;
    agerr(AGERR, "%s in line %d \n", msg, htmllineno());
    error_context();
}

#ifdef HAVE_LIBEXPAT
/* lexerror:
 * called by lexer when unknown <..> is found.
 */
static void lexerror(const char *name)
{
    state.tok = T_error;
    state.error = 1;
    agerr(AGERR, "Unknown HTML element <%s> on line %d \n",
	  name, htmllineno());
}

typedef int (*attrFn) (void *, char *);
typedef int (*bcmpfn) (const void *, const void *);

#define MAX_CHAR    (((unsigned char)(~0)) >> 1)
#define MIN_CHAR    ((signed char)(~MAX_CHAR))
#define MAX_UCHAR   ((unsigned char)(~0))
#define MAX_USHORT  ((unsigned short)(~0))

/* Mechanism for automatically processing attributes */
typedef struct {
    char *name;			/* attribute name */
    attrFn action;		/* action to perform if name matches */
} attr_item;

#define ISIZE (sizeof(attr_item))

/* icmp:
 * Compare two attr_item. Used in bsearch
 */
static int icmp(attr_item * i, attr_item * j)
{
    return strcasecmp(i->name, j->name);
}

static int bgcolorfn(htmldata_t * p, char *v)
{
    p->bgcolor = strdup(v);
    return 0;
}

static int hreffn(htmldata_t * p, char *v)
{
    p->href = strdup(v);
    return 0;
}

static int titlefn(htmldata_t * p, char *v)
{
    p->title = strdup(v);
    return 0;
}

static int portfn(htmldata_t * p, char *v)
{
    p->port = strdup(v);
    return 0;
}

static int targetfn(htmldata_t * p, char *v)
{
    p->target = strdup(v);
    return 0;
}

/* doInt:
 * Scan v for integral value. Check that
 * the value is >= min and <= max. Return value in ul.
 * String s is name of value.
 * Return 0 if okay; 1 otherwise.
 */
static int doInt(char *v, char *s, int min, int max, long *ul)
{
    int rv = 0;
    char *ep;
    long b = strtol(v, &ep, 10);

    if (ep == v) {
	agerr(AGWARN, "Improper %s value %s - ignored", s, v);
	rv = 1;
    } else if (b > max) {
	agerr(AGWARN, "%s value %s > %d - too large - ignored", s, v, max);
	rv = 1;
    } else if (b < min) {
	agerr(AGWARN, "%s value %s < %d - too small - ignored", s, v, min);
	rv = 1;
    } else
	*ul = b;
    return rv;
}

static int borderfn(htmldata_t * p, char *v)
{
    long u;

    if (doInt(v, "BORDER", 0, MAX_UCHAR, &u))
	return 1;
    p->border = (unsigned char) u;
    p->flags |= BORDER_SET;
    return 0;
}

static int cellpaddingfn(htmldata_t * p, char *v)
{
    long u;

    if (doInt(v, "CELLPADDING", 0, MAX_UCHAR, &u))
	return 1;
    p->pad = (unsigned char) u;
    p->flags |= PAD_SET;
    return 0;
}

static int cellspacingfn(htmldata_t * p, char *v)
{
    long u;

    if (doInt(v, "CELLSPACING", MIN_CHAR, MAX_CHAR, &u))
	return 1;
    p->space = (signed char) u;
    p->flags |= SPACE_SET;
    return 0;
}

static int cellborderfn(htmltbl_t * p, char *v)
{
    long u;

    if (doInt(v, "CELLSBORDER", 0, MAX_CHAR, &u))
	return 1;
    p->cb = (unsigned char) u;
    return 0;
}

static int fixedsizefn(htmldata_t * p, char *v)
{
    int rv = 0;
    char c = toupper(*(unsigned char *) v);
    if ((c == 'T') && !strcasecmp(v + 1, "RUE"))
	p->flags |= FIXED_FLAG;
    else if ((c != 'F') || strcasecmp(v + 1, "ALSE")) {
	agerr(AGWARN, "Illegal value %s for FIXEDSIZE - ignored\n", v);
	rv = 1;
    }
    return rv;
}

static int valignfn(htmldata_t * p, char *v)
{
    int rv = 0;
    char c = toupper(*v);
    if ((c == 'B') && !strcasecmp(v + 1, "OTTOM"))
	p->flags |= VALIGN_BOTTOM;
    else if ((c == 'T') && !strcasecmp(v + 1, "OP"))
	p->flags |= VALIGN_TOP;
    else if ((c != 'M') || strcasecmp(v + 1, "IDDLE")) {
	agerr(AGWARN, "Illegal value %s for VALIGN - ignored\n", v);
	rv = 1;
    }
    return rv;
}

static int halignfn(htmldata_t * p, char *v)
{
    int rv = 0;
    char c = toupper(*v);
    if ((c == 'L') && !strcasecmp(v + 1, "EFT"))
	p->flags |= HALIGN_LEFT;
    else if ((c == 'R') && !strcasecmp(v + 1, "IGHT"))
	p->flags |= HALIGN_RIGHT;
    else if ((c != 'C') || strcasecmp(v + 1, "ENTER")) {
	agerr(AGWARN, "Illegal value %s for ALIGN - ignored\n", v);
	rv = 1;
    }
    return rv;
}

static int cell_halignfn(htmldata_t * p, char *v)
{
    int rv = 0;
    char c = toupper(*v);
    if ((c == 'L') && !strcasecmp(v + 1, "EFT"))
	p->flags |= HALIGN_LEFT;
    else if ((c == 'R') && !strcasecmp(v + 1, "IGHT"))
	p->flags |= HALIGN_RIGHT;
    else if ((c == 'T') && !strcasecmp(v + 1, "EXT"))
	p->flags |= HALIGN_TEXT;
    else if ((c != 'C') || strcasecmp(v + 1, "ENTER"))
	rv = 1;
    if (rv)
	agerr(AGWARN, "Illegal value %s for ALIGN in TD - ignored\n", v);
    return rv;
}

static int balignfn(htmldata_t * p, char *v)
{
    int rv = 0;
    char c = toupper(*v);
    if ((c == 'L') && !strcasecmp(v + 1, "EFT"))
	p->flags |= BALIGN_LEFT;
    else if ((c == 'R') && !strcasecmp(v + 1, "IGHT"))
	p->flags |= BALIGN_RIGHT;
    else if ((c != 'C') || strcasecmp(v + 1, "ENTER"))
	rv = 1;
    if (rv)
	agerr(AGWARN, "Illegal value %s for BALIGN in TD - ignored\n", v);
    return rv;
}

static int heightfn(htmldata_t * p, char *v)
{
    long u;

    if (doInt(v, "HEIGHT", 0, MAX_USHORT, &u))
	return 1;
    p->height = (unsigned short) u;
    return 0;
}

static int widthfn(htmldata_t * p, char *v)
{
    long u;

    if (doInt(v, "WIDTH", 0, MAX_USHORT, &u))
	return 1;
    p->width = (unsigned short) u;
    return 0;
}

static int rowspanfn(htmlcell_t * p, char *v)
{
    long u;

    if (doInt(v, "ROWSPAN", 0, MAX_USHORT, &u))
	return 1;
    if (u == 0) {
	agerr(AGWARN, "ROWSPAN value cannot be 0 - ignored\n");
	return 1;
    }
    p->rspan = (unsigned char) u;
    return 0;
}

static int colspanfn(htmlcell_t * p, char *v)
{
    long u;

    if (doInt(v, "COLSPAN", 0, MAX_USHORT, &u))
	return 1;
    if (u == 0) {
	agerr(AGWARN, "COLSPAN value cannot be 0 - ignored\n");
	return 1;
    }
    p->cspan = (unsigned char) u;
    return 0;
}

static int colorfn(htmlfont_t * p, char *v)
{
    p->color = strdup(v);
    return 0;
}

static int facefn(htmlfont_t * p, char *v)
{
    p->name = strdup(v);
    return 0;
}

static int ptsizefn(htmlfont_t * p, char *v)
{
    long u;

    if (doInt(v, "POINT-SIZE", 0, MAX_UCHAR, &u))
	return 1;
    p->size = (double) u;
    return 0;
}

static int srcfn(htmlimg_t * p, char *v)
{
    p->src = strdup(v);
    return 0;
}

static int scalefn(htmlimg_t * p, char *v)
{
    p->scale = strdup(v);
    return 0;
}

static int alignfn(int *p, char *v)
{
    int rv = 0;
    char c = toupper(*v);
    if ((c == 'R') && !strcasecmp(v + 1, "IGHT"))
	*p = 'r';
    else if ((c == 'L') || !strcasecmp(v + 1, "EFT"))
	*p = 'l';
    else if ((c == 'C') || strcasecmp(v + 1, "ENTER")) 
	*p = 'n';
    else {
	agerr(AGWARN, "Illegal value %s for ALIGN - ignored\n", v);
	rv = 1;
    }
    return rv;
}

/* Tables used in binary search; MUST be alphabetized */
static attr_item tbl_items[] = {
    {"align", (attrFn) halignfn},
    {"bgcolor", (attrFn) bgcolorfn},
    {"border", (attrFn) borderfn},
    {"cellborder", (attrFn) cellborderfn},
    {"cellpadding", (attrFn) cellpaddingfn},
    {"cellspacing", (attrFn) cellspacingfn},
    {"fixedsize", (attrFn) fixedsizefn},
    {"height", (attrFn) heightfn},
    {"href", (attrFn) hreffn},
    {"port", (attrFn) portfn},
    {"target", (attrFn) targetfn},
    {"title", (attrFn) titlefn},
    {"tooltip", (attrFn) titlefn},
    {"valign", (attrFn) valignfn},
    {"width", (attrFn) widthfn},
};

static attr_item cell_items[] = {
    {"align", (attrFn) cell_halignfn},
    {"balign", (attrFn) balignfn},
    {"bgcolor", (attrFn) bgcolorfn},
    {"border", (attrFn) borderfn},
    {"cellpadding", (attrFn) cellpaddingfn},
    {"cellspacing", (attrFn) cellspacingfn},
    {"colspan", (attrFn) colspanfn},
    {"fixedsize", (attrFn) fixedsizefn},
    {"height", (attrFn) heightfn},
    {"href", (attrFn) hreffn},
    {"port", (attrFn) portfn},
    {"rowspan", (attrFn) rowspanfn},
    {"target", (attrFn) targetfn},
    {"title", (attrFn) titlefn},
    {"tooltip", (attrFn) titlefn},
    {"valign", (attrFn) valignfn},
    {"width", (attrFn) widthfn},
};

static attr_item font_items[] = {
    {"color", (attrFn) colorfn},
    {"face", (attrFn) facefn},
    {"point-size", (attrFn) ptsizefn},
};

static attr_item img_items[] = {
    {"scale", (attrFn) scalefn},
    {"src", (attrFn) srcfn},
};

static attr_item br_items[] = {
    {"align", (attrFn) alignfn},
};

/* doAttrs:
 * General function for processing list of name/value attributes.
 * Do binary search on items table. If match found, invoke action
 * passing it tp and attribute value.
 * Table size is given by nel
 * Name/value pairs are in array atts, which is null terminated.
 * s is the name of the HTML element being processed.
 */
static void
doAttrs(void *tp, attr_item * items, int nel, char **atts, char *s)
{
    char *name;
    char *val;
    attr_item *ip;
    attr_item key;

    while ((name = *atts++) != NULL) {
	val = *atts++;
	key.name = name;
	ip = (attr_item *) bsearch(&key, items, nel, ISIZE, (bcmpfn) icmp);
	if (ip)
	    state.warn |= ip->action(tp, val);
	else {
	    agerr(AGWARN, "Illegal attribute %s in %s - ignored\n", name,
		  s);
	    state.warn = 1;
	}
    }
}

static void mkBR(char **atts)
{
    htmllval.i = UNSET_ALIGN;
    doAttrs(&htmllval.i, br_items, sizeof(br_items) / ISIZE, atts, "<BR>");
}

static htmlimg_t *mkImg(char **atts)
{
    htmlimg_t *img = NEW(htmlimg_t);

    doAttrs(img, img_items, sizeof(img_items) / ISIZE, atts, "<IMG>");

    return img;
}

static htmlfont_t *mkFont(char **atts)
{
    htmlfont_t *font = NEW(htmlfont_t);

    font->size = -1.0;		/* unassigned */
    doAttrs(font, font_items, sizeof(font_items) / ISIZE, atts, "<FONT>");

    return font;
}

static htmlcell_t *mkCell(char **atts)
{
    htmlcell_t *cell = NEW(htmlcell_t);

    cell->cspan = 1;
    cell->rspan = 1;
    doAttrs(cell, cell_items, sizeof(cell_items) / ISIZE, atts, "<TD>");

    return cell;
}

static htmltbl_t *mkTbl(char **atts)
{
    htmltbl_t *tbl = NEW(htmltbl_t);

    tbl->rc = -1;		/* flag that table is a raw, parsed table */
    tbl->cb = -1;		/* unset cell border attribute */
    doAttrs(tbl, tbl_items, sizeof(tbl_items) / ISIZE, atts, "<TABLE>");

    return tbl;
}

static void startElement(void *user, const char *name, char **atts)
{
    if (strcasecmp(name, "TABLE") == 0) {
	htmllval.tbl = mkTbl(atts);
	state.inCell = 0;
	state.tok = T_table;
    } else if ((strcasecmp(name, "TR") == 0)
	       || (strcasecmp(name, "TH") == 0)) {
	state.inCell = 0;
	state.tok = T_row;
    } else if (strcasecmp(name, "TD") == 0) {
	state.inCell = 1;
	htmllval.cell = mkCell(atts);
	state.tok = T_cell;
    } else if (strcasecmp(name, "FONT") == 0) {
	htmllval.font = mkFont(atts);
	state.tok = T_font;
    } else if (strcasecmp(name, "BR") == 0) {
	mkBR(atts);
	state.tok = T_br;
    } else if (strcasecmp(name, "IMG") == 0) {
	htmllval.img = mkImg(atts);
	state.tok = T_img;
    } else if (strcasecmp(name, "HTML") == 0) {
	state.tok = T_html;
    } else {
	lexerror(name);
    }
}

static void endElement(void *user, const char *name)
{
    if (strcasecmp(name, "TABLE") == 0) {
	state.tok = T_end_table;
	state.inCell = 1;
    } else if ((strcasecmp(name, "TR") == 0)
	       || (strcasecmp(name, "TH") == 0)) {
	state.tok = T_end_row;
    } else if (strcasecmp(name, "TD") == 0) {
	state.tok = T_end_cell;
	state.inCell = 0;
    } else if (strcasecmp(name, "HTML") == 0) {
	state.tok = T_end_html;
    } else if (strcasecmp(name, "FONT") == 0) {
	state.tok = T_end_font;
    } else if (strcasecmp(name, "BR") == 0) {
	if (state.tok == T_br)
	    state.tok = T_BR;
	else
	    state.tok = T_end_br;
    } else if (strcasecmp(name, "IMG") == 0) {
	if (state.tok == T_img)
	    state.tok = T_IMG;
	else
	    state.tok = T_end_img;
    } else {
	lexerror(name);
    }
}

/* characterData:
 * Generate T_string token. Do this only when immediately in
 * <TD>..</TD> or <HTML>..</HTML>, i.e., when inCell is true.
 * Strip out formatting characters but keep spaces.
 * Distinguish between all whitespace vs. strings with non-whitespace
 * characters.
 */
static void characterData(void *user, const char *s, int length)
{
    int i, rc, cnt = 0;
    unsigned char c;

    if (state.inCell) {
	for (i = length; i; i--) {
	    c = *s++;
	    if (c >= ' ') {
		cnt++;
		rc = agxbputc(state.xb, c);
	    }
	}
	if (cnt) state.tok = T_string;
    }
}
#endif

int initHTMLlexer(char *src, agxbuf * xb, int charset)
{
#ifdef HAVE_LIBEXPAT
    state.xb = xb;
    agxbinit (&state.lb, SMALLBUF, NULL);
    state.ptr = src;
    state.mode = 0;
    state.warn = 0;
    state.error = 0;
    state.currtoklen = 0;
    state.prevtoklen = 0;
    state.inCell = 1;
    state.parser = XML_ParserCreate(charsetToStr(charset));
    XML_SetElementHandler(state.parser,
			  (XML_StartElementHandler) startElement,
			  endElement);
    XML_SetCharacterDataHandler(state.parser, characterData);
    return 0;
#else
    static int first;
    if (!first) {
	agerr(AGWARN,
	      "Not built with libexpat. Table formatting is not available.\n");
	first++;
    }
    return 1;
#endif
}

int clearHTMLlexer()
{
#ifdef HAVE_LIBEXPAT
    int rv = state.warn | state.error;
    XML_ParserFree(state.parser);
    agxbfree (&state.lb);
    return rv;
#else
    return 1;
#endif
}

#ifdef HAVE_LIBEXPAT
/* eatComment:
 * Given first character after open comment, eat characters
 * upto comment close, returning pointer to closing > if it exists,
 * or null character otherwise.
 * We rely on HTML strings having matched nested <>.
 */
static char *eatComment(char *p)
{
    int depth = 1;
    char *s = p;
    char c;

    while (depth && (c = *s++)) {
	if (c == '<')
	    depth++;
	else if (c == '>')
	    depth--;
    }
    s--;			/* move back to '\0' or '>' */
    if (*s) {
	char *t = s - 2;
	if ((t < p) || strncmp(t, "--", 2)) {
	    agerr(AGWARN, "Unclosed comment\n");
	    state.warn = 1;
	}
    }
    return s;
}

/* findNext:
 * Return next XML unit. This is either <..>, an HTML 
 * comment <!-- ... -->, or characters up to next <.
 */
static char *findNext(char *s, agxbuf* xb)
{
    char* t = s + 1;
    char c;
    int rc;

    if (*s == '<') {
	if ((*t == '!') && !strncmp(t + 1, "--", 2))
	    t = eatComment(t + 3);
	else
	    while (*t && (*t != '>'))
		t++;
	if (*t != '>') {
	    agerr(AGWARN, "Label closed before end of HTML element\n");
	    state.warn = 1;
	} else
	    t++;
    } else {
	rc = agxbputc(xb, *s);
	while ((c = *t) && (c != '<')) {
	    if ((c == '&') && (*(t+1) != '#')) {
		t = scanEntity(t + 1, xb);
	    }
	    else {
		rc = agxbputc(xb, c);
		t++;
	    }
	}
    }
    return t;
}
#endif

int htmllineno()
{
#ifdef HAVE_LIBEXPAT
    return XML_GetCurrentLineNumber(state.parser);
#else
    return 0;
#endif
}

#ifdef DEBUG
static void printTok(int tok)
{
    char *s;

    switch (tok) {
    case T_BR:
	s = "T_BR";
	break;
    case T_br:
	s = "T_br";
	break;
    case T_end_br:
	s = "T_end_br";
	break;
    case T_end_table:
	s = "T_end_table";
	break;
    case T_row:
	s = "T_row";
	break;
    case T_end_row:
	s = "T_end_row";
	break;
    case T_end_cell:
	s = "T_end_cell";
	break;
    case T_html:
	s = "T_html";
	break;
    case T_end_html:
	s = "T_end_html";
	break;
    case T_string:
	s = "T_string";
	break;
    case T_error:
	s = "T_error";
	break;
    case T_table:
	s = "T_table";
	break;
    case T_cell:
	s = "T_cell";
	break;
    case T_img:
	s = "T_img";
	break;
    case T_end_img:
	s = "T_end_img";
	break;
    case T_IMG:
	s = "T_IMG";
	break;
    default:
	s = "<unknown>";
    }
    if (tok == T_string) {
	fprintf(stderr, "%s \"", s);
	fwrite(agxbstart(state.xb), 1, agxblen(state.xb), stderr);
	fprintf(stderr, "\"\n");
    } else
	fprintf(stderr, "%s\n", s);
}

#endif

int htmllex()
{
#ifdef HAVE_LIBEXPAT
    static char *begin_html = "<HTML>";
    static char *end_html = "</HTML>";

    char *s;
    char *endp = 0;
    int len, llen;
    int rv;

    state.tok = 0;
    do {
	if (state.mode == 2)
	    return EOF;
	if (state.mode == 0) {
	    state.mode = 1;
	    s = begin_html;
	    len = strlen(s);
	    endp = 0;
	} else {
	    s = state.ptr;
	    if (*s == '\0') {
		state.mode = 2;
		s = end_html;
		len = strlen(s);
	    } else {
		endp = findNext(s,&state.lb);
		len = endp - s;
	    }
	}
	state.prevtok = state.currtok;
	state.prevtoklen = state.currtoklen;
	state.currtok = s;
	state.currtoklen = len;
	if ((llen = agxblen(&state.lb)))
	    rv = XML_Parse(state.parser, agxbuse(&state.lb),llen, 0);
	else
	    rv = XML_Parse(state.parser, s, len, (len ? 0 : 1));
	if (rv == XML_STATUS_ERROR) {
	    if (!state.error) {
		agerr(AGERR, "%s in line %d \n",
		      XML_ErrorString(XML_GetErrorCode(state.parser)),
		      htmllineno());
		error_context();
		state.error = 1;
		state.tok = T_error;
	    }
	}
	if (endp)
	    state.ptr = endp;
    } while (state.tok == 0);
    return state.tok;
#else
    return EOF;
#endif
}
