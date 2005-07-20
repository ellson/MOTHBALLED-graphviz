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

%{

#include "gvc.h"
#include "htmltable.h"
#include "htmllex.h"

extern int yyparse(void);

static struct {
  htmllabel_t* lbl;       /* Generated label */
  htmltbl_t*   tblstack;  /* Stack of tables maintained during parsing */
  Dt_t*        lines;     /* Dictionary for lines of text */
  agxbuf*      str;       /* Buffer for text */
} HTMLstate;

/* free_ritem:
 * Free row. This closes and frees row's list, then
 * the pitem itself is freed.
 */
static void
free_ritem(Dt_t* d, pitem* p,Dtdisc_t* ds)
{
  dtclose (p->u.rp);
  free (p);
}

/* free_ritem:
 * Free cell item after cell has been copies into final table.
 * Only the pitem is freed.
 */
static void
free_item(Dt_t* d, pitem* p,Dtdisc_t* ds)
{
  free (p);
}

/* cleanTbl:
 * Clean up table if error in parsing.
 */
static void
cleanTbl (htmltbl_t* tp)
{
  dtclose (tp->u.p.rows);
  free_html_data (&tp->data);
  free (tp);
}

/* cleanCell:
 * Clean up cell if error in parsing.
 */
static void
cleanCell (htmlcell_t* cp)
{
  if (cp->child.kind == HTML_TBL) cleanTbl (cp->child.u.tbl);
  else if (cp->child.kind == HTML_TEXT) free_html_text (cp->child.u.txt);
  free_html_data (&cp->data);
  free (cp);
}

/* free_citem:
 * Free cell item during parsing. This frees cell and pitem.
 */
static void
free_citem(Dt_t* d, pitem* p,Dtdisc_t* ds)
{
  cleanCell (p->u.cp);
  free (p);
}

static Dtdisc_t rowDisc = {
    offsetof(pitem,u),
    sizeof(void*),
    offsetof(pitem,link),
    NIL(Dtmake_f),
    (Dtfree_f)free_ritem,
    NIL(Dtcompar_f),
    NIL(Dthash_f),
    NIL(Dtmemory_f),
    NIL(Dtevent_f)
};
static Dtdisc_t cellDisc = {
    offsetof(pitem,u),
    sizeof(void*),
    offsetof(pitem,link),
    NIL(Dtmake_f),
    (Dtfree_f)free_item,
    NIL(Dtcompar_f),
    NIL(Dthash_f),
    NIL(Dtmemory_f),
    NIL(Dtevent_f)
};

typedef struct {
  Dtlink_t      link;
  const char*   s;          /* line of text */
  char          c;          /* alignment of text */
} sitem;

static void
free_sitem(Dt_t* d,sitem* p,Dtdisc_t* ds)
{
  free (p);
}

static Dtdisc_t strDisc = {
    offsetof(sitem,s),
    sizeof(char*),
    offsetof(sitem,link),
    NIL(Dtmake_f),
    (Dtfree_f)free_sitem,
    NIL(Dtcompar_f),
    NIL(Dthash_f),
    NIL(Dtmemory_f),
    NIL(Dtevent_f)
};

static void
appendStrList(const char* p,int v)
{
  sitem*  sp = NEW(sitem);
  sp->s = strdup(p);
  sp->c = v;
  dtinsert (HTMLstate.lines, sp);
}

/* mkText:
 * Construct htmltxt_t from list of lines in HTMLstate.lines.
 * lastl is a last, odd line with no <BR>, so we use n by default.
 */
static htmltxt_t*
mkText (const char* lastl)
{
  int         cnt;
  textline_t* lp;
  sitem*      sp;
  Dt_t*       lines = HTMLstate.lines;
  htmltxt_t* tp = NEW(htmltxt_t);

  if (lines)
    cnt = dtsize (lines);
  else
    cnt = 0;
  if (lastl) cnt++;

  tp->nlines = cnt;
  tp->line = N_NEW(cnt+1,textline_t);

  lp = tp->line;
  if (lines) {
    sp = (sitem*)dtflatten(lines);
    for (; sp; sp = (sitem*)dtlink(lines,(Dtlink_t*)sp)) {
      lp->str = (char*)(sp->s);
      lp->xshow = NULL;
      lp->just = sp->c;
      lp++;
    }
  }
  if (lastl) {
    lp->str = strdup(lastl);
    lp->just = 'n';
  }

  if (lines) dtclear (lines);

  return tp;
}

/* addRow:
 * Add new cell row to current table.
 */
static void addRow (void)
{
  Dt_t*      dp = dtopen(&cellDisc, Dtqueue);
  htmltbl_t* tbl = HTMLstate.tblstack;
  pitem*     sp = NEW(pitem);

  sp->u.rp = dp;
  dtinsert (tbl->u.p.rows, sp);
}

/* setCell:
 * Set cell body and type and attach to row
 */
static void setCell (htmlcell_t* cp, void* obj, int kind)
{
  pitem*     sp = NEW(pitem);
  htmltbl_t* tbl = HTMLstate.tblstack;
  pitem*     rp = (pitem*)dtlast (tbl->u.p.rows);
  Dt_t*      row = rp->u.rp;

  sp->u.cp = cp;
  dtinsert (row, sp);
  cp->child.kind = kind;
  if (kind == HTML_TEXT)
    cp->child.u.txt = (htmltxt_t*)obj;
  else if (kind == HTML_IMAGE)
    cp->child.u.img = (htmlimg_t*)obj;
  else
    cp->child.u.tbl = (htmltbl_t*)obj;
}

/* setFont:
 * Copy in font attributes. fp has the new attributes.
 * curf corresponds to the current font info of the object.
 * From the parser, we are moving out from the object. Since
 * the inmost value is the one used, we only use a new value
 * if the attribute has not already been assigned.
 */
static htmlfont_t* setFont (htmlfont_t* fp, htmlfont_t*  curf)
{
  if (curf) {
    if (curf->size < 0.0) curf->size = fp->size;
    if (!curf->color) curf->color = fp->color;
    else if (fp->color) free (fp->color);
    if (!curf->name) curf->name = fp->name;
    else if (fp->name) free (fp->name);
    free (fp);
    return curf;
  }
  else
    return fp;
}


/* fontText:
 * Attach font information to text.
 */
static void fontText (htmlfont_t* fp, htmltxt_t* cp)
{
  cp->font = setFont (fp, cp->font);
}

/* fontTable:
 * Attach font information to table.
 */
static void fontTable (htmlfont_t* fp, htmltbl_t* cp)
{
  cp->font = setFont (fp, cp->font);
}

/* mkLabel:
 * Create label, given body and type.
 */
static htmllabel_t* mkLabel (void* obj, int kind)
{
  htmllabel_t* lp = NEW(htmllabel_t);

  lp->kind = kind;
  if (kind == HTML_TEXT)
    lp->u.txt = (htmltxt_t*)obj;
  else
    lp->u.tbl = (htmltbl_t*)obj;
    
  return lp;
}

/* cleanup:
 * Called on error. Frees resources allocated during parsing.
 * This includes a label, plus a walk down the stack of
 * tables. Note that we use the free_citem function to actually
 * free cells.
 */
static void cleanup (void)
{
  htmltbl_t* tp = HTMLstate.tblstack;
  htmltbl_t* next;

  if (HTMLstate.lbl) {
    free_html_label (HTMLstate.lbl,1);
    HTMLstate.lbl = NULL;
  }
  cellDisc.freef = (Dtfree_f)free_citem;
  while (tp) {
    next = tp->u.p.prev;
    cleanTbl (tp);
    tp = next;
  }
  cellDisc.freef = (Dtfree_f)free_item;
}

/* nonSpace:
 * Return 1 if s contains a non-space character.
 */
static int nonSpace (char* s)
{
  char   c;

  while ((c = *s++)) {
    if (c != ' ') return 1;
  }
  return 0;
}

%}

%union  {
  int    i;
  htmltxt_t*  txt;
  htmlcell_t*  cell;
  htmltbl_t*   tbl;
  htmlfont_t*  font;
  htmlimg_t*   img;
}

%token T_end_br T_end_img T_row T_end_row T_html T_end_html
%token T_end_table T_end_cell T_end_font T_string T_error
%token <i> T_BR T_br
%token <img> T_IMG T_img
%token <tbl> T_table
%token <cell> T_cell
%token <font> T_font

%type <txt> text fonttext
%type <i> br
%type <tbl> table fonttable
%type <img> image

%start html

%%

html  : T_html fonttext { HTMLstate.lbl = mkLabel($2,HTML_TEXT); } T_end_html
      | T_html fonttable { HTMLstate.lbl = mkLabel($2,HTML_TBL); } T_end_html
      | error { cleanup(); YYABORT; }
      ;

fonttext : text { $$ = $1; }
         | T_font text T_end_font { fontText($1,$2); $$ = $2; }
         ;

text : lines
        { $$ = mkText (NULL); }
     | lines string
        { $$ = mkText (agxbuse(HTMLstate.str)); }
     | string
        { $$ = mkText (agxbuse(HTMLstate.str)); }
     |  /* empty */
        { $$ = mkText (NULL); }
     ;

lines : string br
        { appendStrList (agxbuse(HTMLstate.str),$2); }
      | lines string br
        { appendStrList (agxbuse(HTMLstate.str), $3); }
      ;

br     : T_br T_end_br { $$ = $1; }
       | T_BR { $$ = $1; }
       ;

string : T_string
       | string T_string
       ;

table : opt_space T_table { 
          if (nonSpace(agxbuse(HTMLstate.str))) {
            yyerror ("Syntax error: non-space string used before <TABLE>");
            cleanup(); YYABORT;
          }
          $2->u.p.prev = HTMLstate.tblstack;
          $2->u.p.rows = dtopen(&rowDisc, Dtqueue);
          HTMLstate.tblstack = $2;
          $$ = $2;
        }
        rows T_end_table opt_space {
          if (nonSpace(agxbuse(HTMLstate.str))) {
            yyerror ("Syntax error: non-space string used after </TABLE>");
            cleanup(); YYABORT;
          }
          $$ = HTMLstate.tblstack;
          HTMLstate.tblstack = HTMLstate.tblstack->u.p.prev;
        }
      ;

fonttable : table { $$ = $1; }
          | T_font table T_end_font { fontTable($1,$2); $$ = $2; }
          ;

opt_space : string 
          | /* empty*/
          ;

rows : row
     | rows row
     ;

row : T_row { addRow (); } cells T_end_row
      ;

cells : cell
      | cells cell
      ;

cell : T_cell fonttable { setCell($1,$2,HTML_TBL); } T_end_cell
     | T_cell fonttext { setCell($1,$2,HTML_TEXT); } T_end_cell
     | T_cell image { setCell($1,$2,HTML_IMAGE); } T_end_cell
     ;

image  : T_img T_end_img { $$ = $1; }
       | T_IMG { $$ = $1; }
       ;


%%

htmllabel_t*
simpleHTML (char* txt)
{
  htmltxt_t*   tobj = mkText (txt);
  htmllabel_t* l = mkLabel(tobj,HTML_TEXT);
  return l;
}

/* parseHTML:
 * Return parsed label or NULL if failure.
 * Set warn to 0 on success; 1 for warning message; 2 if no expat.
 */
htmllabel_t*
parseHTML (char* txt, int* warn, int charset)
{
  unsigned char buf[SMALLBUF];
  agxbuf        str;
  htmllabel_t*  l;

  HTMLstate.tblstack = 0;
  HTMLstate.lbl = 0;
  HTMLstate.lines = dtopen(&strDisc, Dtqueue);
  agxbinit (&str, SMALLBUF, buf);
  HTMLstate.str = &str;
  
  if (initHTMLlexer (txt, &str, charset)) {/* failed: no libexpat - give up */
    *warn = 2;
    l = NULL;
  }
  else {
    yyparse();
    *warn = clearHTMLlexer ();
    l = HTMLstate.lbl;
  }

  dtclose (HTMLstate.lines);
  HTMLstate.lines = NULL;
  agxbfree (&str);

  return l;
}

