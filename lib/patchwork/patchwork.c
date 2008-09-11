#include <stdio.h>
#include <stdlib.h>
#include <patchwork.h>
#include "render.h"

extern void patchwork_init_graph(graph_t * g);

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
		p->kind = AGGRAPH;
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

/* normal layout */
static void layouter(treenode_t *tree, int dir, rect_t r)
{
	double	delta;
	pointf	ref;
	rect_t	r0;
	treenode_t	*p;

	tree->r = r;
	if (dir == BT) delta = (r.UR.y - r.LL.y) / tree->area;
	else if (dir == LR) delta = (r.UR.x - r.LL.x) / tree->area;
	else abort();
	ref = r.LL;
	for (p = tree->leftchild; p; p = p->rightsib) {
		r0.LL = ref;
		if (dir == BT) {
			r0.UR.x = r.UR.x;
			r0.UR.y = ref.y + p->area * delta;
		}
		else {
			r0.UR.x = ref.x + p->area * delta;
			r0.UR.y = r.UR.y;
		}
		layouter(p,(dir == BT? LR : BT),r0);
		if (dir == BT) ref.y = r0.UR.y;
		else ref.x = r0.UR.x;
	}
}

/* squarified layout */
static double aspect(rect_t r) { return (r.UR.y - r.LL.y)/(r.UR.x - r.LL.x); }

static void sqlayouter(treenode_t *list, int dir, rect_t r, double total)
{
	double frac = list->area / total;
	rect_t s = r;
	if (dir == BT) s.UR.y = s.LL.y +  frac * (r.UR.y - r.LL.y);
	else s.UR.x = s.LL.x + frac * (r.UR.x - r.LL.x);
	list->r = s;

	if (list->leftchild) {
		if (aspect(r) > 1) sqlayouter(list->leftchild, BT, r, list->area);
		else sqlayouter(list->leftchild, LR,r, list->area);
	}

	if (list->rightsib) {
		total = total - list->area;
		if (dir == BT) r.LL.y = s.UR.y;
		else r.LL.x = s.UR.x;
		if (aspect(r) > 1) sqlayouter(list->rightsib, BT, r, total);
		else sqlayouter(list->rightsib, LR, r, total);
	}
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
		case AGGRAPH:
		case AGRAPH:
			break;
		case AGNODE:
			rr = tree->r;
			center.x = (tree->r.UR.x + tree->r.LL.x) / 2.0;
			center.y = (tree->r.UR.y + tree->r.LL.y) / 2.0;

			n = tree->u.n;
			ND_coord(n) = center;
			ND_height(n) = PS2INCH(tree->r.UR.y - tree->r.LL.y);
			ND_width(n) = PS2INCH(tree->r.UR.x - tree->r.LL.x);
			gv_nodesize(n,GD_flip(n->graph));
			finishNode (n);
			/*fprintf(stderr,"%s coord %.3g %.3g ht %d width %d\n",
				n->name, ND_coord(n).x, ND_coord(n).y, ND_ht_i(n),
				ND_rw_i(n)+ND_lw_i(n));*/
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
	root.leftchild = treebuilder(g);
	sizeit(&root);
	/*layouter(&root,LR,r);*/
	sqlayouter(&root,LR,r,root.area);
	printer(&root);
	return 0;
}
#endif

void patchwork_layout(Agraph_t *g)
{
	static treenode_t 	root;
	rect_t r = {{0.0, 0.0}, {100.0, 100.0}};
	patchwork_init_graph(g);
	root.leftchild = treebuilder(g);
	root.u.subg = g;
	sizeit(&root);
	sqlayouter(&root,LR,r,root.area);
	/* printer(&root); */
	walker(&root);
	/* compute_bb(g); */
	/* fprintf(stderr,"bb %d %d %d %d\n", */
		/* GD_bb(g).LL.x, GD_bb(g).LL.y, GD_bb(g).UR.x, GD_bb(g).UR.y); */
	dotneato_postprocess(g);
}
