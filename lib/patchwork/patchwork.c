#include <stdio.h>
#include <stdlib.h>
#include "render.h"

typedef struct rect_t {pointf ll; pointf ur;} rect_t;
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
	p = malloc(sizeof(treenode_t));
	p->leftchild = p->rightsib = 0;
	p->area = 0.0;
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
		p->kind = AGRAPH;
		p->u.subg = subg;
		p->leftchild = treebuilder(subg);
	}

	for (n = agfstnode(g); n; n = agnxtnode(g,n)) {
		char *val;
		p = newtreenode(&first,&prev);
		val = agget(n,"area");
		if (val) p->area = atof(val);
		if (p->area <= 0.0) p->area = 1.0;
		p->kind = AGNODE;
		p->u.n = n;
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
	if (dir == BT) delta = (r.ur.y - r.ll.y) / tree->area;
	else if (dir == LR) delta = (r.ur.x - r.ll.x) / tree->area;
	else abort();
	ref = r.ll;
	for (p = tree->leftchild; p; p = p->rightsib) {
		r0.ll = ref;
		if (dir == BT) {
			r0.ur.x = r.ur.x;
			r0.ur.y = ref.y + p->area * delta;
		}
		else {
			r0.ur.x = ref.x + p->area * delta;
			r0.ur.y = r.ur.y;
		}
		layouter(p,(dir == BT? LR : BT),r0);
		if (dir == BT) ref.y = r0.ur.y;
		else ref.x = r0.ur.x;
	}
}

/* squarified layout */
static double aspect(rect_t r) { return (r.ur.y - r.ll.y)/(r.ur.x - r.ll.x); }

static void sqlayouter(treenode_t *list, int dir, rect_t r, double total)
{
	double frac = list->area / total;
	rect_t s = r;
	if (dir == BT) s.ur.y = s.ll.y +  frac * (r.ur.y - r.ll.y);
	else s.ur.x = s.ll.x + frac * (r.ur.x - r.ll.x);
	list->r = s;

	if (list->leftchild) {
		if (aspect(r) > 1) sqlayouter(list->leftchild, BT, r, list->area);
		else sqlayouter(list->leftchild, LR,r, list->area);
	}

	if (list->rightsib) {
		total = total - list->area;
		if (dir == BT) r.ll.y = s.ur.y;
		else r.ll.x = s.ur.x;
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
		tree->r.ll.x,tree->r.ll.y,
		tree->r.ur.x,tree->r.ll.y,
		tree->r.ur.x,tree->r.ur.y,
		tree->r.ll.x,tree->r.ur.y);
	if (tree->leftchild) {
		fprintf(stderr,"stroke \n");
		for (p = tree->leftchild; p; p = p->rightsib) printer(p);
	}
	else {
		fprintf(stderr,"gsave %.3lf 1.0 1.0 sethsbcolor fill grestore stroke\n",drand48());
	}
}

static void walker(treenode_t *tree)
{
	treenode_t	*p;
	Agnode_t	*n;
	point		center;

	switch(tree->kind) {
		case AGRAPH:
			break;
		case AGNODE:
			center.x = (tree->r.ur.x + tree->r.ll.x) / 2.0;
			center.y = (tree->r.ur.y + tree->r.ll.y) / 2.0;

			n = tree->u.n;
			ND_coord_i(n) = center;
			ND_height(n) = PS2INCH(tree->r.ur.y - tree->r.ll.y);
			ND_width(n) = PS2INCH(tree->r.ur.x - tree->r.ll.x);
			gv_nodesize(n,GD_flip(n->graph));
			/*fprintf(stderr,"%s coord %d %d ht %d width %d\n",
				n->name, ND_coord_i(n).x, ND_coord_i(n).y, ND_ht_i(n),
				ND_rw_i(n)+ND_lw_i(n));*/
			break;
		default: abort();
	}
	if (tree->leftchild)
		for (p = tree->leftchild; p; p = p->rightsib) walker(p);
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
	sizeit(&root);
	sqlayouter(&root,LR,r,root.area);
	printer(&root);
	walker(&root);
	compute_bb(g);
	fprintf(stderr,"bb %d %d %d %d\n",
		GD_bb(g).LL.x, GD_bb(g).LL.y, GD_bb(g).UR.x, GD_bb(g).UR.y);
	dotneato_postprocess(g);
}
