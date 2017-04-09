/* $Id$ $Revision$ */
/* vim:set shiftwidth=4 ts=8: */

/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: See CVS logs. Details at http://www.graphviz.org/
 *************************************************************************/


/* requires flex (i.e. not lex)  */
%{
#include <grammar.h>
#include <cghdr.h>
#include <agxbuf.h>
#include <ctype.h>
// #define YY_BUF_SIZE 128000
#define GRAPH_EOF_TOKEN		'@'		/* lex class must be defined below */
	/* this is a workaround for linux flex */
static int line_num = 1;
static int html_nest = 0;  /* nesting level for html strings */
static char* InputFile;
static Agdisc_t	*Disc;
static void 	*Ifile;
static int graphType;

  /* Reset line number */
void agreadline(int n) { line_num = n; }

  /* (Re)set file:
   */
void agsetfile(char* f) { InputFile = f; line_num = 1; }

/* There is a hole here, because switching channels 
 * requires pushing back whatever was previously read.
 * There probably is a right way of doing this.
 */
void aglexinit(Agdisc_t *disc, void *ifile) { Disc = disc; Ifile = ifile; graphType = 0;}

#define isatty(x) 0
#ifndef YY_INPUT
#define YY_INPUT(buf,result,max_size) \
	if ((result = Disc->io->afread(Ifile, buf, max_size)) < 0) \
		YY_FATAL_ERROR( "input in flex scanner failed" )
#endif

/* buffer for arbitrary length strings (longer than BUFSIZ) */
static char	*Sbuf,*Sptr,*Send;
static void beginstr(void) {
	if (Sbuf == NIL(char*)) {
		Sbuf = malloc(BUFSIZ);
		Send = Sbuf + BUFSIZ;
	}
	Sptr = Sbuf;
	*Sptr = 0;
}

static void addstr(char *src) {
	char	c;
	if (Sptr > Sbuf) Sptr--;
	do {
		do {c = *Sptr++ = *src++;} while (c && (Sptr < Send));
		if (c) {
			long	sz = Send - Sbuf;
			long	off = Sptr - Sbuf;
			sz *= 2;
			Sbuf = (char*)realloc(Sbuf,sz);
			Send = Sbuf + sz;
			Sptr = Sbuf + off;
		}
	} while (c);
}

static void endstr(void) {
	yylval.str = (char*)agstrdup(Ag_G_global,Sbuf);
	*Sbuf = 0;
}

static void endstr_html(void) {
	yylval.str = (char*)agstrdup_html(Ag_G_global,Sbuf);
	*Sbuf = 0;
}

static void
storeFileName (char* fname, int len)
{
    static int cnt;
    static char* buf;

    if (len > cnt) {
	if (cnt) buf = (char*)realloc (buf, len+1);
	else buf = (char*)malloc (len+1);
	cnt = len;
    }
    strcpy (buf, fname);
    InputFile = buf;
}

/* ppDirective:
 * Process a possible preprocessor line directive.
 * yytext = #.*
 */
static void ppDirective (void)
{
    int r, cnt, lineno;
    char buf[2];
    char* s = yytext + 1;  /* skip initial # */

    if (strncmp(s, "line", 4) == 0) s += 4;
    r = sscanf(s, "%d %1[\"]%n", &lineno, buf, &cnt);
    if (r > 0) { /* got line number */ 
	line_num = lineno - 1;
	if (r > 1) { /* saw quote */
	    char* p = s + cnt;
	    char* e = p;
	    while (*e && (*e != '"')) e++; 
	    if (e != p && *e == '"') {
 		*e = '\0';
		storeFileName (p, e-p);
	    }
	}
    }
}

/* twoDots:
 * Return true if token has more than one '.';
 * we know the last character is a '.'.
 */
static int twoDots(void)
{
    int i;
    for (i = yyleng-2; i >= 0; i--) {
	if (((unsigned char)yytext[i]) == '.')
	    return 1;
    }
    return 0;
}

/* chkNum:
 * The regexp for NUMBER allows a terminating letter or '.'.
 * This way we can catch a number immediately followed by a name
 * or something like 123.456.78, and report this to the user.
 */
static int chkNum(void) {
    unsigned char c = (unsigned char)yytext[yyleng-1];   /* last character */
    if ((!isdigit(c) && (c != '.')) || ((c == '.') && twoDots())) {  /* c is letter */
	unsigned char xbuf[BUFSIZ];
	char buf[BUFSIZ];
	agxbuf  xb;
	char* fname;

	if (InputFile)
	    fname = InputFile;
	else
	    fname = "input";

	agxbinit(&xb, BUFSIZ, xbuf);

	agxbput(&xb,"syntax ambiguity - badly delimited number '");
	agxbput(&xb,yytext);
	sprintf(buf,"' in line %d of ", line_num);
	agxbput(&xb,buf);
	agxbput(&xb,fname);
	agxbput(&xb, " splits into two tokens\n");
	agerr(AGWARN, "%s", agxbuse(&xb));

	agxbfree(&xb);
	return 1;
    }
    else return 0;
}

/* The LETTER class below consists of ascii letters, underscore, all non-ascii
 * characters. This allows identifiers to have characters from any
 * character set independent of locale. The downside is that, for certain
 * character sets, non-letter and, in fact, undefined characters will be
 * accepted. This is not likely and, from dot's stand, shouldn't do any
 * harm. (Presumably undefined characters will be ignored in display.) And,
 * it allows a greater wealth of names. */
%}
GRAPH_EOF_TOKEN				[@]	
LETTER [A-Za-z_\200-\377]
DIGIT	[0-9]
NAME	{LETTER}({LETTER}|{DIGIT})*
NUMBER	[-]?(({DIGIT}+(\.{DIGIT}*)?)|(\.{DIGIT}+))(\.|{LETTER})?
ID		({NAME}|{NUMBER})
%x comment
%x qstring
%x hstring
%%
{GRAPH_EOF_TOKEN}		return(EOF);
<INITIAL,comment,qstring>\n	line_num++;
"/*"					BEGIN(comment);
<comment>[^*\n]*		/* eat anything not a '*' */
<comment>"*"+[^*/\n]*	/* eat up '*'s not followed by '/'s */
<comment>"*"+"/"		BEGIN(INITIAL);
"//".*					/* ignore C++-style comments */
^"#".*					ppDirective ();
"#".*					/* ignore shell-like comments */
[ \t\r]					/* ignore whitespace */
"\xEF\xBB\xBF"				/* ignore BOM */
"node"					return(T_node);			/* see tokens in agcanonstr */
"edge"					return(T_edge);
"graph"					if (!graphType) graphType = T_graph; return(T_graph);
"digraph"				if (!graphType) graphType = T_digraph; return(T_digraph);
"strict"				return(T_strict);
"subgraph"				return(T_subgraph);
"->"				if (graphType == T_digraph) return(T_edgeop); else return('-');
"--"				if (graphType == T_graph) return(T_edgeop); else return('-');
{NAME}					{ yylval.str = (char*)agstrdup(Ag_G_global,yytext); return(T_atom); }
{NUMBER}				{ if (chkNum()) yyless(yyleng-1); yylval.str = (char*)agstrdup(Ag_G_global,yytext); return(T_atom); }
["]						BEGIN(qstring); beginstr();
<qstring>["]			BEGIN(INITIAL); endstr(); return (T_qatom);
<qstring>[\\]["]		addstr ("\"");
<qstring>[\\][\\]		addstr ("\\\\");
<qstring>[\\][\n]		line_num++; /* ignore escaped newlines */
<qstring>([^"\\]*|[\\])		addstr(yytext);
[<]						BEGIN(hstring); html_nest = 1; beginstr();
<hstring>[>]			html_nest--; if (html_nest) addstr(yytext); else {BEGIN(INITIAL); endstr_html(); return (T_qatom);}
<hstring>[<]			html_nest++; addstr(yytext);
<hstring>[\n]			addstr(yytext); line_num++; /* add newlines */
<hstring>([^><\n]*)		addstr(yytext);
.						return (yytext[0]);
%%
 
void yyerror(char *str)
{
	unsigned char	xbuf[BUFSIZ];
	char	buf[BUFSIZ];
	agxbuf  xb;

	agxbinit(&xb, BUFSIZ, xbuf);
	if (InputFile) {
		agxbput (&xb, InputFile);
		agxbput (&xb, ": ");
	}
	agxbput (&xb, str);
	sprintf(buf," in line %d", line_num);
	agxbput (&xb, buf);
	if (*yytext) {
		agxbput(&xb," near '");
		agxbput (&xb, yytext);
		agxbputc (&xb, '\'');
	}
	else switch (YYSTATE) {
	case qstring :
		sprintf(buf, " scanning a quoted string (missing endquote? longer than %d?)", YY_BUF_SIZE);
		agxbput (&xb, buf);
		if (*Sbuf) {
			int len = strlen(Sbuf);
			agxbput (&xb, "\nString starting:\"");
			if (len > 80)
				Sbuf[80] = '\0';
			agxbput (&xb, Sbuf);
		}
		break;
	case hstring :
		sprintf(buf, " scanning a HTML string (missing '>'? bad nesting? longer than %d?)", YY_BUF_SIZE);
		agxbput (&xb, buf);
		if (*Sbuf) {
			int len = strlen(Sbuf);
			agxbput (&xb, "\nString starting:<");
			if (len > 80)
				Sbuf[80] = '\0';
			agxbput (&xb, Sbuf);
		}
		break;
	case comment :
		sprintf(buf, " scanning a /*...*/ comment (missing '*/? longer than %d?)", YY_BUF_SIZE);
		agxbput (&xb, buf);
		break;
	}
	agxbputc (&xb, '\n');
	agerr(AGERR, "%s", agxbuse(&xb));
	agxbfree(&xb);
    BEGIN(INITIAL);
}
/* must be here to see flex's macro defns */
void aglexeof() { unput(GRAPH_EOF_TOKEN); }

void aglexbad() { YY_FLUSH_BUFFER; }

#ifndef YY_CALL_ONLY_ARG
# define YY_CALL_ONLY_ARG void
#endif

int yywrap(YY_CALL_ONLY_ARG)
{
	return 1;
}

