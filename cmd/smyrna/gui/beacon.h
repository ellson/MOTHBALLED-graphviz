#ifndef BEACON_H
#define BEACON_H
#include "smyrnadefs.h"

extern int pick_node(topview_node * n);
extern int is_node_picked(topview_node * n);
extern int remove_from_pick_list(topview_node * n);
extern int add_to_pick_list(topview_node * n);
extern int draw_node_hint_boxes(void);

#endif
