#ifndef BEACON_H
#define BEACON_H
#include "smyrnadefs.h"

int pick_node(topview_node * n);
int is_node_picked(topview_node * n);
int remove_from_pick_list(topview_node * n);
int add_to_pick_list(topview_node * n);

#endif
