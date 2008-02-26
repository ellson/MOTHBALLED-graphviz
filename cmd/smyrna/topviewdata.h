#ifndef TOPVIEWDATA_H
#define TOPVIEWDATA_H

#include <gtk/gtk.h>
#include "cgraph.h"
#include "smyrnadefs.h"
#include "tvnodes.h"





int prepare_nodes_for_groups(topview* t,topviewdata* td,int groupindex);
int load_host_buttons(topview* t,Agraph_t *g,glCompSet* s);
int validate_group_node(tv_node* TV_Node,char* regex_string);
int click_group_button(int groupindex);
void glhost_button_clicked_Slot(void* p);
_BB void host_button_clicked_Slot(GtkWidget *widget,gpointer     user_data);


#endif
