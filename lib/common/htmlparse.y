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

%{

#include "render.h"
#include "htmltable.h"
#include "htmllex.h"

extern int yyparse(void);

typedef struct sfont_t {
    textfont_t *cfont;	
    struct sfont_t *pfont;
} sfont_t;

static struct {
  htmllabel_t* lbl;       /* Generated label */
  htmltbl_t*   tblstack;  /* Stack of tables maintained during parsing */
  Dt_t*        fitemList; /* Dictionary for font text items */
  Dt_t*        fspanList; 
  agxbuf*      str;       /* Buffer for text */
  sfont_t*     fontstack;
  GVC_t*       gvc;
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

/* free_item:
 * Generic Dt free. Only frees container, assuming contents
 * have been copied elsewhere.
 */
static void
free_item(Dt_t* d, void* p,Dtdisc_t* ds)
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
    Dtlink_t    link;
    textspan_t  ti;
} fitem;

typedef struct {
    Dtlink_t     link;
    htextspan_t  lp;
} fspan;

static void 
free_fitem(Dt_t* d, fitem* p, Dtdisc_t* ds)
{
    if (p->ti.str)
	free (p->ti.str);
    free (p);
}

static void 
free_fspan(Dt_t* d, fspan* p, Dtdisc_t* ds)
{
    textspan_t* ti;

    if (p->lp.nitems) {
	int i;
	ti = p->lp.items;
	for (i = 0; i < p->lp.nitems; i++) {
	    if (ti->str) free (ti->str);
	    ti++;
	}
	free (p->lp.items);
    }
    free (p);
}

static Dtdisc_t fstrDisc = {
    0,
    0,
    offsetof(fitem,link),
    NIL(Dtmake_f),
    (Dtfree_f)free_item,
    NIL(Dtcompar_f),
    NIL(Dthash_f),
    NIL(Dtmemory_f),
    NIL(Dtevent_f)
};


static Dtdisc_t fspanDisc = {
    0,
    0,
    offsetof(fspan,link),
    NIL(Dtmake_f),
    (Dtfree_f)free_item,
    NIL(Dtcompar_f),
    NIL(Dthash_f),
    NIL(Dtmemory_f),
    NIL(Dtevent_f)
};

/* appendFItemList:
 * Append a new fitem to the list.
 */
static void
appendFItemList (agxbuf *ag)
{
    fitem *fi = NEW(fitem);

    fi->ti.str = strdup(agxbuse(ag));
    fi->ti.font = HTMLstate.fontstack->cfont;
    dtinsert(HTMLstate.fitemList, fi);
}	

/* appendFLineList:
 */
static void 
appendFLineList (int v)
{
    int cnt;
    fspan *ln = NEW(fspan);
    fitem *fi;
    Dt_t *ilist = HTMLstate.fitemList;

    cnt = dtsize(ilist);
    ln->lp.just = v;
    if (cnt) {
        int i = 0;
	ln->lp.nitems = cnt;
	ln->lp.items = N_NEW(cnt, textspan_t);

	fi = (fitem*)dtflatten(ilist);
	for (; fi; fi = (fitem*)dtlink(fitemList,(Dtlink_t*)fi)) {
		/* NOTE: When fitemList is closed, it uses free_item, which only frees the container,
		 * not the contents, so this copy is safe.
		 */
	    ln->lp.items[i] = fi->ti;  
	    i++;
	}
    }
    else {
	ln->lp.items = NEW(textspan_t);
	ln->lp.nitems = 1;
	ln->lp.items[0].str = strdup("");
	ln->lp.items[0].font = HTMLstate.fontstack->cfont;
    }

    dtclear(ilist);

    dtinsert(HTMLstate.fspanList, ln);
}

static htmltxt_t*
mkText(void)
{
    int cnt;
    Dt_t * ispan = HTMLstate.fspanList;
    fspan *fl ;
    htmltxt_t *hft = NEW(htmltxt_t);
    
    if (dtsize (HTMLstate.fitemList)) 
	appendFLineList (UNSET_ALIGN);

    cnt = dtsize(ispan);
    hft->nspans = cnt;
    	
    if (cnt) {
	int i = 0;
	hft->spans = N_NEW(cnt,htextspan_t);	
    	for(fl=(fspan *)dtfirst(ispan); fl; fl=(fspan *)dtnext(ispan,fl)) {
    	    hft->spans[i] = fl->lp;
    	    i++;
    	}
    }
    
    dtclear(ispan);

    return hft;
}

static pitem* lastRow (void)
{
  htmltbl_t* tbl = HTMLstate.tblstack;
  pitem*     sp = dtlast (tbl->u.p.rows);
  return sp;
}

/* addRow:
 * Add new cell row to current table.
 */
static pitem* addRow (void)
{
  Dt_t*      dp = dtopen(&cellDisc, Dtqueue);
  htmltbl_t* tbl = HTMLstate.tblstack;
  pitem*     sp = NEW(pitem);
  sp->u.rp = dp;
  if (tbl->flags & HTML_HRULE)
    sp->ruled = 1;
  dtinsert (tbl->u.p.rows, sp);
  return sp;
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
  if (tbl->flags & HTML_VRULE)
    cp->ruled = HTML_VRULE;
  
  if(kind == HTML_TEXT)
  	cp->child.u.txt = (htmltxt_t*)obj;
  else if (kind == HTML_IMAGE)
    cp->child.u.img = (htmlimg_t*)obj;
  else
    cp->child.u.tbl = (htmltbl_t*)obj;
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

/* freeFontstack:
 * Free all stack items but the last, which is
 * put on artificially during in parseHTML.
 */
static void
freeFontstack(void)
{
    sfont_t* s;
    sfont_t* next;

    for (s = HTMLstate.fontstack; (next = s->pfont); s = next) {
	free(s);
    }
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

  fstrDisc.freef = (Dtfree_f)free_fitem;
  dtclear (HTMLstate.fitemList);
  fstrDisc.freef = (Dtfree_f)free_item;

  fspanDisc.freef = (Dtfree_f)free_fspan;
  dtclear (HTMLstate.fspanList);
  fspanDisc.freef = (Dtfree_f)free_item;

  freeFontstack();
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

/* pushFont:
 * Fonts are allocated in the lexer.
 */
static void
pushFont (textfont_t *fp)
{
    sfont_t *ft = NEW(sfont_t);
    textfont_t* curfont = HTMLstate.fontstack->cfont;
    textfont_t  f = *fp;

    if (curfont) {
	if (!f.color && curfont->color)
	    f.color = curfont->color;
	if ((f.size < 0.0) && (curfont->size >= 0.0))
	    f.size = curfont->size;
	if (!f.name && curfont->name)
	    f.name = curfont->name;
	if (curfont->flags)
	    f.flags |= curfont->flags;
    }

    ft->cfont = dtinsert(HTMLstate.gvc->textfont_dt, &f);
    ft->pfont = HTMLstate.fontstack;
    HTMLstate.fontstack = ft;
}

/* popFont:
 */
static void 
popFont (void)
{
    sfont_t* curfont = HTMLstate.fontstack;
    sfont_t* prevfont = curfont->pfont;

    free (curfont);
    HTMLstate.fontstack = prevfont;
}

%}

%union  {
  int    i;
  htmltxt_t*  txt;
  htmlcell_t*  cell;
  htmltbl_t*   tbl;
  textfont_t*  font;
  htmlimg_t*   img;
  pitem*       p;
}

%token T_end_br T_end_img T_row T_end_row T_html T_end_html
%token T_end_table T_end_cell T_end_font T_string T_error
%token T_n_italic T_n_bold T_n_underline  T_n_overline T_n_sup T_n_sub T_n_s
%token T_HR T_hr T_end_hr
%token T_VR T_vr T_end_vr
%token <i> T_BR T_br
%token <img> T_IMG T_img
%token <tbl> T_table
%token <cell> T_cell
%token <font> T_font T_italic T_bold T_underline T_overline T_sup T_sub T_s

%type <txt> fonttext
%type <cell> cell cells
%type <i> br  
%type <tbl> table fonttable
%type <img> image
%type <p> row rows

%start html
             
%%

html  : T_html fonttext T_end_html { HTMLstate.lbl = mkLabel($2,HTML_TEXT); }
      | T_html fonttable T_end_html { HTMLstate.lbl = mkLabel($2,HTML_TBL); }
      | error { cleanup(); YYABORT; }
      ;

fonttext : text { $$ = mkText(); }
      ;

text : text textitem  
     | textitem 
     ;

textitem : string { appendFItemList(HTMLstate.str);}
         | br {appendFLineList($1);}
         | font text n_font
         | italic text n_italic
         | underline text n_underline
         | overline text n_overline
         | bold text n_bold
         | sup text n_sup
         | sub text n_sub
         | strike text n_strike
         ;

font : T_font { pushFont ($1); }
      ;

n_font : T_end_font { popFont (); }
      ;

italic : T_italic {pushFont($1);}
          ;

n_italic : T_n_italic {popFont();}
            ;

bold : T_bold {pushFont($1);}
          ;

n_bold : T_n_bold {popFont();}
            ;

strike : T_s {pushFont($1);}
          ;

n_strike : T_n_s {popFont();}
            ;

underline : T_underline {pushFont($1);}
          ;

n_underline : T_n_underline {popFont();}
            ;

overline : T_overline {pushFont($1);}
          ;

n_overline : T_n_overline {popFont();}
            ;

sup : T_sup {pushFont($1);}
          ;

n_sup : T_n_sup {popFont();}
            ;

sub : T_sub {pushFont($1);}
          ;

n_sub : T_n_sub {popFont();}
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
          $2->font = HTMLstate.fontstack->cfont;
          $<tbl>$ = $2;
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
          | font table n_font { $$=$2; }
          | italic table n_italic { $$=$2; }
          | underline table n_underline { $$=$2; }
          | overline table n_overline { $$=$2; }
          | bold table n_bold { $$=$2; }
          ;

opt_space : string 
          | /* empty*/
          ;

rows : row { $$ = $1; }
     | rows row { $$ = $2; }
     | rows HR row { $1->ruled = 1; $$ = $3; }
     ;

row : T_row { addRow (); } cells T_end_row { $$ = lastRow(); }
      ;

cells : cell { $$ = $1; }
      | cells cell { $$ = $2; }
      | cells VR cell { $1->ruled |= HTML_VRULE; $$ = $3; }
      ;

cell : T_cell fonttable { setCell($1,$2,HTML_TBL); } T_end_cell { $$ = $1; }
     | T_cell fonttext { setCell($1,$2,HTML_TEXT); } T_end_cell { $$ = $1; }
     | T_cell image { setCell($1,$2,HTML_IMAGE); } T_end_cell { $$ = $1; }
     | T_cell { setCell($1,mkText(),HTML_TEXT); } T_end_cell { $$ = $1; }
     ;

image  : T_img T_end_img { $$ = $1; }
       | T_IMG { $$ = $1; }
       ;

HR  : T_hr T_end_hr
    | T_HR
    ;

VR  : T_vr T_end_vr
    | T_VR
    ;


%%

/* parseHTML:
 * Return parsed label or NULL if failure.
 * Set warn to 0 on success; 1 for warning message; 2 if no expat.
 */
htmllabel_t*
parseHTML (char* txt, int* warn, htmlenv_t *env)
{
  unsigned char buf[SMALLBUF];
  agxbuf        str;
  htmllabel_t*  l;
  sfont_t       dfltf;

  dfltf.cfont = NULL;
  dfltf.pfont = NULL;
  HTMLstate.fontstack = &dfltf;
  HTMLstate.tblstack = 0;
  HTMLstate.lbl = 0;
  HTMLstate.gvc = GD_gvc(env->g);
  HTMLstate.fitemList = dtopen(&fstrDisc, Dtqueue);
  HTMLstate.fspanList = dtopen(&fspanDisc, Dtqueue);

  agxbinit (&str, SMALLBUF, buf);
  HTMLstate.str = &str;
  
  if (initHTMLlexer (txt, &str, env)) {/* failed: no libexpat - give up */
    *warn = 2;
    l = NULL;
  }
  else {
    yyparse();
    *warn = clearHTMLlexer ();
    l = HTMLstate.lbl;
  }

  dtclose (HTMLstate.fitemList);
  dtclose (HTMLstate.fspanList);
  
  HTMLstate.fitemList = NULL;
  HTMLstate.fspanList = NULL;
  HTMLstate.fontstack = NULL;
  
  agxbfree (&str);

  return l;
}

