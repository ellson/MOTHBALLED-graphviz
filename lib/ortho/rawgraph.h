/* vim:set shiftwidth=4 ts=8: */
#ifndef RAWGRAPH_H
#define RAWGRAPH_H

#include <cdt.h>

typedef struct {
  int color;
  int topsort_order;
  Dt_t* adj_list;  /* adj_list */
} vertex;

typedef struct {
  int nvs;
  vertex* vertices;
} rawgraph;

extern rawgraph* make_graph(int n);  /* makes a graph wih n vertices, 0 edges */
extern void free_graph(rawgraph*); 
  /* inserts edge FROM v1 to v2 */
extern void insert_edge(rawgraph*, int v1, int v2); 
  /* removes any edge between v1 to v2 -- irrespective of direction */
extern void remove_redge(rawgraph*, int v1, int v2);  
  /* tests if there is an edge FROM v1 TO v2 */
extern int edge_exists(rawgraph*, int v1, int v2);
  /* topologically sorts the directed graph */
extern void top_sort(rawgraph*); 

#endif
