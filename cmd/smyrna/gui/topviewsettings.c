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

#include "topviewsettings.h"
#include "gui.h"


void on_settingsOKBtn_clicked(GtkWidget * widget, gpointer user_data)
{


}

void on_settingsCancelBtn_clicked(GtkWidget * widget, gpointer user_data)
{

}
int load_settings_from_graph(Agraph_t * g)
{

    return 1;
}

int update_graph_from_settings(Agraph_t * g)
{

    return 1;
}

int show_settings_form()
{
    gtk_widget_hide(glade_xml_get_widget(xml, "dlgSettings"));
    gtk_widget_show(glade_xml_get_widget(xml, "dlgSettings"));
    gtk_window_set_keep_above((GtkWindow *)
			      glade_xml_get_widget(xml, "dlgSettings"), 1);
}
