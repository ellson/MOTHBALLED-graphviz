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

#include "digcola.h"
#ifdef DIGCOLA
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <float.h>
#include <assert.h>
#include "matrix_ops.h"
#include "kkutils.h"
#include "quad_prog_solver.h"

#define quad_prog_tol 1e-2

float**
unpackMatrix(float * packedMat, int n)
{
    float **mat;
	int i,j,k;

	mat = N_GNEW(n, float*);
	mat[0] = N_GNEW(n*n, float);
	set_vector_valf(n*n, 0, mat[0]);
	for (i=1; i<n; i++) {
		mat[i] = mat[0]+i*n;
	}

	for (i=0,k=0; i<n; i++) {
		for (j=i; j<n; j++,k++) {
			mat[j][i]=mat[i][j]=packedMat[k];
		}
	}
    return mat;
}

static void
ensureMonotonicOrdering(float * place, int n, int * ordering) 
{
	/* ensure that 'ordering' is monotonically increasing by 'place',  */
	/* this also implies that levels are separated in the initial layout */
	int i, node;
	float lower_bound = place[ordering[0]];
	for (i=1; i<n; i++) {		
		node = ordering[i];
		if (place[node]<lower_bound) {
			place[node]=lower_bound;
		}
		lower_bound = place[node];
	}
}
			
static void
ensureMonotonicOrderingWithGaps(float* place, int n, int* ordering, 
                                int* levels, int num_levels, float levels_gap)
{
	/* ensure that levels are separated in the initial layout and that 
     * places are monotonic increasing within layer
     */
	
	int i;
	int node, level, max_in_level;
	float lower_bound = (float)-1e9;

	level = -1; max_in_level=0;		
	for (i=0; i<n; i++) {
		if (i>=max_in_level) {
			/* we are entering a new level */
			level++;
			if (level==num_levels) {
				/* last_level */
				max_in_level=n;
			}
			else {
				max_in_level = levels[level];
			}
			lower_bound = i>0 ? place[ordering[i-1]]+levels_gap : (float)-1e9;
			quicksort_placef(place, ordering, i, max_in_level-1); 
		}

		node = ordering[i];
		if (place[node]<lower_bound) {
			place[node]=lower_bound;
		}
	}
}

static void
computeHierarchyBoundaries(float* place, int n, int* ordering, int* levels, 
                           int num_levels, float* hierarchy_boundaries)
{
	int i;
	for (i=0; i<num_levels; i++) {
		hierarchy_boundaries[i] = place[ordering[levels[i]-1]];
	}
}


int 
constrained_majorization_new(CMajEnv *e, float * b, float **coords, 
                             int cur_axis, int dims, int max_iterations, 
                             float* hierarchy_boundaries, float levels_gap) 
{
	int n=e->n;
	float *place=coords[cur_axis];
	float **lap=e->A;
	int * ordering=e->ordering;
	int * levels=e->levels;
	int num_levels=e->num_levels; 
	int i,j;
	float new_place_i;
	bool converged=false;
	float upper_bound, lower_bound;
	int node;
	int left, right;
	float cur_place;
	float des_place_block;
	float block_deg;
	float toBlockConnectivity;
	float * lap_node;
	int block_len;
	int first_next_level;
	int level=-1, max_in_level=0;	
	float* desired_place;
	float* prefix_desired_place;
	float* suffix_desired_place;
	int* block;
	int* lev;
    int counter;

	if (max_iterations<=0) {
		return 0;
	}
	if (levels_gap!=0) {
		return constrained_majorization_new_with_gaps(e, b, coords,cur_axis,dims, max_iterations, hierarchy_boundaries, levels_gap);
	}

	/* ensureMonotonicOrderingWithGaps(place, n, ordering, levels, num_levels);  */
	ensureMonotonicOrdering(place, n, ordering); 
	/* it is important that in 'ordering' nodes are always sorted by layers, 
     * and within a layer by 'place'
     */
	
    /* the desired place of each individual node in the current block */
	desired_place = e->fArray1;
    /* the desired place of each prefix of current block */
	prefix_desired_place = e->fArray2;
    /* the desired place of each suffix of current block */
	suffix_desired_place = e->fArray3;
    /* current block (nodes connected by active constraints) */
	block = e->iArray1;

	lev = e->iArray2; /* level of each node */
	for (i=0; i<n; i++) {
		if (i>=max_in_level) {
			/* we are entering a new level */
			level++;
			if (level==num_levels) {
				/* last_level */
				max_in_level=n;
			}
			else {
				max_in_level = levels[level];
			}		
		}
		node = ordering[i];
		lev[node]=level;	
	}

	for (counter=0; counter<max_iterations&&!converged; counter++) {
		converged=true;
		lower_bound = -1e9; /* no lower bound for first level */
		for (left=0; left<n; left=right) {
			int best_i;
			double max_movement;
			double movement;
			float prefix_des_place, suffix_des_place;
			/* compute a block 'ordering[left]...ordering[right-1]' of 
             * nodes with the same coordinate:
             */
			cur_place = place[ordering[left]];			
			for (right=left+1; right<n; right++) {
				if (place[ordering[right]]!=cur_place) {
					break;
				}
			}

			/* compute desired place of nodes in block: */
			for (i=left; i<right; i++) {
				node = ordering[i];
				new_place_i = -b[node];
				lap_node = lap[node];
				for (j=0; j<n; j++) {
					if (j==node) {
						continue;
					}
					new_place_i	+= lap_node[j]*place[j];
				}
				desired_place[node] = new_place_i/(-lap_node[node]);
			}

			/* reorder block by levels, and within levels by "relaxed" desired position */
			block_len = 0;
			first_next_level=0;
			for (i=left; i<right; i=first_next_level) {
				level = lev[ordering[i]];
				if (level==num_levels) {
					/* last_level */
					first_next_level=right;
				}
				else {
					first_next_level = MIN(right,levels[level]);
				}

				/* First, collect all nodes with desired places smaller than current place */
				for (j=i; j<first_next_level; j++) {
					node = ordering[j];
					if (desired_place[node] < cur_place) {
						block[block_len++] = node;
					}
				}
				/* Second, collect all nodes with desired places equal to current place */
				for (j=i; j<first_next_level; j++) {
					node = ordering[j];
					if (desired_place[node] == cur_place) {
						block[block_len++] = node;
					}
				}
				/* Third, collect all nodes with desired places greater than current place */
				for (j=i; j<first_next_level; j++) {
					node = ordering[j];
					if (desired_place[node] > cur_place) {
						block[block_len++] = node;
					}
				}
			}
			
			/* loop through block and compute desired places of its prefixes */
			des_place_block = 0;
			block_deg = 0;
			for (i=0; i<block_len; i++) {
				node = block[i];
				toBlockConnectivity = 0;
				lap_node = lap[node];
				for (j=0; j<i; j++) {
					toBlockConnectivity -= lap_node[block[j]];
				}				
				toBlockConnectivity*=2;
				/* update block stats */
				des_place_block = (block_deg*des_place_block+(-lap_node[node])*desired_place[node]+toBlockConnectivity*cur_place)/(block_deg-lap_node[node]+toBlockConnectivity);
				prefix_desired_place[i] = des_place_block;
				block_deg += toBlockConnectivity-lap_node[node];				
			}

			/* loop through block and compute desired places of its suffixes */
			des_place_block = 0;
			block_deg = 0;
			for (i=block_len-1; i>=0; i--) {
				node = block[i];
				toBlockConnectivity = 0;
				lap_node = lap[node];
				for (j=i+1; j<block_len; j++) {
					toBlockConnectivity -= lap_node[block[j]];
				}
				toBlockConnectivity*=2;
				/* update block stats */
				des_place_block = (block_deg*des_place_block+(-lap_node[node])*desired_place[node]+toBlockConnectivity*cur_place)/(block_deg-lap_node[node]+toBlockConnectivity);
				suffix_desired_place[i] = des_place_block;
				block_deg += toBlockConnectivity-lap_node[node];				
			}

		
			/* now, find best place to split block */
			best_i = -1;
			max_movement = 0;
			for (i=0; i<block_len; i++) {
				suffix_des_place = suffix_desired_place[i];
				prefix_des_place = i>0 ? prefix_desired_place[i-1] : suffix_des_place;
				/* limit moves to ensure that the prefix is placed before the suffix */
				if (suffix_des_place<prefix_des_place) {
					if (suffix_des_place<cur_place) {
						if (prefix_des_place > cur_place) {
							prefix_des_place = cur_place;
						}
						suffix_des_place = prefix_des_place;
					}
					else if (prefix_des_place>cur_place) {
						prefix_des_place = suffix_des_place;
					}
				}
				movement = (block_len-i)*fabs(suffix_des_place-cur_place) + i*fabs(prefix_des_place-cur_place);
				if (movement>max_movement) {
					max_movement = movement;
					best_i = i;
				}
			}
			/* Actually move prefix and suffix */
			if (best_i>=0) {
				suffix_des_place = suffix_desired_place[best_i];
				prefix_des_place = best_i>0 ? prefix_desired_place[best_i-1] : suffix_des_place;
				
				/* compute right border of feasible move */
				if (right>=n) {
					/* no nodes after current block */
					upper_bound = 1e9;
				}
				else {
					upper_bound = place[ordering[right]];
				}
				suffix_des_place = MIN(suffix_des_place, upper_bound);
				prefix_des_place = MAX(prefix_des_place, lower_bound);
			
				/* limit moves to ensure that the prefix is placed before the suffix */
				if (suffix_des_place<prefix_des_place) {
					if (suffix_des_place<cur_place) {
						if (prefix_des_place > cur_place) {
							prefix_des_place = cur_place;
						}
						suffix_des_place = prefix_des_place;
					}
					else if (prefix_des_place>cur_place) {
						prefix_des_place = suffix_des_place;
					}
				}

				/* move prefix: */
				for (i=0; i<best_i; i++) {
					place[block[i]] = prefix_des_place;
				}
				/* move suffix: */
				for (i=best_i; i<block_len; i++) {
					place[block[i]] = suffix_des_place;
				}
				
				lower_bound = suffix_des_place; // lower bound for next block

				/* reorder 'ordering' to reflect change of places
				 * Note that it is enough to reorder the level where 
                 * the split was done
                 */
#if 0
				int max_in_level, min_in_level;
				
				level = lev[best_i];
				if (level==num_levels) {
					// last_level
					max_in_level = MIN(right,n);		
				}
				else {
					max_in_level = MIN(right,levels[level]);
				}
				if (level==0) {
					// first level
					min_in_level = MAX(left,0);		
				}
				else {
					min_in_level = MAX(left,levels[level-1]);
				}
#endif
				for (i=left; i<right; i++) {
					ordering[i] = block[i-left];
				}
				converged = converged && fabs(prefix_des_place-cur_place)<quad_prog_tol  
								      && fabs(suffix_des_place-cur_place)<quad_prog_tol;
			
			}
			else {
				/* no movement */
				lower_bound = cur_place; // lower bound for next block
			}
			
		}
	}

	computeHierarchyBoundaries(place, n, ordering, levels, num_levels, hierarchy_boundaries);

	return counter;
}

int 
constrained_majorization_new_with_gaps(CMajEnv *e, float* b, float** coords, 
    int ndims, int cur_axis, int max_iterations, float* hierarchy_boundaries,
    float levels_gap)
{
	float *place=coords[cur_axis];
	int i,j;
	int n=e->n;
	float** lap = e->A;
	int* ordering = e->ordering;
	int* levels = e->levels;
	int num_levels = e->num_levels; 
	float new_place_i;
	bool converged=false;
	float upper_bound, lower_bound;
	int node;
	int left, right;
	float cur_place;
	float des_place_block;
	float block_deg;
	float toBlockConnectivity;
	float * lap_node;
	float * desired_place;
	float * prefix_desired_place;
	float * suffix_desired_place;
    int* block;
	int block_len;
	int first_next_level;
	int * lev;
	int level=-1, max_in_level=0;	
    int counter;
	float* gap;
	float total_gap, target_place;

	if (max_iterations<=0) {
		return 0;
	}

	ensureMonotonicOrderingWithGaps(place, n, ordering, levels, num_levels, levels_gap);
	/* it is important that in 'ordering' nodes are always sorted by layers, 
     * and within a layer by 'place'
     */
	
    /* the desired place of each individual node in the current block */
	desired_place = e->fArray1; 
    /* the desired place of each prefix of current block */
	prefix_desired_place = e->fArray2; 
    /* the desired place of each suffix of current block */
	suffix_desired_place = e->fArray3; 
    /* current block (nodes connected by active constraints) */
	block = e->iArray1;

	lev = e->iArray2; /* level of each node */
	for (i=0; i<n; i++) {
		if (i>=max_in_level) {
			/* we are entering a new level */
			level++;
			if (level==num_levels) {
				/* last_level */
				max_in_level=n;
			}
			else {
				max_in_level = levels[level];
			}		
		}
		node = ordering[i];
		lev[node]=level;	
	}

    /* displacement of block's nodes from block's reference point */
	gap = e->fArray4;
	
	for (counter=0; counter<max_iterations&&!converged; counter++) {
		converged=true;
		lower_bound = -1e9; // no lower bound for first level
		for (left=0; left<n; left=right) {
			int best_i;
			double max_movement;
			double movement;
			float prefix_des_place, suffix_des_place;
			/* compute a block 'ordering[left]...ordering[right-1]' of 
             * nodes connected with active constraints
             */
			cur_place = place[ordering[left]];			
			total_gap = 0;	
			target_place = cur_place;
			gap[ordering[left]] = 0;
			for (right=left+1; right<n; right++) {
				if (lev[right]>lev[right-1]) {
					/* we are entering a new level */
					target_place += levels_gap; /* note that 'levels_gap' may be negative */
					total_gap += levels_gap; 
				}
				node = ordering[right];
#if 0
				if (place[node]!=target_place)
#endif
            /* not sure if this is better than 'place[node]!=target_place' */
				if (fabs(place[node]-target_place)>1e-9) 
                {
					break;
				}
				gap[node] = place[node]-cur_place;			
			}			

			/* compute desired place of block's reference point according 
             * to each node in the block:
             */
			for (i=left; i<right; i++) {
				node = ordering[i];
				new_place_i = -b[node];
				lap_node = lap[node];
				for (j=0; j<n; j++) {
					if (j==node) {
						continue;
					}
					new_place_i	+= lap_node[j]*place[j];
				}
				desired_place[node] = new_place_i/(-lap_node[node]) - gap[node];
			}

			/* reorder block by levels, and within levels 
             * by "relaxed" desired position
             */
			block_len = 0;
			first_next_level=0;
			for (i=left; i<right; i=first_next_level) {
				level = lev[ordering[i]];
				if (level==num_levels) {
					/* last_level */
					first_next_level=right;
				}
				else {
					first_next_level = MIN(right,levels[level]);
				}

				/* First, collect all nodes with desired places smaller 
                 * than current place
                 */
				for (j=i; j<first_next_level; j++) {
					node = ordering[j];
					if (desired_place[node] < cur_place) {
						block[block_len++] = node;
					}
				}
				/* Second, collect all nodes with desired places equal 
                 * to current place
                 */
				for (j=i; j<first_next_level; j++) {
					node = ordering[j];
					if (desired_place[node] == cur_place) {
						block[block_len++] = node;
					}
				}
				/* Third, collect all nodes with desired places greater 
                 * than current place
                 */
				for (j=i; j<first_next_level; j++) {
					node = ordering[j];
					if (desired_place[node] > cur_place) {
						block[block_len++] = node;
					}
				}
			}
			
			/* loop through block and compute desired places of its prefixes */
			des_place_block = 0;
			block_deg = 0;
			for (i=0; i<block_len; i++) {
				node = block[i];
				toBlockConnectivity = 0;
				lap_node = lap[node];
				for (j=0; j<i; j++) {
					toBlockConnectivity -= lap_node[block[j]];
				}
				toBlockConnectivity*=2;
				/* update block stats */
				des_place_block = (block_deg*des_place_block+(-lap_node[node])*desired_place[node]+toBlockConnectivity*cur_place)/(block_deg-lap_node[node]+toBlockConnectivity);
				prefix_desired_place[i] = des_place_block;
				block_deg += toBlockConnectivity-lap_node[node];				
			}

			if (block_len==n) {  
				/* fix is needed since denominator was 0 in this case */
				prefix_desired_place[n-1] = cur_place; /* a "neutral" value */
			}

			/* loop through block and compute desired places of its suffixes */
			des_place_block = 0;
			block_deg = 0;
			for (i=block_len-1; i>=0; i--) {
				node = block[i];
				toBlockConnectivity = 0;
				lap_node = lap[node];
				for (j=i+1; j<block_len; j++) {
					toBlockConnectivity -= lap_node[block[j]];
				}
				toBlockConnectivity*=2;
				/* update block stats */
				des_place_block = (block_deg*des_place_block+(-lap_node[node])*desired_place[node]+toBlockConnectivity*cur_place)/(block_deg-lap_node[node]+toBlockConnectivity);
				suffix_desired_place[i] = des_place_block;
				block_deg += toBlockConnectivity-lap_node[node];				
			}
			
			if (block_len==n) { 
				/* fix is needed since denominator was 0 in this case */
				suffix_desired_place[0] = cur_place; /* a "neutral" value? */
			}

		
			/* now, find best place to split block */
			best_i = -1;
			max_movement = 0;
			for (i=0; i<block_len; i++) {
				suffix_des_place = suffix_desired_place[i];
				prefix_des_place = i>0 ? prefix_desired_place[i-1] : suffix_des_place;
				/* limit moves to ensure that the prefix is placed before the suffix */
				if (suffix_des_place<prefix_des_place) {
					if (suffix_des_place<cur_place) {
						if (prefix_des_place > cur_place) {
							prefix_des_place = cur_place;
						}
						suffix_des_place = prefix_des_place;
					}
					else if (prefix_des_place>cur_place) {
						prefix_des_place = suffix_des_place;
					}
				}
				movement = (block_len-i)*fabs(suffix_des_place-cur_place) + i*fabs(prefix_des_place-cur_place);
				if (movement>max_movement) {
					max_movement = movement;
					best_i = i;
				}
			}
			/* Actually move prefix and suffix */
			if (best_i>=0) {
				suffix_des_place = suffix_desired_place[best_i];
				prefix_des_place = best_i>0 ? prefix_desired_place[best_i-1] : suffix_des_place;
				
				/* compute right border of feasible move */
				if (right>=n) {
					/* no nodes after current block */
					upper_bound = 1e9;
				}
				else {
					/* notice that we have to deduct 'gap[block[block_len-1]]'
                     * since all computations should be relative to 
                     * the block's reference point
                     */
					if (lev[ordering[right]]>lev[ordering[right-1]]) {
						upper_bound = place[ordering[right]]-levels_gap-gap[block[block_len-1]];
					}
					else {
						upper_bound = place[ordering[right]]-gap[block[block_len-1]];
					}
				}
				suffix_des_place = MIN(suffix_des_place, upper_bound);
				prefix_des_place = MAX(prefix_des_place, lower_bound);
			
				/* limit moves to ensure that the prefix is placed before the suffix */
				if (suffix_des_place<prefix_des_place) {
					if (suffix_des_place<cur_place) {
						if (prefix_des_place > cur_place) {
							prefix_des_place = cur_place;
						}
						suffix_des_place = prefix_des_place;
					}
					else if (prefix_des_place>cur_place) {
						prefix_des_place = suffix_des_place;
					}
				}

				/* move prefix: */
				for (i=0; i<best_i; i++) {
					place[block[i]] = prefix_des_place+gap[block[i]];
				}
				/* move suffix: */
				for (i=best_i; i<block_len; i++) {
					place[block[i]] = suffix_des_place+gap[block[i]];
				}

		
				/* compute lower bound for next block */
				if (right<n && lev[ordering[right]]>lev[ordering[right-1]]) {
					lower_bound = place[block[block_len-1]]+levels_gap;
				}
				else {
					lower_bound = place[block[block_len-1]];
				}
				

				/* reorder 'ordering' to reflect change of places
				 * Note that it is enough to reorder the level where 
                 * the split was done
                 */
#if 0
				int max_in_level, min_in_level;
				
				level = lev[best_i];
				if (level==num_levels) {
					/* last_level */
					max_in_level = MIN(right,n);		
				}
				else {
					max_in_level = MIN(right,levels[level]);
				}
				if (level==0) {
					/* first level */
					min_in_level = MAX(left,0);		
				}
				else {
					min_in_level = MAX(left,levels[level-1]);
				}
#endif
				for (i=left; i<right; i++) {
					ordering[i] = block[i-left];
				}
				converged = converged && fabs(prefix_des_place-cur_place)<quad_prog_tol  
								      && fabs(suffix_des_place-cur_place)<quad_prog_tol;

		
			}
			else {
				/* no movement */
				/* compute lower bound for next block */
				if (right<n && lev[ordering[right]]>lev[ordering[right-1]]) {
					lower_bound = place[block[block_len-1]]+levels_gap;
				}
				else {
					lower_bound = place[block[block_len-1]];
				}
			}	
		}
		orthog1f(n, place); /* for numerical stability, keep ||place|| small */
		computeHierarchyBoundaries(place, n, ordering, levels, num_levels, hierarchy_boundaries);
	}
	
	return counter;
}

void
deleteCMajEnv(CMajEnv *e) 
{
	free (e->A[0]); 
    free (e->A);
	free (e->lev);
	free (e->fArray1);
	free (e->fArray2);
	free (e->fArray3);
	free (e->fArray4);
	free (e->iArray1);
	free (e->iArray2);
	free (e->iArray3);
	free (e->iArray4);
	free (e);
}

CMajEnv*
initConstrainedMajorization(float* packedMat, int n, int* ordering, 
                            int* levels, int num_levels)
{
	int i, level=-1, start_of_level_above=0;
	CMajEnv *e = GNEW(CMajEnv);
	e->A=NULL;
	e->n=n;
	e->ordering=ordering;
	e->levels=levels;
	e->num_levels=num_levels;
	e->A = unpackMatrix(packedMat,n);
	e->lev = N_GNEW(n, int);
	for (i=0; i<e->n; i++) {
		if (i>=start_of_level_above) {
			level++;
			start_of_level_above=(level==num_levels)?e->n:levels[level];
		}
		e->lev[ordering[i]]=level;
	}	
	e->fArray1 = N_GNEW(n,float);
	e->fArray2 = N_GNEW(n,float);
	e->fArray3 = N_GNEW(n,float);
	e->fArray4 = N_GNEW(n,float);
	e->iArray1 = N_GNEW(n,int);
	e->iArray2 = N_GNEW(n,int);
	e->iArray3 = N_GNEW(n,int);
	e->iArray4 = N_GNEW(n,int);
	return e;
}
#endif /* DIGCOLA */

