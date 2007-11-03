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


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

/* TODO:
 * If cut point is in exactly 2 blocks, expand block circles to overlap
 * especially in the case where one block is the sole child of the other.
 */

#include	"blockpath.h"

#define		LEN(x,y) (sqrt((x)*(x) + (y)*(y)))

/* getRotation:
 * The function determines how much the block should be rotated
 * for best positioning with parent, assuming its center is at x and y
 * relative to the parent.
 * angle gives the angle of the new position, i.e., tan(angle) = y/x.
 * If sn has 2 nodes, we arrange the line of the 2 normal to angle.
 * If sn has 1 node, parent_pos has already been set to the 
 * correct angle assuming no rotation.
 * Otherwise, we find the node in sn connected to the parent and rotate
 * the block so that it is closer or at least visible to its node in the
 * parent.
 *
 * For COALESCED blocks, if neighbor is in left half plane, 
 * use unCOALESCED case.
 * Else let theta be angle, R = LEN(x,y), pho the radius of actual 
 * child block, phi be angle of neighbor in actual child block,
 * and r the distance from center of coalesced block to center of 
 * actual block. Then, the angle to rotate the coalesced block to
 * that the edge from the parent is tangent to the neighbor on the
 * actual child block circle is
 *    alpha = theta + PI/2 - phi - arcsin((l/R)*(sin B))
 * where l = r - rho/(cos phi) and beta = PI/2 + phi.
 * Thus, 
 *    alpha = theta + PI/2 - phi - arcsin((l/R)*(cos phi))
 */
static double
getRotation(block_t * sn, Agraph_t * g, double x, double y, double theta)
{
    double mindist;
    Agraph_t *subg;
    /* Agedge_t* e; */
    Agnode_t *n, *closest_node, *neighbor;
    nodelist_t *list;
    double len, newX, newY;
    int count;

    subg = sn->sub_graph;
#ifdef OLD
    parent = sn->parent;
#endif

    list = sn->circle_list;

    if (sn->parent_pos >= 0) {
	theta += PI - sn->parent_pos;
	if (theta < 0)
	    theta += 2 * PI;

	return theta;
    }

    count = sizeNodelist(list);
    if (count == 2) {
	return (theta - PI / 2.0);
    }

    /* Find node in block connected to block's parent */
    neighbor = CHILD(sn);
#ifdef OLD
    for (e = agfstedge(g, parent); e; e = agnxtedge(g, e, parent)) {
	n = e->head;
	if (n == parent)
	    n = e->tail;

	if ((BLOCK(n) == sn) && (PARENT(n) == parent)) {
	    neighbor = n;
	    break;
	}
    }
#endif
    newX = ND_pos(neighbor)[0] + x;
    newY = ND_pos(neighbor)[1] + y;
    mindist = LEN(newX, newY);
    closest_node = neighbor;

    for (n = agfstnode(subg); n; n = agnxtnode(subg, n)) {
	if (n == neighbor)
	    continue;

	newX = ND_pos(n)[0] + x;
	newY = ND_pos(n)[1] + y;

	len = LEN(newX, newY);
	if (len < mindist) {
	    mindist = len;
	    closest_node = n;
	}
    }

    /* if((neighbor != closest_node) && !ISPARENT(neighbor)) { */
    if (neighbor != closest_node) {
	double rho = sn->rad0;
	double r = sn->radius - rho;
	double n_x = ND_pos(neighbor)[0];
	if (COALESCED(sn) && (-r < n_x)) {
	    double R = LEN(x, y);
	    double n_y = ND_pos(neighbor)[1];
	    double phi = atan2(n_y, n_x + r);
	    double l = r - rho / (cos(phi));

	    theta += PI / 2.0 - phi - asin((l / R) * (cos(phi)));
	} else {		/* Origin still at center of this block */
	    double phi = atan2(ND_pos(neighbor)[1], ND_pos(neighbor)[0]);
	    theta += PI - phi - PSI(neighbor);
	    if (theta > 2 * PI)
		theta -= 2 * PI;
	}
    } else
	theta = 0;
    return theta;
}


/* applyDelta:
 * Recursively apply rotation rotate followed by translation (x,y)
 * to block sn and its children.
 */
static void applyDelta(block_t * sn, double x, double y, double rotate)
{
    block_t *child;
    Agraph_t *subg;
    Agnode_t *n;

    subg = sn->sub_graph;

    for (n = agfstnode(subg); n; n = agnxtnode(subg, n)) {
	double X, Y;

	if (rotate != 0) {
	    double tmpX, tmpY;
	    double cosR, sinR;

	    tmpX = ND_pos(n)[0];
	    tmpY = ND_pos(n)[1];
	    cosR = cos(rotate);
	    sinR = sin(rotate);

	    X = tmpX * cosR - tmpY * sinR;
	    Y = tmpX * sinR + tmpY * cosR;
	} else {
	    X = ND_pos(n)[0];
	    Y = ND_pos(n)[1];
	}

	/* translate */
	ND_pos(n)[0] = X + x;
	ND_pos(n)[1] = Y + y;
    }

    for (child = sn->children.first; child; child = child->next)
	applyDelta(child, x, y, rotate);
}

/* firstangle and lastangle give the range of child angles.
 * These are set and used only when a block has just 1 node.
 * And are used to give the center angle between the two extremes.
 * The parent will then be attached at PI - center angle (parent_pos).
 * If this block has no children, this is PI. Otherwise, doParent will
 * be called once with the blocks node. firstangle will be 0, with
 * succeeding angles increasing. 
 * position can always return the center angle - PI, since the block
 * must have children and if the block has 1 node, the limits will be
 * correctly set. If the block has more than 1 node, the value is
 * unused.
 */
typedef struct {
    double radius;		/* Basic radius of block */
    double subtreeR;		/* Max of subtree radii */
    double nodeAngle;		/* Angle allocated to each node in block */
    double firstAngle;		/* Smallest child angle when block has 1 node */
    double lastAngle;		/* Largest child angle when block has 1 node */
    block_t *cp;		/* Children of block */
    node_t *neighbor;		/* Node connected to parent block, if any */
} posstate;

/* doParent:
 * Attach child blocks belonging to n.
 * children of n are placed in theta +/- stp->nodeAngle/2 unless
 * length = 1, in which case they go from 0 to
 * childCount*(incidentAngle + mindistAngle/2).
 * If length is 1, keeps track of minimum and maximum child angle.
 */
static double
doParent(Agraph_t * g, double theta, Agnode_t * n,
	 int length, double min_dist, posstate * stp)
{
    block_t *child;
    double mindistance;
    double childAngle;		/* angle of child */
    double deltaX, deltaY;
    double snRadius = stp->subtreeR;	/* max subtree radius */
    double firstAngle = stp->firstAngle;
    double lastAngle = stp->lastAngle;
    double rotateAngle;
    double incidentAngle;

    int childCount = 0;		/* No. of blocks that are children of n */
    double maxRadius = 0;	/* Max. radius of children */
    double childRadius;		/* Radius of circle on which children are placed */
    double diameter = 0;	/* sum of child diameters */
    double mindistAngle;	/* angle to move min_dist at childRadius */
    int cnt, midChild;
    double midAngle = 0;

    for (child = stp->cp; child; child = child->next) {
	if (BLK_PARENT(child) == n) {
	    childCount++;
	    if (maxRadius < child->radius) {
		maxRadius = child->radius;
	    }
	    diameter += 2 * child->radius + min_dist;
	}
    }

    if (length == 1)
	childAngle = 0;
    else
	childAngle = theta - stp->nodeAngle / 2;

    childRadius = length * diameter / (2 * PI);

    /* FIX: If the parent block stp has only 1 child, we should probably
     * also set childRadius to mindistance. In this case, can 1 prove that
     * childRadius < mindistance? Probably not, since we can increase length
     * arbitrarily.
     */
    mindistance = stp->radius + min_dist + maxRadius;
    if (childRadius < mindistance)
	childRadius = mindistance;

    if ((childRadius + maxRadius) > snRadius)
	snRadius = childRadius + maxRadius;

    mindistAngle = min_dist / childRadius;

    cnt = 0;
    midChild = (childCount + 1) / 2;
    for (child = stp->cp; child; child = child->next) {
	if (BLK_PARENT(child) != n)
	    continue;
	if (sizeNodelist(child->circle_list) <= 0)
	    continue;

	incidentAngle = child->radius / childRadius;
	if (length == 1) {
	    if (childAngle != 0)
		childAngle += incidentAngle;

	    if (firstAngle < 0)
		firstAngle = childAngle;

	    lastAngle = childAngle;
	} else {
	    if (childCount == 1) {
		childAngle = theta;
	    } else {
		childAngle += incidentAngle + mindistAngle / 2;
	    }
	}

	deltaX = childRadius * cos(childAngle);
	deltaY = childRadius * sin(childAngle);

	/* first apply the delta to the immediate child and see if we need
	 * to rotate it for better edge link                                            
	 * should return the theta value if there was a rotation else zero
	 */

	rotateAngle = getRotation(child, g, deltaX, deltaY, childAngle);
	applyDelta(child, deltaX, deltaY, rotateAngle);

	if (length == 1) {
	    childAngle += incidentAngle + mindistAngle;
	} else {
	    childAngle += incidentAngle + mindistAngle / 2;
	}
	cnt++;
	if (cnt == midChild)
	    midAngle = childAngle;
    }

    if ((length > 1) && (n == stp->neighbor)) {
	PSI(n) = midAngle;
    }

    stp->subtreeR = snRadius;
    stp->firstAngle = firstAngle;
    stp->lastAngle = lastAngle;
    return maxRadius;
}

/* position:
 * Assume childCount > 0
 */
static double
position(Agraph_t * g, int childCount, int length, nodelist_t * path,
	 block_t * sn, double min_dist)
{
    nodelistitem_t *item;
    Agnode_t *n;
    posstate state;
    int counter = 0;
    double maxRadius = 0.0;
    double angle;
    double theta = 0.0;

    state.cp = sn->children.first;
    state.subtreeR = sn->radius;
    state.radius = sn->radius;
    state.neighbor = CHILD(sn);
    state.nodeAngle = 2 * PI / length;
    state.firstAngle = -1;
    state.lastAngle = -1;

    for (item = path->first; item; item = item->next) {
	n = item->curr;

	if (length != 1) {
	    theta = counter * state.nodeAngle;
	    counter++;
	}

	if (ISPARENT(n))
	    maxRadius = doParent(g, theta, n, length, min_dist, &state);
    }

    /* If block has only 1 child, to save space, we coalesce it with the
     * child. Instead of having final radius sn->radius + max child radius,
     * we have half that. However, the origin of the block is no longer in
     * the center of the block, so we cannot do a simple rotation to get
     * the neighbor node next to the parent block in getRotate.
     */
    if (childCount == 1) {
	applyDelta(sn, -(maxRadius + min_dist / 2), 0, 0);
	sn->radius += min_dist / 2 + maxRadius;
	SET_COALESCED(sn);
    } else
	sn->radius = state.subtreeR;

    angle = (state.firstAngle + state.lastAngle) / 2.0 - PI;
    return angle;
}

/* doBlock:
 * Set positions of block sn and its child blocks.
 */
static void doBlock(Agraph_t * g, block_t * sn, double min_dist)
{
    block_t *child;
    nodelist_t *longest_path;
    int childCount, length;
    double centerAngle = PI;

    /* layout child subtrees */
    childCount = 0;
    for (child = sn->children.first; child; child = child->next) {
	doBlock(g, child, min_dist);
	childCount++;
    }

    /* layout this block */
    longest_path = layout_block(g, sn, min_dist);
    sn->circle_list = longest_path;
    length = sizeNodelist(longest_path);	/* path contains everything in block */

    /* attach children */
    if (childCount > 0)
	centerAngle =
	    position(g, childCount, length, longest_path, sn, min_dist);

    if ((length == 1) && (BLK_PARENT(sn))) {
	sn->parent_pos = centerAngle;
	if (sn->parent_pos < 0)
	    sn->parent_pos += 2 * PI;
    }
}

void circPos(Agraph_t * g, block_t * sn, circ_state * state)
{
    doBlock(g, sn, state->min_dist);
}
