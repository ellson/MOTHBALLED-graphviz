/* vim:set shiftwidth=4 ts=8: */

/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: See CVS logs. Details at http://www.graphviz.org/
 *************************************************************************/

#ifndef TOPVIEWDATA_H
#define TOPVIEWDATA_H

#include <gtk/gtk.h>
#include "cgraph.h"
#include "smyrnadefs.h"
#include "tvnodes.h"

#ifdef __cplusplus
extern "C" {
#endif

    int prepare_nodes_for_groups(topview * t, topviewdata * td,
				 int groupindex);
    int load_host_buttons(topview * t, Agraph_t * g, glCompSet * s);
    int click_group_button(int groupindex);
    void glhost_button_clicked_Slot(void *p);
    _BB void host_button_clicked_Slot(GtkWidget * widget,
				      gpointer user_data);

#ifdef __cplusplus
}				/* end extern "C" */
#endif
#endif
