/* vim:set shiftwidth=4 ts=8: */

#include <assert.h>
#include "hier.h"
#include "memory.h"

/* To use:
  double* x_coords; // initial x coordinates
  double* y_coords; // initial y coordinates
  focus_t* fs;
  int ne;
  vtx_data* graph = makeGraph (topview*, &ne);
  hierarchy = makeHier(topview*, graph, x_coords, y_coords);
  freeGraph (graph);
  fs = initFocus (topview->Nodecount); // create focus set

  In loop, 
    update fs.
      For example, if user clicks mouse at (p.x,p.y) to pick a single new focus,
        int closest_fine_node;
        find_closest_active_node(hierarchy, p.x, p.y, &closest_fine_node);
        fs->num_foci = 1;
        fs->foci_nodes[0] = closest_fine_node;
        fs->x_foci[0] = hierarchy->geom_graphs[cur_level][closest_fine_node].x_coord; 
        fs->y_foci[0] = hierarchy->geom_graphs[cur_level][closest_fine_node].y_coord;


      
    set_active_levels(hierarchy, fs->foci_nodes, fs->num_foci);
    positionAllItems(hierarchy, fs, parms)

  When done:
    release (hierarchy);
*/

static void 
scale_coords (double* x_coords, double* y_coords, int n, hierparms_t* parms)
{
    int i;
    double w = parms->ClientWidth;
    double h = parms->ClientHeight;
    double margin = parms->margin;
    double minX,maxX,minY,maxY;
    double scale_ratioX;
    double scale_ratioY;
    double scale_ratio;

    w*=parms->graphSize/100.0; h*=parms->graphSize/100.0;
    w-=2*margin; h-=2*margin;
		
    minX=maxX=x_coords[0]; minY=maxY=y_coords[0];
    for (i=1; i<n; i++) {
	if (x_coords[i] < minX)
	    minX=x_coords[i];
	if (y_coords[i] < minY)
	    minY=y_coords[i];
	if (x_coords[i] > maxX)
	    maxX=x_coords[i];
	if (y_coords[i] > maxY)
	    maxY=y_coords[i];
    }
    for (i=0; i<n; i++) {
	x_coords[i] -= minX;
	y_coords[i] -= minY;
    }

    // scale the layout to fill canvas:
	
    scale_ratioX = w/(maxX-minX);
    scale_ratioY = h/(maxY-minY);
    scale_ratio = MIN(scale_ratioX, scale_ratioY);
    for (i=0; i<n; i++)  {
	x_coords[i] *= scale_ratio;
	y_coords[i] *= scale_ratio;
    }
		
    for (i=0; i<n; i++)  {
	x_coords[i] += margin; 
	y_coords[i] += margin;
    }
}

void
positionAllItems (Hierarchy* hp, focus_t* fs, hierparms_t* parms)
{
    int i;
    int interval = 20;
    int counter = 0;
    double* x_coords = N_NEW(hp->nvtxs[0], double);
    double* y_coords = N_NEW(hp->nvtxs[0], double);
    int max_level = hp->nlevels-1; // coarsest level
    int ClientWidth = parms->ClientWidth;
    int ClientHeight = parms->ClientHeight;
    int margin = parms->margin;

    /* get all logical coordinates of active nodes */
    for (i=0; i<hp->nvtxs[max_level]; i++) {
        counter = extract_active_logical_coords(hp, i, max_level, x_coords, y_coords, counter);
    }

    /* distort logical coordinates in order to get uniform density
     * (equivalent to concentrating on the focus area)
     */
    if (fs->num_foci==0) {
        scale_coords(x_coords, y_coords, counter, parms);
    }
    else switch (parms->rescale_type) {
    case Polar:
	rescale_layout_polar(x_coords, y_coords, fs->x_foci, fs->y_foci, fs->num_foci, counter, interval, ClientWidth, ClientHeight, margin);
	break;
    case Rectilinear:
	rescale_layout(x_coords, y_coords, counter, interval, ClientWidth, ClientHeight, margin);
	break;
    case NoRescale:
	scale_coords(x_coords, y_coords, counter, parms);
	break;
    }

    /* Update the final physical coordinates of the active nodes */
    for (counter = 0,i=0; i<hp->nvtxs[max_level]; i++) {
        counter = set_active_physical_coords(hp, i, max_level, x_coords, y_coords, counter);
    }

    free (x_coords);
    free (y_coords);
}

vtx_data*
makeGraph (topview* tv, int* nedges)
{
    int i;
    int ne = tv->Edgecount; /* upper bound */
    int nv = tv->Nodecount;
    vtx_data *graph = N_NEW(nv, vtx_data);
    int *edges = N_NEW(2 * ne + nv, int);   /* reserve space for self loops */
    Agnode_t* np;
    Agedge_t* ep;
    Agraph_t* g = NULL;
    int i_nedges;

    ne = 0;
    for (i = 0; i < nv; i++) {
        graph[i].edges = edges++;       /* reserve space for the self loop */
        graph[i].ewgts = NULL;
        graph[i].styles = NULL;
        i_nedges = 1;           /* one for the self */

        np = tv->Nodes[i].Node;
        if (!g) g = agraphof (np);
	for (ep = agfstedge(g, np); ep; ep = agnxtedge(g, ep, np)) {
	    Agnode_t* vp;
	    Agnode_t* tp = agtail(ep);
	    Agnode_t* hp = aghead(ep);
            assert (hp != tp);
		/* FIX: handle multiedges */
	    vp = (tp == np ? hp : tp);
	    ne++;
	    *edges++ = ((custom_object_data*)AGDATA(vp))->TVRef;
	}

        graph[i].nedges = i_nedges;
        graph[i].edges[0] = i;
    }
    ne /= 2;   /* each edge counted twice */
    *nedges = ne;
    return graph;
}

Hierarchy*
makeHier (topview* tv, vtx_data* graph, double* x_coords, double* y_coords)
{
    vtx_data* delaunay;
    ex_vtx_data* geom_graph;
    int nn = tv->Nodecount;
    int ne = tv->Edgecount;
    int ngeom_edges;
    Hierarchy* hp = NEW(Hierarchy);

    delaunay = UG_graph(x_coords, y_coords, nn, 0);

    ngeom_edges = init_ex_graph(delaunay, graph, nn, x_coords, y_coords, &geom_graph);
    free (delaunay[0].edges); free (delaunay);

    hp = create_hierarchy(graph, nn, ne, geom_graph, ngeom_edges, 20); 
    free (geom_graph[0].edges); free (geom_graph);

    set_horizontal_active_level(hp, 0);

    return hp;
}

