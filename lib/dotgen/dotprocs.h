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

#ifndef DOTPROCS_H
#define DOTPROCS_H

#ifdef _BEGIN_EXTERNS_
_BEGIN_EXTERNS_			/* public data */
#endif
/* tabs at 8, or you're a sorry loser */
#ifdef __cplusplus
extern "C" {
#endif

    extern void acyclic(Agraph_t *);
    extern void add_to_component(Agnode_t *);
    extern void allocate_ranks(Agraph_t *);
    extern void basic_merge(Agedge_t *, Agedge_t *);
    extern void begin_component(void);
    extern void build_ranks(Agraph_t *, int);
    extern void build_skeleton(Agraph_t *, Agraph_t *);
    extern void class1(Agraph_t *);
    extern void class2(Agraph_t *);
    extern void cleanup1(Agraph_t *);
    extern Agnode_t *clone_vn(Agraph_t *, Agnode_t *);
    extern void cluster_leader(Agraph_t *);
    extern void collapse_cluster(Agraph_t *, Agraph_t *);
    extern void collapse_leaves(Agraph_t *);
    extern void collapse_rankset(Agraph_t *, Agraph_t *, int);
    extern void collapse_sets(Agraph_t *);
    extern void decompose(Agraph_t *, int);
    extern void delete_fast_edge(Agedge_t *);
    extern void delete_fast_node(Agraph_t *, Agnode_t *);
    extern void delete_flat_edge(Agedge_t *);
    extern void delete_other_edge(Agedge_t *);
    extern void dfs(Agnode_t *);
    extern void do_ordering(Agraph_t *, int);
    extern void dot_cleanup(graph_t * g);
    extern void dot_cleanup_edge(edge_t * e);
    extern void dot_cleanup_graph(graph_t * g);
    extern void dot_cleanup_node(node_t * n);
    extern void dot_free_splines(edge_t * e);
    extern void dot_init_node_edge(Agraph_t *);
    extern void dot_init_edge(Agedge_t *);
    extern void dot_init_node(Agnode_t *);
    extern void dot_layout(Agraph_t * g);
    extern void dot_init_graph(graph_t * g);
    extern void edgelabel_ranks(Agraph_t *);
    extern void end_component(void);
    extern void exchange(Agnode_t *, Agnode_t *);
    extern void expand_cluster(Agraph_t *);
    extern void expand_ranksets(Agraph_t *);
    extern Agedge_t *fast_edge(Agedge_t *);
    extern void fast_node(Agraph_t *, Agnode_t *);
    extern void fast_nodeapp(Agnode_t *, Agnode_t *);
    extern void find_clusters(Agraph_t *);
    extern Agedge_t *find_fast_edge(Agnode_t *, Agnode_t *);
    extern Agnode_t *find_fast_node(Agraph_t *, Agnode_t *);
    extern Agedge_t *find_flat_edge(Agnode_t *, Agnode_t *);
    extern void flat_edge(Agraph_t *, Agedge_t *);
    extern int flat_edges(Agraph_t *);
    extern int flat_mval(Agnode_t *);
    extern int in_cross(Agnode_t *, Agnode_t *);
    extern void incr_width(Agraph_t *, Agnode_t *);
    extern int inside_cluster(Agraph_t *, Agnode_t *);
    extern void install_cluster(Agraph_t *, Agnode_t *, int, queue *);
    extern void install_in_rank(Agraph_t *, Agnode_t *);
    extern void interclexp(Agraph_t *);
    extern void interclrep(Agraph_t *, Agedge_t *);
    extern void interclust1(Agraph_t *, Agnode_t *, Agnode_t *,
			    Agedge_t *);
    extern int is_a_normal_node_of(Agraph_t *, Agnode_t *);
    extern int is_a_vnode_of_an_edge_of(Agraph_t *, Agnode_t *);
    extern int is_cluster(Agraph_t *);
    extern int is_cluster_edge(Agedge_t *);
    extern int is_fast_node(Agraph_t *, Agnode_t *);
    extern Agnode_t *label_vnode(Agraph_t *, Agedge_t *);
    extern Agnode_t *leader_of(Agraph_t *, Agnode_t *);
    extern int left2right(Agraph_t *, Agnode_t *, Agnode_t *);
    extern int local_cross(elist, int);
    extern void dot_compoundEdges(Agraph_t *);
    extern Agedge_t *make_aux_edge(Agnode_t *, Agnode_t *, int, int);
    extern void make_chain(Agraph_t *, Agnode_t *, Agnode_t *, Agedge_t *);
    extern void make_interclust_chain(Agraph_t *, Agnode_t *, Agnode_t *,
				      Agedge_t *);
    extern int make_new_cluster(Agraph_t *, Agraph_t *);
    extern void make_slots(Agraph_t *, int, int, int);
    extern Agnode_t *map_interclust_node(Agnode_t *);
    extern void map_path(Agnode_t *, Agnode_t *, Agedge_t *, Agedge_t *,
			 int);
    extern void mark_clusters(Agraph_t *);
    extern void mark_lowclusters(Agraph_t *);
    extern int mergeable(edge_t * e, edge_t * f);
    extern void merge_chain(Agraph_t *, Agedge_t *, Agedge_t *, int);
    extern void merge_components(Agraph_t *);
    extern Agnode_t *merge_leaves(Agraph_t *, Agnode_t *, Agnode_t *);
    extern void merge_oneway(Agedge_t *, Agedge_t *);
    extern void merge_ranks(Agraph_t *);
    extern void minmax_edges(Agraph_t *);
    extern int ncross(Agraph_t *);
    extern Agedge_t *new_virtual_edge(Agnode_t *, Agnode_t *, Agedge_t *);
    extern void node_induce(Agraph_t *, Agraph_t *);
    extern int nonconstraint_edge(Agedge_t *);
    extern int ordercmpf(int *, int *);
    extern void ordered_edges(Agraph_t *);
    extern void other_edge(Agedge_t *);
    extern int out_cross(Agnode_t *, Agnode_t *);
    extern Agnode_t *plain_vnode(Agraph_t *, Agedge_t *);
    extern int portcmp(port p0, port p1);
    extern void potential_leaf(Agraph_t *, Agedge_t *, Agnode_t *);
    extern int ports_eq(edge_t *, edge_t *);
    extern void rank1(Agraph_t *);
    extern void rank(Agraph_t *, int, int);
    extern int rank_set_class(Agraph_t *);
    extern int rcross(Agraph_t *, int);
    extern void rec_reset_vlists(Agraph_t *);
    extern void rec_save_vlists(Agraph_t *);
    extern void remove_rankleaders(Agraph_t *);
    extern void renewlist(elist *);
    extern void reorder(Agraph_t *, int, int, int);
    extern void reverse_edge(Agedge_t *);
    extern void safe_delete_fast_edge(Agedge_t *);
    extern void safe_list_append(Agedge_t *, elist *);
    extern void safe_other_edge(Agedge_t *);
    extern void save_vlist(Agraph_t *);
    extern void scan_ranks(Agraph_t *);
    extern void scan_result(void);
    extern void search_component(Agraph_t *, Agnode_t *);
    extern void set_minmax(Agraph_t *);
    extern void setup_page(Agraph_t *, point);
    extern int strccnt(char *, char);
    extern void transpose(Agraph_t *, int);
    extern int transpose_step(Agraph_t *, int, int);
    extern void unmerge_oneway(Agedge_t *);
    extern void update(Agedge_t *, Agedge_t *);
    extern void update_bb(Agraph_t *, point);
    extern Agedge_t *virtual_edge(Agnode_t *, Agnode_t *, Agedge_t *);
    extern Agnode_t *virtual_node(Agraph_t *);
    extern void virtual_weight(Agedge_t *);
    extern void zapinlist(elist *, Agedge_t *);

#if defined(_BLD_dot) && defined(_DLL)
#   define extern __EXPORT__
#endif
    extern void dot_nodesize(Agnode_t *, boolean);
    extern void dot_concentrate(Agraph_t *);
    extern void dot_mincross(Agraph_t *);
    extern void dot_position(Agraph_t *);
    extern void dot_rank(Agraph_t *);
    extern void dot_sameports(Agraph_t *);
    extern void dot_splines(Agraph_t *);
#undef extern

#ifdef _END_EXTERNS_
     _END_EXTERNS_
#endif
#ifdef __cplusplus
}
#endif
#endif
