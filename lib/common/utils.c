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
#include "agxbuf.h"
#include "htmltable.h"
#include "entities.h"
#include "logic.h"

#ifndef WIN32
#include <unistd.h>
#endif

/*
 *  a queue of nodes
 */
nodequeue *new_queue(int sz)
{
    nodequeue *q = NEW(nodequeue);

    if (sz <= 1)
	sz = 2;
    q->head = q->tail = q->store = N_NEW(sz, node_t *);
    q->limit = q->store + sz;
    return q;
}

void free_queue(nodequeue * q)
{
    free(q->store);
    free(q);
}

void enqueue(nodequeue * q, node_t * n)
{
    *(q->tail++) = n;
    if (q->tail >= q->limit)
	q->tail = q->store;
}

node_t *dequeue(nodequeue * q)
{
    node_t *n;
    if (q->head == q->tail)
	n = NULL;
    else {
	n = *(q->head++);
	if (q->head >= q->limit)
	    q->head = q->store;
    }
    return n;
}

/* returns index of an attribute if bound, else -1 */
int late_attr(void *obj, char *name)
{
    attrsym_t *a;
    if ((a = agfindattr(obj, name)) != 0)
	return a->index;
    else
	return -1;
}

int late_int(void *obj, attrsym_t * attr, int def, int low)
{
    char *p;
    int rv;
    if (attr == NULL)
	return def;
    p = agxget(obj, attr->index);
    if (p[0] == '\0')
	return def;
    if ((rv = atoi(p)) < low)
	rv = low;
    return rv;
}

double late_double(void *obj, attrsym_t * attr, double def, double low)
{
    char *p;
    double rv;

    if (attr == NULL)
	return def;
    p = agxget(obj, attr->index);
    if (p[0] == '\0')
	return def;
    if ((rv = atof(p)) < low)
	rv = low;
    return rv;
}

char *late_string(void *obj, attrsym_t * attr, char *def)
{
    if (attr == NULL)
	return def;
    return agxget(obj, attr->index);
}

char *late_nnstring(void *obj, attrsym_t * attr, char *def)
{
    char *rv = late_string(obj, attr, def);
    if (rv[0] == '\0')
	rv = def;
    return rv;
}

boolean late_bool(void *obj, attrsym_t * attr, int def)
{
    if (attr == NULL)
	return def;
    return mapbool(agxget(obj, attr->index));
}

/* union-find */
node_t *UF_find(node_t * n)
{
    while (ND_UF_parent(n) && (ND_UF_parent(n) != n)) {
	if (ND_UF_parent(n)->u.UF_parent)
	    ND_UF_parent(n) = ND_UF_parent(n)->u.UF_parent;
	n = ND_UF_parent(n);
    }
    return n;
}

node_t *UF_union(node_t * u, node_t * v)
{
    if (u == v)
	return u;
    if (ND_UF_parent(u) == NULL) {
	ND_UF_parent(u) = u;
	ND_UF_size(u) = 1;
    } else
	u = UF_find(u);
    if (ND_UF_parent(v) == NULL) {
	ND_UF_parent(v) = v;
	ND_UF_size(v) = 1;
    } else
	v = UF_find(v);
    if (u->id > v->id) {
	ND_UF_parent(u) = v;
	ND_UF_size(v) += ND_UF_size(u);
    } else {
	ND_UF_parent(v) = u;
	ND_UF_size(u) += ND_UF_size(v);
	v = u;
    }
    return v;
}

void UF_remove(node_t * u, node_t * v)
{
    assert(ND_UF_size(u) == 1);
    ND_UF_parent(u) = u;
    ND_UF_size(v) -= ND_UF_size(u);
}

void UF_singleton(node_t * u)
{
    ND_UF_size(u) = 1;
    ND_UF_parent(u) = NULL;
    ND_ranktype(u) = NORMAL;
}

void UF_setname(node_t * u, node_t * v)
{
    assert(u == UF_find(u));
    ND_UF_parent(u) = v;
    ND_UF_size(v) += ND_UF_size(u);
}

point coord(node_t * n)
{
    pointf pf;
    pf.x = ND_pos(n)[0];
    pf.y = ND_pos(n)[1];
    return cvt2pt(pf);
}

/* from Glassner's Graphics Gems */
#define W_DEGREE 5

/*
 *  Bezier : 
 *	Evaluate a Bezier curve at a particular parameter value
 *      Fill in control points for resulting sub-curves if "Left" and
 *	"Right" are non-null.
 * 
 */
pointf Bezier(pointf * V, int degree, double t, pointf * Left, pointf * Right)
{
    int i, j;			/* Index variables      */
    pointf Vtemp[W_DEGREE + 1][W_DEGREE + 1];

    /* Copy control points  */
    for (j = 0; j <= degree; j++) {
	Vtemp[0][j] = V[j];
    }

    /* Triangle computation */
    for (i = 1; i <= degree; i++) {
	for (j = 0; j <= degree - i; j++) {
	    Vtemp[i][j].x =
		(1.0 - t) * Vtemp[i - 1][j].x + t * Vtemp[i - 1][j + 1].x;
	    Vtemp[i][j].y =
		(1.0 - t) * Vtemp[i - 1][j].y + t * Vtemp[i - 1][j + 1].y;
	}
    }

    if (Left != NULL)
	for (j = 0; j <= degree; j++)
	    Left[j] = Vtemp[j][0];
    if (Right != NULL)
	for (j = 0; j <= degree; j++)
	    Right[j] = Vtemp[degree - j][j];

    return (Vtemp[degree][0]);
}

#ifdef DEBUG
edge_t *debug_getedge(graph_t * g, char *s0, char *s1)
{
    node_t *n0, *n1;
    n0 = agfindnode(g, s0);
    n1 = agfindnode(g, s1);
    if (n0 && n1)
	return agfindedge(g, n0, n1);
    else
	return NULL;
}
#endif

#if !defined(MSWIN32) && !defined(WIN32)
#include	<pwd.h>
static unsigned char userbuf[SMALLBUF];
static agxbuf xb;

#if 0
static void cleanup(void)
{
    agxbfree(&xb);
}
#endif
#endif

char *gvUsername(void)
{
    char *user = NULL;
#if !defined(MSWIN32) && !defined(WIN32)
    static int first = 1;
    struct passwd *p;
    if (first) {
	agxbinit(&xb, SMALLBUF, userbuf);
#if 0
	atexit(cleanup);
#endif
	first = 0;
    }
    p = (struct passwd *) getpwuid(getuid());
    if (p) {
	agxbputc(&xb, '(');
	agxbput(&xb, p->pw_name);
	agxbput(&xb, ") ");
#ifdef SVR4
	agxbput(&xb, p->pw_comment);
#else
	agxbput(&xb, p->pw_gecos);
#endif
	user = agxbuse(&xb);
    }
#endif
    if (user == NULL)
	user = getenv ("USERNAME");
    if (user == NULL)
	user = "Bill Gates";
    return user;
}

/* Fgets:
 * Read a complete line.
 * Return pointer to line, 
 * or 0 on EOF
 */
char *Fgets(FILE * fp)
{
    static int bsize = 0;
    static char *buf;
    char *lp;
    int len;

    len = 0;
    do {
	if (bsize - len < BUFSIZ) {
	    bsize += BUFSIZ;
	    buf = grealloc(buf, bsize);
	}
	lp = fgets(buf + len, bsize - len, fp);
	if (lp == 0)
	    break;
	len += strlen(lp);	/* since lp != NULL, len > 0 */
    } while (buf[len - 1] != '\n');

    if (len > 0)
	return buf;
    else
	return 0;
}

/* safefile:
 * Check to make sure it is okay to read in files.
 * For normal uses, it is a no-op.
 * If the application has set HTTPServerEnVar, this indicates
 * it is web-active. In this case, it requires that the Gvfilepath
 * variable be set. This gives the legal directory
 * from which files may be read. The filename returned is thus
 * Gvfilepath concatenated with the last component of filename,
 * where a component is determined by a slack, backslash or colon
 * character.  
 * If the argument contains one of these characters, the user is
 * warned, once, that everything to the left is ignored.
 * As coded now, Gvfilepath must end in a slash character, or
 * equivalent.
 * Returns NULL if the argument is trivial.
 */
char *safefile(const char *filename)
{
    static boolean onetime = TRUE;
    static char *safefilename = NULL;
    const char *str, *p;

    if (!filename || !filename[0])
	return NULL;
    if (HTTPServerEnVar) {
	/* 
	 * If we are running in an http server we allow
	 * files only from the directory specified in
	 * the GV_FILE_PATH environment variable.
	 */
	if (!Gvfilepath) {
	    if (onetime) {
		agerr(AGWARN,
		      "file loading is disabled because the environment contains: %s\n"
		      "and there is no GV_FILE_PATH variable.\n",
		      HTTPServerEnVar);
		onetime = FALSE;
	    }
	    return NULL;
	}

	/* allocate a buffer that we are sure is big enough
         * +1 for null character.
         */
	safefilename = realloc(safefilename,
			       (strlen(Gvfilepath) + strlen(filename) + 1));

	strcpy(safefilename, Gvfilepath);
	str = filename;
	if ((p = strrchr(str, '/')))
	    str = ++p;
	if ((p = strrchr(str, '\\')))
	    str = ++p;
	if ((p = strrchr(str, ':')))
	    str = ++p;
	strcat(safefilename, str);

	if (onetime && str != filename) {
	    agerr(AGWARN, "Path provided to file: \"%s\" has been ignored"
		  " because files are only permitted to be loaded from the \"%s\""
		  " directory when running in an http server.\n", filename,
		  Gvfilepath);
	    onetime = FALSE;
	}

	return safefilename;
    }
    /* else, not in server, use original filename without modification. */
    return filename;
}

int maptoken(char *p, char **name, int *val)
{
    int i;
    char *q;

    for (i = 0; (q = name[i]) != 0; i++)
	if (p && streq(p, q))
	    break;
    return val[i];
}

boolean mapbool(char *p)
{
    if (p == NULL)
	return FALSE;
    if (!strcasecmp(p, "false"))
	return FALSE;
    if (!strcasecmp(p, "true"))
	return TRUE;
    return atoi(p);
}

pointf dotneato_closest(splines * spl, pointf pt)
{
    int i, j, k, besti, bestj;
    double bestdist2, d2, dlow2, dhigh2; /* squares of distances */
    double low, high, t;
    pointf c[4], pt2;
    bezier bz;

    besti = bestj = -1;
    bestdist2 = 1e+38;
    for (i = 0; i < spl->size; i++) {
	bz = spl->list[i];
	for (j = 0; j < bz.size; j++) {
	    pointf b;

	    b.x = bz.list[j].x;
	    b.y = bz.list[j].y;
	    d2 = DIST2(b, pt);
	    if ((bestj == -1) || (d2 < bestdist2)) {
		besti = i;
		bestj = j;
		bestdist2 = d2;
	    }
	}
    }

    bz = spl->list[besti];
    j = bestj / 3;
    if (j >= spl->size)
	j--;
    for (k = 0; k < 4; k++) {
	c[k].x = bz.list[j + k].x;
	c[k].y = bz.list[j + k].y;
    }
    low = 0.0;
    high = 1.0;
    dlow2 = DIST2(c[0], pt);
    dhigh2 = DIST2(c[3], pt);
    do {
	t = (low + high) / 2.0;
	pt2 = Bezier(c, 3, t, NULL, NULL);
	if (fabs(dlow2 - dhigh2) < 1.0)
	    break;
	if (fabs(high - low) < .00001)
	    break;
	if (dlow2 < dhigh2) {
	    high = t;
	    dhigh2 = DIST2(pt2, pt);
	} else {
	    low = t;
	    dlow2 = DIST2(pt2, pt);
	}
    } while (1);
    return pt2;
}

pointf spline_at_y(splines * spl, double y)
{
    int i, j;
    double low, high, d, t;
    pointf c[4], p;
    static bezier bz;

/* this caching seems to prevent p.x from getting set from bz.list[0].x
	- optimizer problem ? */

#if 0
    static splines *mem = NULL;

    if (mem != spl) {
	mem = spl;
#endif
	for (i = 0; i < spl->size; i++) {
	    bz = spl->list[i];
	    if (BETWEEN(bz.list[bz.size - 1].y, y, bz.list[0].y))
		break;
	}
#if 0
    }
#endif
    if (y > bz.list[0].y)
	p = bz.list[0];
    else if (y < bz.list[bz.size - 1].y)
	p = bz.list[bz.size - 1];
    else {
	for (i = 0; i < bz.size; i += 3) {
	    for (j = 0; j < 3; j++) {
		if ((bz.list[i + j].y <= y) && (y <= bz.list[i + j + 1].y))
		    break;
		if ((bz.list[i + j].y >= y) && (y >= bz.list[i + j + 1].y))
		    break;
	    }
	    if (j < 3)
		break;
	}
	assert(i < bz.size);
	for (j = 0; j < 4; j++) {
	    c[j].x = bz.list[i + j].x;
	    c[j].y = bz.list[i + j].y;
	    /* make the spline be monotonic in Y, awful but it works for now */
	    if ((j > 0) && (c[j].y > c[j - 1].y))
		c[j].y = c[j - 1].y;
	}
	low = 0.0;
	high = 1.0;
	do {
	    t = (low + high) / 2.0;
	    p = Bezier(c, 3, t, NULL, NULL);
	    d = p.y - y;
	    if (ABS(d) <= 1)
		break;
	    if (d < 0)
		high = t;
	    else
		low = t;
	} while (1);
    }
    p.y = y;
    return p;
}

pointf neato_closest(splines * spl, pointf p)
{
/* this is a stub so that we can share a common emit.c between dot and neato */

    return spline_at_y(spl, p.y);
}

static int Tflag;
void gvToggle(int s)
{
    Tflag = !Tflag;
#if !defined(MSWIN32) && !defined(WIN32)
    signal(SIGUSR1, gvToggle);
#endif
}

int test_toggle()
{
    return Tflag;
}

void common_init_node(node_t * n)
{
    char *str;

    ND_width(n) =
	late_double(n, N_width, DEFAULT_NODEWIDTH, MIN_NODEWIDTH);
    ND_height(n) =
	late_double(n, N_height, DEFAULT_NODEHEIGHT, MIN_NODEHEIGHT);
    ND_shape(n) =
	bind_shape(late_nnstring(n, N_shape, DEFAULT_NODESHAPE), n);
    if (N_label == NULL)
	str = NODENAME_ESC;
    else 
	str = agxget(n, N_label->index);
    ND_label(n) = make_label((void*)n, str,
	        ((aghtmlstr(str) ? LT_HTML : LT_NONE) | ( (shapeOf(n) == SH_RECORD) ? LT_RECD : LT_NONE)),
		late_double(n, N_fontsize, DEFAULT_FONTSIZE, MIN_FONTSIZE),
		late_nnstring(n, N_fontname, DEFAULT_FONTNAME),
		late_nnstring(n, N_fontcolor, DEFAULT_COLOR));
    ND_showboxes(n) = late_int(n, N_showboxes, 0, 0);
    ND_shape(n)->fns->initfn(n);
}

struct fontinfo {
    double fontsize;
    char *fontname;
    char *fontcolor;
};

static void initFontEdgeAttr(edge_t * e, struct fontinfo *fi)
{
    fi->fontsize = late_double(e, E_fontsize, DEFAULT_FONTSIZE, MIN_FONTSIZE);
    fi->fontname = late_nnstring(e, E_fontname, DEFAULT_FONTNAME);
    fi->fontcolor = late_nnstring(e, E_fontcolor, DEFAULT_COLOR);
}

static void
initFontLabelEdgeAttr(edge_t * e, struct fontinfo *fi,
		      struct fontinfo *lfi)
{
    if (!fi->fontname) initFontEdgeAttr(e, fi);
    lfi->fontsize = late_double(e, E_labelfontsize, fi->fontsize, MIN_FONTSIZE);
    lfi->fontname = late_nnstring(e, E_labelfontname, fi->fontname);
    lfi->fontcolor = late_nnstring(e, E_labelfontcolor, fi->fontcolor);
}

/* noClip:
 * Return true if head/tail end of edge should not be clipped
 * to node.
 */
static boolean 
noClip(edge_t *e, attrsym_t* sym)
{
    char		*str;
    boolean		rv = FALSE;

    if (sym) {	/* mapbool isn't a good fit, because we want "" to mean true */
	str = agxget(e,sym->index);
	if (str && str[0]) rv = !mapbool(str);
	else rv = FALSE;
    }
    return rv;
}

/*chkPort:
 */
static port
chkPort (port (*pf)(node_t*, char*, char*), node_t* n, char* s)
{ 
    port pt;
    char* cp = strchr(s,':');

    if (cp) {
	*cp = '\0';
	pt = pf(n, s, cp+1);
	*cp = ':';
    }
    else
	pt = pf(n, s, NULL);
    return pt;
}

/* return true if edge has label */
int common_init_edge(edge_t * e)
{
    char *str;
    int r = 0;
    struct fontinfo fi;
    struct fontinfo lfi;
    graph_t *sg = e->tail->graph;

    fi.fontname = NULL;
    lfi.fontname = NULL;
    if (E_label && (str = agxget(e, E_label->index)) && (str[0])) {
	r = 1;
	initFontEdgeAttr(e, &fi);
	ED_label(e) = make_label((void*)e, str, (aghtmlstr(str) ? LT_HTML : LT_NONE),
				fi.fontsize, fi.fontname, fi.fontcolor);
	GD_has_labels(sg) |= EDGE_LABEL;
	ED_label_ontop(e) =
	    mapbool(late_string(e, E_label_float, "false"));
    }


    /* vladimir */
    if (E_headlabel && (str = agxget(e, E_headlabel->index)) && (str[0])) {
	initFontLabelEdgeAttr(e, &fi, &lfi);
	ED_head_label(e) = make_label((void*)e, str, (aghtmlstr(str) ? LT_HTML : LT_NONE),
				lfi.fontsize, lfi.fontname, lfi.fontcolor);
	GD_has_labels(sg) |= HEAD_LABEL;
    }
    if (E_taillabel && (str = agxget(e, E_taillabel->index)) && (str[0])) {
	if (!lfi.fontname)
	    initFontLabelEdgeAttr(e, &fi, &lfi);
	ED_tail_label(e) = make_label((void*)e, str, (aghtmlstr(str) ? LT_HTML : LT_NONE),
				lfi.fontsize, lfi.fontname, lfi.fontcolor);
	GD_has_labels(sg) |= TAIL_LABEL;
    }
    /* end vladimir */

    /* We still accept ports beginning with colons but this is deprecated */
    str = agget(e, TAIL_ID);
    if (str[0])
	ND_has_port(e->tail) = TRUE;
    ED_tail_port(e) = chkPort (ND_shape(e->tail)->fns->portfn,e->tail, str);
    if (noClip(e, E_tailclip))
	ED_tail_port(e).clip = FALSE;
    str = agget(e, HEAD_ID);
    if (str[0])
	ND_has_port(e->head) = TRUE;
    ED_head_port(e) = chkPort(ND_shape(e->head)->fns->portfn,e->head, str);
    if (noClip(e, E_headclip))
	ED_head_port(e).clip = FALSE;

    return r;
}

/* addLabelBB:
 */
static boxf addLabelBB(boxf bb, textlabel_t * lp, boolean flipxy)
{
    double width, height;
    pointf p = lp->pos;
    double min, max;

    if (flipxy) {
	height = lp->dimen.x;
	width = lp->dimen.y;
    }
    else {
	width = lp->dimen.x;
	height = lp->dimen.y;
    }
    min = p.x - width / 2.;
    max = p.x + width / 2.;
    if (min < bb.LL.x)
	bb.LL.x = min;
    if (max > bb.UR.x)
	bb.UR.x = max;

    min = p.y - height / 2.;
    max = p.y + height / 2.;
    if (min < bb.LL.y)
	bb.LL.y = min;
    if (max > bb.UR.y)
	bb.UR.y = max;

    return bb;
}

/* updateBB:
 * Reset graph's bounding box to include bounding box of the given label.
 * Assume the label's position has been set.
 */
void updateBB(graph_t * g, textlabel_t * lp)
{
    GD_bb(g) = addLabelBB(GD_bb(g), lp, GD_flip(g));
}

/* compute_bb:
 * Compute bounding box of g using nodes, splines, and clusters.
 * Assumes bb of clusters already computed.
 * store in GD_bb.
 */
void compute_bb(graph_t * g)
{
    node_t *n;
    edge_t *e;
    boxf b, bb;
    boxf BF;
    pointf ptf, s2;
    int i, j;

    if ((agnnodes(g) == 0) && (GD_n_cluster(g) ==0)) {
	bb.LL = pointfof(0, 0);
	bb.UR = pointfof(0, 0);
	return;
    }

    bb.LL = pointfof(INT_MAX, INT_MAX);
    bb.UR = pointfof(-INT_MAX, -INT_MAX);
    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	P2PF(coord(n), ptf);
	s2.x = ND_xsize(n) / 2. + 1;
	s2.y = ND_ysize(n) / 2. + 1;
	b.LL = sub_pointf(ptf, s2);
	b.UR = add_pointf(ptf, s2);

	EXPANDBB(bb,b);
	for (e = agfstout(g, n); e; e = agnxtout(g, e)) {
	    if (ED_spl(e) == 0)
		continue;
	    for (i = 0; i < ED_spl(e)->size; i++) {
		for (j = 0; j < ED_spl(e)->list[i].size; j++) {
		    ptf = ED_spl(e)->list[i].list[j];
		    EXPANDBP(bb,ptf);
		}
	    }
	    if (ED_label(e) && ED_label(e)->set) {
		bb = addLabelBB(bb, ED_label(e), GD_flip(g));
	    }
	}
    }

    for (i = 1; i <= GD_n_cluster(g); i++) {
	B2BF(GD_clust(g)[i]->u.bb, BF);
	EXPANDBB(bb,BF);
    }

    GD_bb(g) = bb;
}

/* setAttr:
 * Sets object's name attribute to the given value.
 * Creates the attribute if not already set.
 */
static Agsym_t *setAttr(graph_t * g, void *obj, char *name, char *value,
			Agsym_t * ap)
{
    if (ap == NULL) {
	switch (agobjkind(obj)) {
	case AGGRAPH:
	    ap = agraphattr(g, name, "");
	    break;
	case AGNODE:
	    ap = agnodeattr(g, name, "");
	    break;
	case AGEDGE:
	    ap = agedgeattr(g, name, "");
	    break;
	}
    }
    agxset(obj, ap->index, value);
    return ap;
}

/* clustNode:
 * Generate a special cluster node representing the end node
 * of an edge to the cluster cg. n is a node whose name is the same
 * as the cluster cg. clg is the subgraph of all of
 * the original nodes, which will be deleted later.
 */
static node_t *clustNode(node_t * n, graph_t * cg, agxbuf * xb,
			 graph_t * clg)
{
    node_t *cn;
    static int idx = 0;
    char num[100];

    agxbput(xb, "__");
    sprintf(num, "%d", idx++);
    agxbput(xb, num);
    agxbputc(xb, ':');
    agxbput(xb, cg->name);

    cn = agnode(cg->root, agxbuse(xb));
    SET_CLUST_NODE(cn);
    aginsert(cg, cn);
    aginsert(clg, n);

    /* set attributes */
    N_label = setAttr(cn->graph, cn, "label", "", N_label);
    N_style = setAttr(cn->graph, cn, "style", "invis", N_style);
    N_shape = setAttr(cn->graph, cn, "shape", "box", N_shape);
    /* N_width = setAttr (cn->graph, cn, "width", "0.0001", N_width); */

    return cn;
}

typedef struct {
    Dtlink_t link;		/* cdt data */
    void *p[2];			/* key */
    node_t *t;
    node_t *h;
} item;

static int cmpItem(Dt_t * d, void *p1[], void *p2[], Dtdisc_t * disc)
{
    NOTUSED(d);
    NOTUSED(disc);

    if (p1[0] < p2[0])
	return -1;
    else if (p1[0] > p2[0])
	return 1;
    else if (p1[1] < p2[1])
	return -1;
    else if (p1[1] > p2[1])
	return 1;
    else
	return 0;
}

/* newItem:
 */
static void *newItem(Dt_t * d, item * objp, Dtdisc_t * disc)
{
    item *newp = NEW(item);

    NOTUSED(disc);
    newp->p[0] = objp->p[0];
    newp->p[1] = objp->p[1];
    newp->t = objp->t;
    newp->h = objp->h;

    return newp;
}

/* freeItem:
 */
static void freeItem(Dt_t * d, item * obj, Dtdisc_t * disc)
{
    free(obj);
}

static Dtdisc_t mapDisc = {
    offsetof(item, p),
    sizeof(2 * sizeof(void *)),
    offsetof(item, link),
    (Dtmake_f) newItem,
    (Dtfree_f) freeItem,
    (Dtcompar_f) cmpItem,
    NIL(Dthash_f),
    NIL(Dtmemory_f),
    NIL(Dtevent_f)
};

/* cloneEdge:
 * Make a copy of e in e's graph but using ct and ch as nodes
 */
static edge_t *cloneEdge(edge_t * e, node_t * ct, node_t * ch)
{
    graph_t *g = ct->graph;
    edge_t *ce = agedge(g, ct, ch);
    agcopyattr(e, ce);

    return ce;
}

/* insertEdge:
 */
static void insertEdge(Dt_t * map, void *t, void *h, edge_t * e)
{
    item dummy;

    dummy.p[0] = t;
    dummy.p[1] = h;
    dummy.t = e->tail;
    dummy.h = e->head;
    dtinsert(map, &dummy);

    dummy.p[0] = h;
    dummy.p[1] = t;
    dummy.t = e->head;
    dummy.h = e->tail;
    dtinsert(map, &dummy);
}

/* mapEdge:
 * Check if we already have cluster edge corresponding to t->h,
 * and return it.
 */
static item *mapEdge(Dt_t * map, edge_t * e)
{
    void *key[2];

    key[0] = e->tail;
    key[1] = e->head;
    return (item *) dtmatch(map, &key);
}

/* checkCompound:
 * If endpoint names a cluster, mark for temporary deletion and create 
 * special node and insert into cluster. Then clone the edge. Real edge
 * will be deleted when we delete the original node.
 * Invariant: new edge has same sense as old. That is, given t->h with
 * t and h mapped to ct and ch, the new edge is ct->ch.
 *
 * In the current model, we create a cluster node for each cluster edge
 * between the cluster and some other node or cluster, treating the
 * cluster node as a port on the cluster. This should help with better
 * routing to avoid edge crossings. At present, this is not implemented,
 * so we could use a simpler model in which we create a single cluster
 * node for each cluster used in a cluster edge.
 */
#define MAPC(n) (strncmp((n)->name,"cluster",7)?NULL:agfindsubg((n)->graph, (n)->name))

static void
checkCompound(edge_t * e, graph_t * clg, agxbuf * xb, Dt_t * map)
{
    graph_t *tg;
    graph_t *hg;
    node_t *cn;
    node_t *cn1;
    node_t *t = e->tail;
    node_t *h = e->head;
    edge_t *ce;
    item *ip;

    if (IS_CLUST_NODE(h)) return;
    tg = MAPC(t);
    hg = MAPC(h);
    if (!tg && !hg)
	return;
    if (tg == hg) {
	agerr(AGWARN, "cluster cycle %s -- %s not supported\n", t->name,
	      t->name);
	return;
    }
    ip = mapEdge(map, e);
    if (ip) {
	cloneEdge(e, ip->t, ip->h);
	return;
    }

    if (hg) {
	if (tg) {
	    if (agcontains(hg, tg)) {
		agerr(AGWARN, "tail cluster %s inside head cluster %s\n",
		      tg->name, hg->name);
		return;
	    }
	    if (agcontains(tg, hg)) {
		agerr(AGWARN, "head cluster %s inside tail cluster %s\n",
		      hg->name, tg->name);
		return;
	    }
	    cn = clustNode(t, tg, xb, clg);
	    cn1 = clustNode(h, hg, xb, clg);
	    ce = cloneEdge(e, cn, cn1);
	    insertEdge(map, t, h, ce);
	} else {
	    if (agcontains(hg, t)) {
		agerr(AGWARN, "tail node %s inside head cluster %s\n",
		      t->name, hg->name);
		return;
	    }
	    cn = clustNode(h, hg, xb, clg);
	    ce = cloneEdge(e, t, cn);
	    insertEdge(map, t, h, ce);
	}
    } else {
	if (agcontains(tg, h)) {
	    agerr(AGWARN, "head node %s inside tail cluster %s\n", h->name,
		  tg->name);
	    return;
	}
	cn = clustNode(t, tg, xb, clg);
	ce = cloneEdge(e, cn, h);
	insertEdge(map, t, h, ce);
    }
}

/* processClusterEdges:
 * Look for cluster edges. Replace cluster edge endpoints
 * corresponding to a cluster with special cluster nodes.
 * Delete original nodes.
 * Return 0 if no cluster edges; 1 otherwise.
 */
int processClusterEdges(graph_t * g)
{
    int rv;
    node_t *n;
    node_t *nxt;
    edge_t *e;
    graph_t *clg;
    agxbuf xb;
    Dt_t *map;
    unsigned char buf[SMALLBUF];

    map = dtopen(&mapDisc, Dtoset);
    clg = agsubg(g, "__clusternodes");
    agxbinit(&xb, SMALLBUF, buf);
    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	if (IS_CLUST_NODE(n)) continue;
	for (e = agfstout(g, n); e; e = agnxtout(g, e)) {
	    checkCompound(e, clg, &xb, map);
	}
    }
    agxbfree(&xb);
    dtclose(map);
    rv = agnnodes(clg);
    for (n = agfstnode(clg); n; n = nxt) {
	nxt = agnxtnode(clg, n);
	agdelete(g, n);
    }
    agclose(clg);
    if (rv)
	SET_CLUST_EDGE(g);
    return rv;
}

/* mapN:
 * Convert cluster nodes back to ordinary nodes
 * If n is already ordinary, return it.
 * Otherwise, we know node's name is "__i:xxx"
 * where i is some number and xxx is the nodes's original name.
 * Create new node of name xxx if it doesn't exist and add n to clg
 * for later deletion.
 */
static node_t *mapN(node_t * n, graph_t * clg)
{
    extern Agdict_t *agdictof(void *);
    node_t *nn;
    char *name;
    graph_t *g = n->graph;
    Agdict_t *d;
    Agsym_t **list;
    Agsym_t *sym;

    if (!(IS_CLUST_NODE(n)))
	return n;
    aginsert(clg, n);

    name = strchr(n->name, ':');
    assert(name);
    name++;
    if ((nn = agfindnode(g, name)))
	return nn;
    nn = agnode(g, name);

    /* Set all attributes to default */
    d = agdictof(n);
    list = d->list;
    while ((sym = *list++)) {
	/* Can use pointer comparison because of ref strings. */
	if (agxget(nn, sym->index) != sym->value)
	    agxset(nn, sym->index, sym->value);
    }

    return nn;
}

static void undoCompound(edge_t * e, graph_t * clg)
{
    node_t *t = e->tail;
    node_t *h = e->head;
    node_t *ntail;
    node_t *nhead;

    if (!(IS_CLUST_NODE(t) || IS_CLUST_NODE(h)))
	return;
    ntail = mapN(t, clg);
    nhead = mapN(h, clg);
    cloneEdge(e, ntail, nhead);
}

/* undoClusterEdges:
 * Replace cluster nodes with originals. Make sure original has
 * no attributes. Replace original edges. Delete cluster nodes,
 * which will also delete cluster edges.
 */
void undoClusterEdges(graph_t * g)
{
    node_t *n;
    edge_t *e;
    graph_t *clg;

    clg = agsubg(g, "__clusternodes");
    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	for (e = agfstout(g, n); e; e = agnxtout(g, e)) {
	    undoCompound(e, clg);
	}
    }
    for (n = agfstnode(clg); n; n = agnxtnode(clg, n)) {
	agdelete(g, n);
    }
    agclose(clg);
}

/* safe_dcl:
 * Find the attribute belonging to graph g for objects like obj
 * with given name. If one does not exist, create it with the
 * supplied function fun with default value def.
 */ 
attrsym_t*
safe_dcl(graph_t * g, void *obj, char *name, char *def,
         attrsym_t * (*fun) (Agraph_t *, char *, char *))
{
    attrsym_t *a = agfindattr(obj, name);
    if (a == NULL)
	a = fun(g, name, def);
    return a;
}

static int comp_entities(const void *e1, const void *e2) {
  return strcmp(((struct entities_s *)e1)->name, ((struct entities_s *)e2)->name);
}

#define MAXENTLEN 8

/* scanEntity:
 * Scan non-numeric entity, convert to &#...; form and store in xbuf.
 * t points to first char after '&'. Return after final semicolon.
 * If unknown, we return t and let libexpat flag the error.
 *     */
char* scanEntity (char* t, agxbuf* xb)
{
    char*  endp = strchr (t, ';');
    struct entities_s key, *res;
    int    len;
    char   buf[MAXENTLEN+1];

    agxbputc(xb, '&');
    if (!endp) return t;
    if (((len = endp-t) > MAXENTLEN) || (len < 2)) return t;
    strncpy (buf, t, len);
    buf[len] = '\0';
    key.name =  buf;
    res = bsearch(&key, entities, NR_OF_ENTITIES,
        sizeof(entities[0]), comp_entities);
    if (!res) return t;
    sprintf (buf, "%d", res->value);
    agxbputc(xb, '#');
    agxbput(xb, buf);
    agxbputc(xb, ';');
    return (endp+1);
}


/* htmlEntity:
 * Check for an HTML entity for a special character.
 * Assume *s points to first byte after '&'. 
 * If successful, return the corresponding value and update s to
 * point after the terminating ';'.
 * On failure, return 0 and leave s unchanged.
 */
static int
htmlEntity (char** s)
{
    char *p;
    struct entities_s key, *res;
    char entity_name_buf[ENTITY_NAME_LENGTH_MAX+1];
    unsigned char* str = *(unsigned char**)s;
    unsigned int byte;
    int i, n = 0;

    byte = *str;
    if (byte == '#') {
	byte = *(str + 1);
	if (byte == 'x' || byte == 'X') {
	    for (i = 2; i < 8; i++) {
		byte = *(str + i);
		if (byte >= 'A' && byte <= 'F')
                    byte = byte - 'A' + 10;
		else if (byte >= 'a' && byte <= 'f')
                    byte = byte - 'a' + 10;
		else if (byte >= '0' && byte <= '9')
                    byte = byte - '0';
		else
                    break;
		n = (n * 16) + byte;
	    }
	}
	else {
	    for (i = 1; i < 8; i++) {
		byte = *(str + i);
		if (byte >= '0' && byte <= '9')
		    n = (n * 10) + (byte - '0');
		else
		    break;
	    }
	}
	if (byte == ';') {
	    str += i+1;
	}
	else {
	    n = 0;
	}
    }
    else {
	key.name = p = entity_name_buf;
	for (i = 0; i <  ENTITY_NAME_LENGTH_MAX; i++) {
	    byte = *(str + i);
	    if (byte == '\0') break;
	    if (byte == ';') {
		*p++ = '\0';
		res = bsearch(&key, entities, NR_OF_ENTITIES,
		    sizeof(entities[0]), *comp_entities);
		if (res) {
		    n = res->value;
		    str += i+1;
		}
		break;
	    }
	    *p++ = byte;
	}
    }
    *s = (char*)str;
    return n;
}

/* substitute html entities like: &#123; and: &amp; with the UTF8 equivalents */
char* htmlEntityUTF8 (char* s)
{
    char*  ns;
    agxbuf xb;
    unsigned char buf[BUFSIZ];
    unsigned char c;
    unsigned int v;
    int rc;

    agxbinit(&xb, BUFSIZ, buf);

    while ((c = *(unsigned char*)s++)) {
        if (c < 0xC0) {
	    /*
	     * Handles properly formed UTF-8 characters between
	     * 0x01 and 0x7F.  Also treats \0 and naked trail
	     * bytes 0x80 to 0xBF as valid characters representing
	     * themselves.
	     */
	    if (c == '&') {
		/* replace html entity sequences like: &amp;
		 * and: &#123; with their UTF8 equivalents */
	        v = htmlEntity (&s);
	        if (v) {
		    if (v < 0x7F) /* entity needs 1 byte in UTF8 */
			c = v;
		    else if (v < 0x07FF) { /* entity needs 2 bytes in UTF8 */
			rc = agxbputc(&xb, (v >> 6) | 0xC0);
			c = (v & 0x3F) | 0x80;
		    }
		    else { /* entity needs 3 bytes in UTF8 */
			rc = agxbputc(&xb, (v >> 12) | 0xE0);
			rc = agxbputc(&xb, ((v >> 6) & 0x3F) | 0x80);
			c = (v & 0x3F) | 0x80;
		    }
		}
            }
	}
        else if (c < 0xE0) { /* copy 2 byte UTF8 characters */
	    if ((*s & 0xC0) == 0x80) {
	        rc = agxbputc(&xb, c);
	        c = *(unsigned char*)s++;
	    }
	    else {
		agerr(AGERR, "Invalid 2-byte UTF8 found in input. Perhaps \"-Gcharset=latin1\" is needed?\n");
		exit(EXIT_FAILURE);
	    }
	}
	else if (c < 0xF0) { /* copy 3 byte UTF8 characters */
	    if (((*s & 0xC0) == 0x80) && ((s[1] & 0xC0) == 0x80)) {
	        rc = agxbputc(&xb, c);
	        c = *(unsigned char*)s++;
	        rc = agxbputc(&xb, c);
	        c = *(unsigned char*)s++;
	    }
	    else {
		agerr(AGERR, "Invalid 3-byte UTF8 found in input. Perhaps \"-Gcharset=latin1\" is needed?\n");
		exit(EXIT_FAILURE);
	    }
	}
	else  {
	    agerr(AGERR, "UTF8 codes > 3 bytes are not currently supported. Or perhaps \"-Gcharset=latin1\" is needed?\n");
	    exit(EXIT_FAILURE);
        }
	rc = agxbputc(&xb, c);
    }
    ns = strdup (agxbuse(&xb));
    agxbfree(&xb);
    return ns;
}

/* latin1ToUTF8:
 * Converts string from Latin1 encoding to utf8
 * Also translates HTML entities.
 *
 */
char* latin1ToUTF8 (char* s)
{
    char*  ns;
    agxbuf xb;
    unsigned char buf[BUFSIZ];
    unsigned int  v;
    int rc;
    
    agxbinit(&xb, BUFSIZ, buf);

    /* Values are either a byte (<= 256) or come from htmlEntity, whose
     * values are all less than 0x07FF, so we need at most 3 bytes.
     */
    while ((v = *(unsigned char*)s++)) {
	if (v == '&') {
	    v = htmlEntity (&s);
	    if (!v) v = '&';
        }
	if (v < 0x7F)
	    rc = agxbputc(&xb, v);
	else if (v < 0x07FF) {
	    rc = agxbputc(&xb, (v >> 6) | 0xC0);
	    rc = agxbputc(&xb, (v & 0x3F) | 0x80);
	}
	else {
	    rc = agxbputc(&xb, (v >> 12) | 0xE0);
	    rc = agxbputc(&xb, ((v >> 6) & 0x3F) | 0x80);
	    rc = agxbputc(&xb, (v & 0x3F) | 0x80);
	}
    }
    ns = strdup (agxbuse(&xb));
    agxbfree(&xb);
    return ns;
}

/* utf8ToLatin1:
 * Converts string from utf8 encoding to Latin1
 * Note that it does not attempt to reproduce HTML entities.
 * We assume the input string comes from latin1ToUTF8.
 */
char*
utf8ToLatin1 (char* s)
{
    char*  ns;
    agxbuf xb;
    unsigned char buf[BUFSIZ];
    unsigned char c;
    unsigned char outc;
    int rc;
    
    agxbinit(&xb, BUFSIZ, buf);

    while ((c = *(unsigned char*)s++)) {
	if (c < 0x7F)
	    rc = agxbputc(&xb, c);
	else {
	    outc = (c & 0x03) << 6;
	    c = *(unsigned char*)s++;
	    outc = outc | (c & 0x3F);
	    rc = agxbputc(&xb, outc);
	}
    }
    ns = strdup (agxbuse(&xb));
    agxbfree(&xb);
    return ns;
}

boolean overlap_node(node_t *n, boxf b)
{
    inside_t ictxt;
    pointf p;

    if (! OVERLAP(b, ND_bb(n)))
        return FALSE;

/*  FIXME - need to do something better about CLOSEENOUGH */
    p = sub_pointf(ND_coord(n), mid_pointf(b.UR, b.LL));

    ictxt.s.n = n;
    ictxt.s.bp = NULL;

    return ND_shape(n)->fns->insidefn(&ictxt, p);
}

boolean overlap_label(textlabel_t *lp, boxf b)
{
    pointf s;
    boxf bb;

    s.x = lp->dimen.x / 2.;
    s.y = lp->dimen.y / 2.;
    bb.LL = sub_pointf(lp->pos, s);
    bb.UR = add_pointf(lp->pos, s);
    return OVERLAP(b, bb);
}

static boolean overlap_arrow(pointf p, pointf u, double scale, int flag, boxf b)
{
    if (OVERLAP(b, arrow_bb(p, u, scale, flag))) {
	/* FIXME - check inside arrow shape */
	return TRUE;
    }
    return FALSE;
}

static boolean overlap_bezier(bezier bz, boxf b)
{
    int i;
    pointf p, u;

    assert(bz.size);
    u = bz.list[0];
    for (i = 1; i < bz.size; i++) {
	p = bz.list[i];
	if (lineToBox(p, u, b) != -1)
	    return TRUE;
	u = p;
    }

    /* check arrows */
    if (bz.sflag) {
	if (overlap_arrow(bz.sp, bz.list[0], 1, bz.sflag, b))
	    return TRUE;
    }
    if (bz.eflag) {
	if (overlap_arrow(bz.ep, bz.list[bz.size - 1], 1, bz.eflag, b))
	    return TRUE;
    }
    return FALSE;
}

boolean overlap_edge(edge_t *e, boxf b)
{
    int i;
    splines *spl;
    textlabel_t *lp;

    spl = ED_spl(e);
    if (spl && boxf_overlap(spl->bb, b))
        for (i = 0; i < spl->size; i++)
            if (overlap_bezier(spl->list[i], b))
                return TRUE;

    lp = ED_label(e);
    if (lp && overlap_label(lp, b))
        return TRUE;

    return FALSE;
}

/* setEdgeType:
 * Sets graph's edge type based on the "splines" attribute.
 * If the attribute is not defined, use default.
 * If the attribute is "", use NONE.
 * If attribute value matches (case indepedent), use match.
 *   ortho => ET_ORTHO
 *   none => ET_NONE
 *   line => ET_LINE
 *   polyline => ET_PLINE
 *   spline => ET_SPLINE
 * If attribute is boolean, true means ET_SPLINE, false means ET_LINE.
 * Else warn and use default.
 */
void setEdgeType (graph_t* g, int dflt)
{
    char* s = agget(g, "splines");
    int et;

    if (!s) {
	GD_flags(g) |= dflt;
	return;
    }
    if (*s == '\0') {
	et = ET_NONE;
	return;
    }
    et = 0;
    switch (*s) {
    case '0' :    /* false */
	et = ET_LINE;
	break;
    case '1' :    /* true */
    case '2' :
    case '3' :
    case '4' :
    case '5' :
    case '6' :
    case '7' :
    case '8' :
    case '9' :
	et = ET_SPLINE;
	break;
    case 'c' :
    case 'C' :
	if (!strcasecmp (s+1, "ompound"))
	    et = ET_COMPOUND;
	break;
    case 'f' :
    case 'F' :
	if (!strcasecmp (s+1, "alse"))
	    et = ET_LINE;
	break;
    case 'l' :
    case 'L' :
	if (!strcasecmp (s+1, "ine"))
	    et = ET_LINE;
	break;
    case 'n' :
    case 'N' :
	if (!strcasecmp (s+1, "one")) return;
	break;
    case 'o' :
    case 'O' :
	if (!strcasecmp (s+1, "rtho"))
	    et = ET_ORTHO;
	break;
    case 'p' :
    case 'P' :
	if (!strcasecmp (s+1, "olyline"))
	    et = ET_PLINE;
	break;
    case 's' :
    case 'S' :
	if (!strcasecmp (s+1, "pline"))
	    et = ET_SPLINE;
	break;
    case 't' :
    case 'T' :
	if (!strcasecmp (s+1, "rue"))
	    et = ET_SPLINE;
	break;
    }
    if (!et) {
	agerr(AGWARN, "Unknown \"splines\" value: \"%s\" - ignored\n", s);
	et = dflt;
    }
    GD_flags(g) |= et;
}

#ifndef HAVE_STRCASECMP

#include <string.h>
#include <ctype.h>


int strcasecmp(const char *s1, const char *s2)
{
    while ((*s1 != '\0')
	   && (tolower(*(unsigned char *) s1) ==
	       tolower(*(unsigned char *) s2))) {
	s1++;
	s2++;
    }

    return tolower(*(unsigned char *) s1) - tolower(*(unsigned char *) s2);
}

#endif				/* HAVE_STRCASECMP */

#ifndef HAVE_STRNCASECMP

#include <string.h>
#include <ctype.h>

int strncasecmp(const char *s1, const char *s2, unsigned int n)
{
    if (n == 0)
	return 0;

    while ((n-- != 0)
	   && (tolower(*(unsigned char *) s1) ==
	       tolower(*(unsigned char *) s2))) {
	if (n == 0 || *s1 == '\0' || *s2 == '\0')
	    return 0;
	s1++;
	s2++;
    }

    return tolower(*(unsigned char *) s1) - tolower(*(unsigned char *) s2);
}

#endif				/* HAVE_STRNCASECMP */

void gv_free_splines(edge_t * e)
{
    int i;
    if (ED_spl(e)) {
        for (i = 0; i < ED_spl(e)->size; i++)
            free(ED_spl(e)->list[i].list);
        free(ED_spl(e)->list);
        free(ED_spl(e));
    }
    ED_spl(e) = NULL;
}

void gv_cleanup_edge(edge_t * e)
{
    gv_free_splines(e);
    free_label(ED_label(e));
    memset(&(e->u), 0, sizeof(Agedgeinfo_t));
}

void gv_cleanup_node(node_t * n)
{
    if (ND_pos(n)) free(ND_pos(n));
    if (ND_shape(n))
        ND_shape(n)->fns->freefn(n);
    free_label(ND_label(n));
    memset(&(n->u), 0, sizeof(Agnodeinfo_t));
}

void gv_nodesize(node_t * n, boolean flip)
{
    int w;

    w = ND_xsize(n) = POINTS(ND_width(n));
    ND_lw(n) = ND_rw(n) = w / 2;
    ND_ht(n) = ND_ysize(n) = POINTS(ND_height(n));
}

