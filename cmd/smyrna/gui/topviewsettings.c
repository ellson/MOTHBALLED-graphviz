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


static int set_color_button_widget(char* attribute,char* widget_name)
{
    GdkColor color;
	char* buf;
	buf=agget(view->g[view->activeGraph],attribute);
	if (!buf)
		buf=agget(view->default_attributes,attribute);
	if (buf)
	{
		gdk_color_parse(buf,&color);
		gtk_color_button_set_color((GtkColorButton*)glade_xml_get_widget(xml, widget_name),&color);
		return 1;

	}
	return 0;


}

static int set_checkbox_widget(char* attribute,char* widget_name)
{
	char* buf;
	int value;
	buf=agget(view->g[view->activeGraph],attribute);
	if (!buf)
		buf=agget(view->default_attributes,attribute);
	if (buf)
	{
		value=atoi(buf);
		gtk_toggle_button_set_active((GtkToggleButton*)glade_xml_get_widget(xml, widget_name),value);
		return 1;
	}
	return 0;


}



int load_settings_from_graph(Agraph_t * g)
{
	char* buf;
	int value;

	set_color_button_widget("bgcolor","settingsColorBtn1");
	set_color_button_widget("bordercolor","settingsColorBtn2");
	set_color_button_widget("gridcolor","settingsColorBtn3");
	set_color_button_widget("highlightednodecolor","settingsColorBtn6");
	set_color_button_widget("highlightededgecolor","settingsColorBtn7");
	set_color_button_widget("selectednodecolor","settingsColorBtn8");
	set_color_button_widget("selectededgecolor","settingsColorBtn9");
	set_checkbox_widget("bordervisible","settingsChkBox2");
	set_checkbox_widget("gridvisible","settingsChkBox3");
	set_checkbox_widget("randomizenodecolors","settingsChkBox4");
	set_checkbox_widget("randomizeedgecolors","settingsChkBox5");
	set_checkbox_widget("usermode","settingsChkBox10");
	set_checkbox_widget("nodesizewithdegree","settingsChkBox11");
	set_checkbox_widget("antialiasing","settingsChkBox12");

	/*font selection box*/
	buf=agget(view->g[view->activeGraph],"defaultfontname");
	if (!buf)
		buf=agget(view->default_attributes,"defaultfontname");
	if (!buf)
		gtk_font_selection_set_font_name((GtkFontSelection*)glade_xml_get_widget(xml, "settingsFontSelection"),buf);
	/*Node Shape Combo, 0:opengl dots, 1:circle ,2:box */
	buf=agget(view->g[view->activeGraph],"defaultfontname");
	if (!buf)
		buf=agget(view->default_attributes,"defaultfontname");
	if (!buf)
	{
		/*select the right item in combo box*/
		gtk_combo_box_set_active ((GtkComboBox*)glade_xml_get_widget(xml, "settingsFontSelection"),atoi(buf));
	}
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
	return 1;
}
