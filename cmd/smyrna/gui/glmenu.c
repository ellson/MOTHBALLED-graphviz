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

#include "glmenu.h"
#include "gltemplate.h"
/*
	creates the right click menu for open gl canvas
	params:gtk widget to assign menu
	return value:gtkmenu itself
*/
GtkWidget *create_popup_menu(GtkWidget * drawing_area)
{
    GtkWidget *actions_menu;
    GtkWidget *editing_menu;
    GtkWidget *menu;
    GtkWidget *menu_item;
    int mm = 0;
    /*actions sub menu */
    //PAN   

    mm = MM_PAN;
    actions_menu = gtk_menu_new();
    menu_item = gtk_menu_item_new_with_label("Pan");
    gtk_menu_shell_append(GTK_MENU_SHELL(actions_menu), menu_item);
    g_signal_connect(G_OBJECT(menu_item), "activate",
		     G_CALLBACK(switch_Mouse), (gpointer) mm);
    gtk_widget_show(menu_item);
/**********/
    //ZOOM
    mm = MM_ZOOM;
    menu_item = gtk_menu_item_new_with_label("Zoom");
    gtk_menu_shell_append(GTK_MENU_SHELL(actions_menu), menu_item);
    g_signal_connect(G_OBJECT(menu_item), "activate",
		     G_CALLBACK(switch_Mouse), (gpointer) mm);
    gtk_widget_show(menu_item);
/**********/
    //ROTATE
    mm = MM_ROTATE;
    menu_item = gtk_menu_item_new_with_label("rotate");
    gtk_menu_shell_append(GTK_MENU_SHELL(actions_menu), menu_item);
    g_signal_connect(G_OBJECT(menu_item), "activate",
		     G_CALLBACK(switch_Mouse), (gpointer) mm);
    gtk_widget_show(menu_item);
/**********/
/**********/
    //Single Select
    mm = MM_SINGLE_SELECT;
    menu_item = gtk_menu_item_new_with_label("select");
    gtk_menu_shell_append(GTK_MENU_SHELL(actions_menu), menu_item);
    g_signal_connect(G_OBJECT(menu_item), "activate",
		     G_CALLBACK(switch_Mouse), (gpointer) mm);
    gtk_widget_show(menu_item);
/**********/
    //Rectangle Select
    mm = MM_RECTANGULAR_SELECT;
    menu_item = gtk_menu_item_new_with_label("rect select");
    gtk_menu_shell_append(GTK_MENU_SHELL(actions_menu), menu_item);
    g_signal_connect(G_OBJECT(menu_item), "activate",
		     G_CALLBACK(switch_Mouse), (gpointer) mm);
    gtk_widget_show(menu_item);
/**********/
/**********/
    //Rectangle -x Select
    mm = MM_RECTANGULAR_X_SELECT;
    menu_item = gtk_menu_item_new_with_label("rect-x select");
    gtk_menu_shell_append(GTK_MENU_SHELL(actions_menu), menu_item);
    g_signal_connect(G_OBJECT(menu_item), "activate",
		     G_CALLBACK(switch_Mouse), (gpointer) mm);
    gtk_widget_show(menu_item);
/**********/
/**********/
    //Move
    mm = MM_MOVE;
    menu_item = gtk_menu_item_new_with_label("Move");
    gtk_menu_shell_append(GTK_MENU_SHELL(actions_menu), menu_item);
    g_signal_connect(G_OBJECT(menu_item), "activate",
		     G_CALLBACK(switch_Mouse), (gpointer) mm);
    gtk_widget_show(menu_item);
/**********/
    //activate magnifier
    mm = MM_MAGNIFIER;		//magnifier ,fisheye etc starts at 20
    menu_item = gtk_menu_item_new_with_label("Magnifier");
    gtk_menu_shell_append(GTK_MENU_SHELL(actions_menu), menu_item);
    g_signal_connect(G_OBJECT(menu_item), "activate",
		     G_CALLBACK(switch_Mouse), (gpointer) mm);
    gtk_widget_show(menu_item);
/**********/
    //activate fisheye magnifier
    mm = MM_FISHEYE_MAGNIFIER;
    menu_item = gtk_menu_item_new_with_label("Fisheye Magnifier");
    gtk_menu_shell_append(GTK_MENU_SHELL(actions_menu), menu_item);
    g_signal_connect(G_OBJECT(menu_item), "activate",
		     G_CALLBACK(switch_Mouse), (gpointer) mm);
    gtk_widget_show(menu_item);
/**********/
    editing_menu = gtk_menu_new();
    /* NODE */
    menu_item = gtk_menu_item_new_with_label("Node");
    gtk_menu_shell_append(GTK_MENU_SHELL(editing_menu), menu_item);
    gtk_widget_show(menu_item);

    /* EDGE */
    menu_item = gtk_menu_item_new_with_label("Edge");
    gtk_menu_shell_append(GTK_MENU_SHELL(editing_menu), menu_item);
    gtk_widget_show(menu_item);


    menu = gtk_menu_new();

    /* Actions */
    menu_item = gtk_menu_item_new_with_label("Mouse");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_item), actions_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);
    gtk_widget_show(menu_item);

    /* NEW */
    menu_item = gtk_menu_item_new_with_label("New");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_item), editing_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);
    gtk_widget_show(menu_item);

    /* Quit */
    menu_item = gtk_menu_item_new_with_label("Quit");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);
    g_signal_connect(G_OBJECT(menu_item), "activate",
		     G_CALLBACK(gtk_main_quit), NULL);
    gtk_widget_show(menu_item);
    return menu;
}
