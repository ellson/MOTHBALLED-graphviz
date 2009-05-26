#include <stdio.h>
#include <stdlib.h>
#include <patchwork.h>
#include "render.h"

extern void patchwork_init_graph(graph_t * g);

#ifndef HAVE_DRAND48
extern double drand48(void);
#endif

typedef boxf rect_t;

typedef struct treenode_t {
	double area;
	rect_t	r;
	struct treenode_t *leftchild, *rightsib;
	union {Agraph_t *subg; Agnode_t *n;} u;
	int		kind;
} treenode_t;

#define BT 101
#define LR 202

typedef struct rlist_s {
	treenode_t *elt;
	int n, extent;
	double sum;
} rlist_t;

static treenode_t *newtreenode(treenode_t **first, treenode_t **prev)
{
	treenode_t *p;
	p = NEW(treenode_t);
	if (!*first) *first = p;
	if (*prev) (*prev)->rightsib = p;
	*prev = p;
	return p;
}

/* return list of treenodes */
static treenode_t *treebuilder(Agraph_t *g)
{
	int			i;
	treenode_t	*first = 0;
	treenode_t	*prev = 0;
	treenode_t	*p;
	Agraph_t 	*subg;
	Agnode_t 	*n;

	for (i = 1; i <= GD_n_cluster(g); i++) {
		subg = GD_clust(g)[i];
		if (agnnodes(subg) == 0) continue;
		p = newtreenode(&first,&prev);
#ifndef WITH_CGRAPH
		p->kind = AGGRAPH;
#else
		p->kind = AGRAPH;
#endif
		p->u.subg = subg;
		p->leftchild = treebuilder(subg);
	}

	for (n = agfstnode(g); n; n = agnxtnode(g,n)) {
		char *val;
		if (SPARENT(n)) continue;
		p = newtreenode(&first,&prev);
		val = agget(n,"area");
		if (val) p->area = atof(val);
		if (p->area <= 0.0) p->area = 1.0;
		p->kind = AGNODE;
		p->u.n = n;
		SPARENT(n) = g;
	}
	return first;
}

/* get recursive area requirements */
static double sizeit(treenode_t *tree)
{
	treenode_t	*p;
	double		mysize = 0.0;

	if (tree->leftchild) {
		for (p = tree->leftchild; p; p = p->rightsib) mysize += sizeit(p);
		tree->area = mysize;
	}
	return tree->area;
}

static rect_t mkrectangle(double x0, double y0, double x1, double y1)
{
	rect_t	rv;
	rv.LL.x = x0; rv.LL.y = y0; rv.UR.x = x1; rv.UR.y = y1;
	return rv;
}

static double weight(treenode_t *tree, treenode_t *split)
{
	double rv = 0.0;
	while (tree && (tree!=split)) {rv = rv + tree->area; tree = tree->rightsib;}
	return rv;
}

static void layout(treenode_t *tree, treenode_t *split, rect_t r)
{
	double size, halfsize, w1, tmp, width, height;
	rect_t r1,r2;
	treenode_t *p;

	if (!tree) return;
	if (!tree->rightsib || (tree->rightsib==split)) {tree->r = r; layout(tree->leftchild,0,r); return;}
	size = weight(tree,split);
	halfsize = size / 2.0;
	w1 = 0; tmp = 0;

	for (p = tree; p && (p != split); p = p->rightsib) {
		tmp = w1 + p->area;
		if (abs(halfsize - tmp) > abs(halfsize- w1))
			break;
		w1 = tmp;
	}

	width = r.UR.x - r.LL.x;
	height = r.UR.y - r.LL.y;
	if (width > height) {
		r1 = mkrectangle(r.LL.x,r.LL.y,r.LL.x + width * w1 / size, r.UR.y);
		r2 = mkrectangle(r1.UR.x,r.LL.y,r.UR.x,r.UR.y);
	}
	else {
	/*  this was bottom to top - but we want top to bottom layout 
		r1 = mkrectangle(r.LL.x,r.LL.y,r.UR.x,r.LL.y + height * w1 / size);
		r2 = mkrectangle(r.LL.x,r1.UR.y,r.UR.x,r.UR.y);
	*/
		r1 = mkrectangle(r.LL.x,r.UR.y - height * w1 / size,r.UR.x,r.UR.y);
		r2 = mkrectangle(r.LL.x,r.LL.y,r.UR.x,r1.LL.y);
	}
	layout(tree,p,r1);
	layout(p,split,r2);
}

static void printer(treenode_t *tree)
{
	static int onetime = 1;
	treenode_t	*p;

	if (onetime) { fprintf(stderr,"%%!PS\n"); onetime=0;}
	fprintf(stderr,"newpath %.3lf %.3lf moveto %.3lf %.3lf lineto %.3lf %.3lf lineto %.3lf %.3lf lineto closepath \n",
		tree->r.LL.x,tree->r.LL.y,
		tree->r.UR.x,tree->r.LL.y,
		tree->r.UR.x,tree->r.UR.y,
		tree->r.LL.x,tree->r.UR.y);
	if (tree->leftchild) {
		fprintf(stderr,"stroke \n");
		for (p = tree->leftchild; p; p = p->rightsib) printer(p);
	}
	else {
		fprintf(stderr,"gsave %.3lf 1.0 1.0 sethsbcolor fill grestore stroke\n",drand48());
	}
}

static void finishNode (node_t* n)
{
    char* str = strdup_and_subst_obj(NODENAME_ESC, (void*)n);
    ND_shape(n) = bind_shape("box", n);
    ND_label(n) = make_label((void*)n, str, LT_NONE,
		late_double(n, N_fontsize, DEFAULT_FONTSIZE, MIN_FONTSIZE),
		late_nnstring(n, N_fontname, DEFAULT_FONTNAME),
		late_nnstring(n, N_fontcolor, DEFAULT_COLOR));
    ND_shape(n)->fns->initfn(n);
}

static rect_t walker(treenode_t *tree)
{
    treenode_t	*p;
    Agnode_t	*n;
    pointf		center;
    rect_t      r, rr;

    switch(tree->kind) {
#ifndef WITH_CGRAPH
	case AGGRAPH:
#else
	case AGRAPH:
#endif
	    break;
	case AGNODE:
	    rr = tree->r;
	    center.x = (tree->r.UR.x + tree->r.LL.x) / 2.0;
	    center.y = (tree->r.UR.y + tree->r.LL.y) / 2.0;

	    n = tree->u.n;
	    ND_coord(n) = center;
	    ND_height(n) = PS2INCH(tree->r.UR.y - tree->r.LL.y);
	    ND_width(n) = PS2INCH(tree->r.UR.x - tree->r.LL.x);
	    gv_nodesize(n,GD_flip(agraphof(n)));
	    finishNode (n);
	    /*fprintf(stderr,"%s coord %.5g %.5g ht %d width %d\n",
		    agnameof(n), ND_coord(n).x, ND_coord(n).y, ND_ht(n),
		    ND_rw(n)+ND_lw(n));*/
	    break;
	default: abort();
    }
    if ((p = tree->leftchild)) {
	rr = walker (p);
	p = p->rightsib;
	for (; p; p = p->rightsib) {
	    r = walker(p);
	    EXPANDBB(rr,r);
	}
    rr.LL.x -= 2./72.;
    rr.LL.y -= 2./72.;
    rr.UR.x += 2./72.;
    rr.UR.y += 2./72.;
	GD_bb(tree->u.subg) = rr;
    }
    return rr;
}

#ifdef PWDRIVER
int main()
{
	static treenode_t 	root;
	rect_t r = {{0.0, 0.0}, {100.0, 100.0}};
	Agraph_t *g;

	aginit();
	g = agread(stdin);
#ifndef WITH_CGRAPH
	root.kind = AGGRAPH;
#else
	root.kind = AGRAPH;
#endif
	root.leftchild = treebuilder(g);
	sizeit(&root);
	/*layouter(&root,LR,r);*/
	printer(&root);
	return 0;
}
#endif

void patchwork_layout(Agraph_t *g)
{
	static treenode_t 	root;
	rect_t r = {{0.0, 0.0}, {100.0, 100.0}};
	patchwork_init_graph(g);
#ifndef WITH_CGRAPH
	root.kind = AGGRAPH;
#else
	root.kind = AGRAPH;
#endif
	root.leftchild = treebuilder(g);
	root.u.subg = g;
	sizeit(&root);
	layout(&root,0,r);
	/* printer(&root); */
	walker(&root);
	/* compute_bb(g); */
	/* fprintf(stderr,"bb %d %d %d %d\n", */
		/* GD_bb(g).LL.x, GD_bb(g).LL.y, GD_bb(g).UR.x, GD_bb(g).UR.y); */
	dotneato_postprocess(g);
}
