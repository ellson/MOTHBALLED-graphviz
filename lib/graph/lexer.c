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


#include <stdarg.h>
#include "libgraph.h"
#include "parser.h"
#include "triefa.cP"
#include "agxbuf.h"

#ifdef DMALLOC
#include "dmalloc.h"
#endif

#define InfileName (InputFile?InputFile:"<unknown>")

static FILE *Lexer_fp;
static char *LexPtr, *TokenBuf;
static int LineBufSize;
static uchar In_comment;
static uchar Comment_start;
static uchar Start_html_string;
static int Line_number;
static char *InputFile;
static gets_f Lexer_gets;

  /* Reset line number.
   * Argument n is indexed from 1, so we decrement it.
   */
void agreadline(int n)
{
    Line_number = n - 1;
}

  /* (Re)set file:
   */
void agsetfile(char *f)
{
    InputFile = f;
    Line_number = 0;
}

void aglexinit(FILE * fp, gets_f mygets)
{
    Lexer_fp = fp;
    Lexer_gets = mygets;
    LexPtr = NULL;
    if (AG.linebuf == NULL) {
	LineBufSize = BUFSIZ;
	AG.linebuf = N_NEW(LineBufSize, char);
	TokenBuf = N_NEW(LineBufSize, char);
    }
    (Lexer_gets) (AG.linebuf, 0, fp);	/* reset mygets */
}

#define ISSPACE(c) ((c != 0) && ((isspace(c) || iscntrl(c))))

/* skip leading white space and comments in a string p
 * whitespace includes control characters
 */
static char *skip_wscomments(char *pp)
{
    unsigned char *p = (unsigned char *) pp;
    do {
	while (ISSPACE(*p))
	    p++;
	while (In_comment && p[0]) {
	    while (p[0] && (p[0] != '*'))
		p++;
	    if (p[0]) {
		if (p[1] == '/') {
		    In_comment = FALSE;
		    p += 2;
		    break;
		} else
		    p++;
	    }
	}
	if (p[0] == '/') {
	    if (p[1] == '/')
		while (*p)
		    p++;	/* skip to end of line */
	    else {
		if (p[1] == '*') {
		    In_comment = TRUE;
		    Comment_start = Line_number;
		    p += 2;
		    continue;
		} else
		    break;	/* return a slash */
	    }
	} else {
	    if (!ISSPACE(*p))
		break;
	}
    } while (p[0]);
    return (char *) p;
}

/* scan an unquoted token and return the position after its terminator */
static char *scan_token(char *p, char *token)
{
    char *q;

    q = token;
    if (p == '\0')
	return NULL;
    while (ISALNUM(*p)) {
	*q++ = *p++;
    }
    *q = '\0';
    return p;
}

static char *scan_num(char *p, char *token)
{
    unsigned char *q, *z;
    int saw_rp = FALSE;
    int saw_digit = FALSE;

    z = (unsigned char *) p;
    q = (unsigned char *) token;
    if (*z == '-')
	*q++ = *z++;
    if (*z == '.') {
	saw_rp = TRUE;
	*q++ = *z++;
    }
    while (isdigit(*z)) {
	saw_digit = TRUE;
	*q++ = *z++;
    }
    if ((*z == '.') && (saw_rp == FALSE)) {
	saw_rp = TRUE;
	*q++ = *z++;
	while (isdigit(*z)) {
	    saw_digit = TRUE;
	    *q++ = *z++;
	}
    }
    *q = '\0';
    if (saw_digit && *z && ((isalpha(*z)) || (*z == '_'))) {
	unsigned char *endp = z + 1;
	unsigned char c;
	while ((c = *endp) && ((isalpha(c)) || (c == '_')))
	    endp++;
	*endp = '\0';
	agerr(AGWARN,
	      "%s:%d: ambiguous \"%s\" splits into two names: \"%s\" and \"%s\"\n",
	      InfileName, Line_number, p, token, z);
	*endp = c;
    }

    if (saw_digit == FALSE)
	z = NULL;
    return (char *) z;
}

/* scan a quoted string and return the position after its terminator */
static char *quoted_string(char *p, char *token)
{
    char quote, *q;

    quote = *p++;
    q = token;
    while ((*p) && (*p != quote)) {
	if (*p == '\\') {
	    if (*(p + 1) == quote)
		p++;
	    else {
		if (*(p + 1) == '\\')
		    *q++ = *p++;
	    }
	}
	*q++ = *p++;
    }
    if (*p == '\0')
	agerr(AGWARN, "%s:%d: string ran past end of line\n",
	      InfileName, Line_number);
    else
	p++;
    *q = 0;
    return p;
}

int myaglex(void)
{				/* for debugging */
    int rv = aglex();
    fprintf(stderr, "returning %d\n", rv);
    if (rv == T_symbol)
	fprintf(stderr, "string val is %s\n", aglval.str);
    return rv;
}

/*
 * Return a logical line in AG.linebuf.
 * In particular, the buffer will contain a '\n' as the last non-null char.
 * Ignore lines beginning with '#'; update cpp line number if applicable.
 * Fold long lines, i.e., ignore escaped newlines.
 * Assume the Lexer_gets function reads upto newline or buffer length
 * like fgets.
 * Need to be careful that Lexer_gets might not return full physical line
 * because buffer is too small to hold it.
 */
static char *lex_gets(void)
{
    char *clp;
    int len, curlen;

    len = curlen = 0;

    do {
	/* make sure there is room for at least another SMALLBUF worth */
	if (curlen + SMALLBUF >= LineBufSize) {
	    LineBufSize += BUFSIZ;
	    AG.linebuf = realloc(AG.linebuf, LineBufSize);
	    TokenBuf = realloc(TokenBuf, LineBufSize);
	}

	/* off by one so we can back up in LineBuf */
	clp =
	    (Lexer_gets) (AG.linebuf + curlen + 1,
			  LineBufSize - curlen - 1, Lexer_fp);
	if (clp == NULL)
	    break;


	len = strlen(clp);	/* since clp != NULL, len > 0 */
	if (clp[len - 1] == '\n') {	/* have physical line */
	    if ((clp[0] == '#') && (curlen == 0)) {
		/* comment line or cpp line sync */
		if (sscanf(clp + 1, "%d", &Line_number) == 0)
		    Line_number++;
		clp[0] = 0;
		len = 1;	/* this will make the while test below succeed */
		continue;
	    }
	    Line_number++;
		/* Note it is possible len == 1 and last character in
		 * previous read was '\\'
		 * It is also possible to have curlen=0, and read in
		 * "\\\n". 
		 */
	    if (clp[len - 2] == '\\') {	/* escaped newline */
		len = len - 2;
		clp[len] = '\0';
	    }
	}
	curlen += len;
	/* the following test relies on having AG.linebuf[0] == '\0' */
    } while (clp[len - 1] != '\n');

    if (curlen > 0)
	return AG.linebuf + 1;
    else
	return NULL;
}

/* html_pair:
 * Iteratively scan nested "<...>"
 * p points to first character after initial '<'
 * Store characters up to but not including matching '>'
 * Return pointer to matching '>'
 * We do not check for any escape sequences; pure HTML is
 * expected, so special characters need to be HTML escapes.
 * We read them in and allow the HTML parser to convert them.
 */
static char *html_pair(char *p, agxbuf * tokp)
{
    unsigned char c;
    int rc, depth = 1;

    while (1) {
	while ((c = *p)) {
	    if (c == '>') {
		depth--;
		if (depth == 0)
		    return p;	/* p points to closing > */
	    } else if (c == '<')
		depth++;
	    rc = agxbputc(tokp, c);
	    p++;
	}
	if ((p = lex_gets()) == NULL) {
	    agerr(AGWARN,
		  "non-terminated HTML string starting line %d, file %s\n",
		  Start_html_string, InfileName);
	    return 0;
	}
    }
}

/* html_string:
 * scan an html string and return the position after its terminator 
 * The string is stored in token.
 * p points to the opening <.
 */

static char *html_string(char *p, agxbuf * token)
{
    Start_html_string = Line_number;
    p = html_pair(p + 1, token);
    if (p)
	p++;			/* skip closing '>' */
    return p;
}

int agtoken(char *p)
{
    char ch;
    TFA_Init();
    while ((ch = *p)) {
	/* any non-ascii characters converted to ascii DEL (127) */
	TFA_Advance(ch & ~127 ? 127 : ch);
	p++;
    }
    return TFA_Definition();
}

int aglex(void)
{
    int token;
    char *tbuf, *p;
    static unsigned char BOM[] = { 0xEF, 0xBB, 0xBF };	/* UTF-8 byte order marker */

    /* if the parser has accepted a graph, reset and return EOF */
    if (AG.accepting_state) {
	AG.accepting_state = FALSE;
	return EOF;
    }

    /* get a nonempty lex buffer */
    do {
	if ((LexPtr == NULL) || (LexPtr[0] == '\0'))
	    if ((LexPtr = lex_gets()) == NULL) {
		if (In_comment)
		    agerr(AGWARN, "nonterminated comment in line %d\n",
			  Comment_start);
		return EOF;
	    }
	/* skip UTF-8 Byte Order Marker if at beginning of file */
	if ((Line_number == 1) && !strncmp(LexPtr, (char *) BOM, 3))
	    LexPtr += 3;
	LexPtr = (char *) skip_wscomments(LexPtr);
    } while (LexPtr[0] == '\0');

    tbuf = TokenBuf;

    /* scan quoted strings */
    if (LexPtr[0] == '\"') {
	LexPtr = quoted_string(LexPtr, tbuf);
	aglval.str = agstrdup(tbuf);
	return T_qsymbol;
    }

    /* scan HTML strings */
    if (LexPtr[0] == '<') {
	agxbuf xb;
	unsigned char htmlbuf[BUFSIZ];
	agxbinit(&xb, BUFSIZ, htmlbuf);
	LexPtr = html_string(LexPtr, &xb);
	aglval.str = agstrdup_html(agxbuse(&xb));
	agxbfree(&xb);
	return T_symbol;
    }

    /* scan edge operator */
    if (AG.edge_op
	&& (strncmp(LexPtr, AG.edge_op, strlen(AG.edge_op)) == 0)) {
	LexPtr += strlen(AG.edge_op);
	return T_edgeop;
    }

    /* scan numbers */
    if ((p = scan_num(LexPtr, tbuf))) {
	LexPtr = p;
	aglval.str = agstrdup(tbuf);
	return T_symbol;
    } else {
	unsigned char uc = *(unsigned char *) LexPtr;
	if (ispunct(uc) && (uc != '_'))
	    return *LexPtr++;
	else
	    LexPtr = scan_token(LexPtr, tbuf);
    }

    /* scan other tokens */
    token = agtoken(tbuf);
    if (token == -1) {
	aglval.str = agstrdup(tbuf);
	token = T_symbol;
    }
    return token;
}

static void error_context(void)
{
    char *p;
    char c;
    char *buf = AG.linebuf + 1;	/* characters are always put at AG.linebuf[1] */
    /* or later; AG.linebuf[0] = '\0' */

    if (LexPtr == NULL)
	return;
    agerr(AGPREV, "context: ");
    for (p = LexPtr - 1; (p > buf) && (!isspace(*(unsigned char *) p));
	 p--);
    if (buf < p) {
	c = *p;
	*p = '\0';
	agerr(AGPREV, buf);
	*p = c;
    }
    agerr(AGPREV, " >>> ");
    c = *LexPtr;
    *LexPtr = '\0';
    agerr(AGPREV, p);
    *LexPtr = c;
    agerr(AGPREV, " <<< ");
    agerr(AGPREV, LexPtr);
}

void agerror(char *msg)
{
    if (AG.syntax_errors++)
	return;
    agerr(AGERR, "%s:%d: %s near line %d\n",
	  InfileName, Line_number, msg, Line_number);
    error_context();
}

agerrlevel_t agerrno;		/* Last error */
static agerrlevel_t agerrlevel = AGWARN;	/* Report errors >= agerrlevel */
static long aglast;		/* Last message */
static FILE *agerrout;		/* Message file */

void agseterr(agerrlevel_t lvl)
{
    agerrlevel = lvl;
}

char *aglasterr()
{
    long endpos;
    long len;
    char *buf;

    if (!agerrout)
	return 0;
    fflush(agerrout);
    endpos = ftell(agerrout);
    len = endpos - aglast;
    buf = malloc(len + 1);
    fseek(agerrout, aglast, SEEK_SET);
    fread(buf, sizeof(char), len, agerrout);
    buf[len] = '\0';
    fseek(agerrout, endpos, SEEK_SET);

    return buf;
}

static int agerr_va(agerrlevel_t level, char *fmt, va_list args)
{
    agerrlevel_t lvl;

    lvl = (level == AGPREV ? agerrno : (level == AGMAX) ? AGERR : level);

    agerrno = lvl;
    if (lvl >= agerrlevel) {
	if (level != AGPREV)
	    fprintf(stderr, "%s: ",
		    (level == AGERR) ? "Error" : "Warning");
	vfprintf(stderr, fmt, args);
	va_end(args);
	return 0;
    }

    if (!agerrout) {
	agerrout = tmpfile();
	if (!agerrout)
	    return 1;
    }

    if (level != AGPREV)
	aglast = ftell(agerrout);
    vfprintf(agerrout, fmt, args);
    va_end(args);
    return 0;
}

int agerr(agerrlevel_t level, char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    return agerr_va(level, fmt, args);
}

void agerrorf(char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    agerr_va(AGERR, fmt, args);
}

void agwarningf(char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    agerr_va(AGWARN, fmt, args);
}
