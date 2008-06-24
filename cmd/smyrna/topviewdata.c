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

#include "topviewdata.h"
#include "btree.h"
#include <memory.h>

static int validate_group_node(tv_node * TV_Node, char *regex_string)
{
    btree_node *n = 0;
//              n=tree_from_filter_string("([IP=\"^10.*\",min=\"0\",max=\"0\"])");
    int valid = 0;
    n = tree_from_filter_string(regex_string);
    valid = evaluate_expresions(TV_Node, n);
//      delete_node(n); 
    return valid;
}

int prepare_nodes_for_groups(topview * t, topviewdata * td, int groupindex)
{
    GdkColor color;
    int i;
    int count = 0;
    tv_node tvn;
    gtk_color_button_get_color(td->gtkhostcolor[0], &color);

    for (i = 0; i < t->Nodecount; i++) {
	tvn.index = i;
	if (validate_group_node(&tvn, td->hostregex[groupindex])) {
	    count++;
	    gtk_color_button_get_color(td->gtkhostcolor[groupindex],
				       &color);
	    t->Nodes[i].GroupIndex = groupindex;
	    t->Nodes[i].GroupColor.R = (float) color.red / (float) 65535.0;
	    t->Nodes[i].GroupColor.G =
		(float) color.green / (float) 65535.0;
	    t->Nodes[i].GroupColor.B =
		(float) color.blue / (float) 65535.0;
	}
    }
    return 1;
}

int load_host_buttons(topview * t, Agraph_t * g, glCompSet * s)
{
    GtkLayout *layout;
    int btncount = 0;
    int i = 0;
    char *str;
    char hostbtncaption[50];
    char hostbtnregex[50];
    char hostbtncolorR[50];
    char hostbtncolorG[50];
    char hostbtncolorB[50];
    char hostbtncolorA[50];
    int X = 10;
    int Y = 25;
    GdkColor color;
    glCompPanel *p;
    glCompButton *b;

    layout =
	(GtkLayout *) glade_xml_get_widget(xml, "frmHostSelectionFixed");
    str = '\0';
    str = agget(g, "hostbtncount");
    if (str)
	btncount = atoi(str);

//      Graph [hostbtncaption1="AT&T",hostbtnregex1="*.ATT*",hostbtncolorR1="1",hostbtncolorG1="0",hostbtncolorB1="0",hostbtncolorA1="1"];

    t->TopviewData->hostregex = N_GNEW(btncount, char *);
    t->TopviewData->gtkhostbtn = N_GNEW(btncount, GtkButton *);
    t->TopviewData->gtkhostcolor = N_GNEW(btncount, GtkColorButton *);
    t->TopviewData->gtkhostbtncount = btncount;
    if (btncount > 0) {
	p = glCompPanelNew(25, 75, 165, 400);
	p->data = 2;		//data panel
	p->color.R = (float) 0.80;
	p->color.B = (float) 0.2;
	glCompSetAddPanel(s, p);
    } else
	return 0;

    for (i = 0; i < btncount; i++) {
	sprintf(hostbtncaption, "hostbtncaption%i", i);
	sprintf(hostbtnregex, "hostbtnregex%i", i);
	sprintf(hostbtncolorR, "hostbtncolorR%i", i);
	sprintf(hostbtncolorG, "hostbtncolorG%i", i);
	sprintf(hostbtncolorB, "hostbtncolorB%i", i);
	sprintf(hostbtncolorA, "hostbtncolorA%i", i);
	agget(g, hostbtncaption),
	    agget(g, hostbtnregex),
	    agget(g, hostbtncolorR),
	    agget(g, hostbtncolorG),
	    agget(g, hostbtncolorB), agget(g, hostbtncolorA);
	t->TopviewData->hostregex[i] = agget(g, hostbtnregex);

	b = glCompButtonNew((GLfloat) 5,
			    (GLfloat) 7 + ((GLfloat) i +
					   (GLfloat) 1) * (GLfloat) 36,
			    (GLfloat) 150, (GLfloat) 35, agget(g,
							       hostbtncaption),
			    '\0', 0, 0);
	b->color.R = (float) atof(agget(g, hostbtncolorR));
	b->color.G = (float) atof(agget(g, hostbtncolorG));
	b->color.B = (float) atof(agget(g, hostbtncolorB));
	b->color.A = (float) 1;
	b->panel = p;
	b->groupid = -1;
	b->callbackfunc = glhost_button_clicked_Slot;
	b->data = i;
	glCompSetAddButton(s, b);

	t->TopviewData->gtkhostbtn[i] = (GtkButton *)
	    gtk_button_new_with_label(agget(g, hostbtncaption));
	g_signal_connect((gpointer) t->TopviewData->gtkhostbtn[i],
			 "clicked", G_CALLBACK(host_button_clicked_Slot),
			 (gpointer) i);

	color.blue = 65535 * atoi(agget(g, hostbtncolorB));
	color.red = 65535 * atoi(agget(g, hostbtncolorR));
	color.green = 65535 * atoi(agget(g, hostbtncolorG));

	t->TopviewData->gtkhostcolor[i] =
	    (GtkColorButton *) gtk_color_button_new_with_color(&color);

	gtk_color_button_set_alpha((GtkColorButton *) t->TopviewData->
				   gtkhostbtn[i],
				   65535 * atoi(agget(g, hostbtncolorA)));


	gtk_layout_put(layout, (GtkWidget *) t->TopviewData->gtkhostbtn[i],
		       X, Y);
	gtk_widget_set_size_request((GtkWidget *) t->TopviewData->
				    gtkhostbtn[i], 200, 35);

	gtk_layout_put(layout,
		       (GtkWidget *) t->TopviewData->gtkhostcolor[i],
		       X + 225, Y);
	gtk_widget_set_size_request((GtkWidget *) t->TopviewData->
				    gtkhostcolor[i], 40, 35);

	gtk_widget_show((GtkWidget *) t->TopviewData->gtkhostbtn[i]);
	gtk_widget_show((GtkWidget *) t->TopviewData->gtkhostcolor[i]);
	Y = Y + 40;
	t->TopviewData->hostactive[i] = 0;
    }
    p->height = (GLfloat) 15 + (GLfloat) (btncount + 1) * (GLfloat) 36;
    for (i = 0; i < btncount; i++) {
	prepare_nodes_for_groups(t, t->TopviewData, i);
    }
    return 1;
}
void glhost_button_clicked_Slot(void *p)
{
    //negative active
    int user_data;
    user_data = ((glCompButton *) p)->data;
    if (view->Topview->TopviewData->hostactive[user_data] == 0)
	view->Topview->TopviewData->hostactive[user_data] = 1;
    else
	view->Topview->TopviewData->hostactive[user_data] = 0;
    glexpose();
}

void host_button_clicked_Slot(GtkWidget * widget, gpointer user_data)
{
    //negative active
    if (view->Topview->TopviewData->hostactive[(int) user_data] == 0)
	view->Topview->TopviewData->hostactive[(int) user_data] = 1;
    else
	view->Topview->TopviewData->hostactive[(int) user_data] = 0;
    glexpose();
}
