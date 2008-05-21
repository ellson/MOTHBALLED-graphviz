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
#include "colorprocs.h"
#include "viewport.h"
#include "memory.h"

void on_settingsOKBtn_clicked(GtkWidget * widget, gpointer user_data)
{
    update_graph_from_settings(view->g[view->activeGraph]);
    set_viewport_settings_from_template(view, view->g[view->activeGraph]);
    gtk_widget_hide(glade_xml_get_widget(xml, "dlgSettings"));

}

void on_settingsCancelBtn_clicked(GtkWidget * widget, gpointer user_data)
{
    gtk_widget_hide(glade_xml_get_widget(xml, "dlgSettings"));
}


static int set_color_button_widget(char *attribute, char *widget_name)
{
    GdkColor color;
    gvcolor_t cl;

    char *buf;
    buf = agget(view->g[view->activeGraph], attribute);
    if ((!buf) || (strcmp(buf, "") == 0))
	buf = agget(view->default_attributes, attribute);
    if (buf) {
	colorxlate(buf, &cl, RGBA_DOUBLE);
	color.red = (int) (cl.u.RGBA[0] * 65535.0);
	color.green = (int) (cl.u.RGBA[1] * 65535.0);
	color.blue = (int) (cl.u.RGBA[2] * 65535.0);
//              gdk_color_parse(buf,&color);
	gtk_color_button_set_color((GtkColorButton *)
				   glade_xml_get_widget(xml, widget_name),
				   &color);
	return 1;

    }
    return 0;
}
static int get_color_button_widget_to_attribute(char *attribute,
						char *widget_name,
						Agraph_t * g)
{
    GdkColor color;
    char *buf = N_GNEW(256,char);
    gtk_color_button_get_color((GtkColorButton *)
			       glade_xml_get_widget(xml, widget_name),
			       &color);
    sprintf(buf, "#%02x%02x%02x",
	    (int) ((float) color.red / 65535.0 * 255.0),
	    (int) ((float) color.green / 65535.0 * 255.0),
	    (int) ((float) color.blue / 65535.0 * 255.0));
    agattr(g, AGRAPH, attribute, buf);
    free(buf);
    return 1;
}

static int set_checkbox_widget(char *attribute, char *widget_name)
{
    char *buf;
    int value;
    buf = agget(view->g[view->activeGraph], attribute);
    if ((!buf) || (strcmp(buf, "") == 0))
	buf = agget(view->default_attributes, attribute);
    if (buf) {
	value = atoi(buf);
	gtk_toggle_button_set_active((GtkToggleButton *)
				     glade_xml_get_widget(xml,
							  widget_name),
				     value);
	return 1;
    }
    return 0;


}

static int get_checkbox_widget_to_attribute(char *attribute,
					    char *widget_name,
					    Agraph_t * g)
{
    int value;
    char buf[100];
    value =
	(int) gtk_toggle_button_get_active((GtkToggleButton *)
					   glade_xml_get_widget(xml,
								widget_name));
    sprintf (buf, "%d", value);
    agattr(g, AGRAPH, attribute, buf);
    return 1;
}

static int set_spinbtn_widget(char *attribute, char *widget_name)
{
    char *buf;
    float value;
    buf = agget(view->g[view->activeGraph], attribute);
    if ((!buf) || (strcmp(buf, "") == 0))
	buf = agget(view->default_attributes, attribute);
    if (buf) {
	value = (float) atof(buf);
	gtk_spin_button_set_value((GtkSpinButton *)
				  glade_xml_get_widget(xml, widget_name),
				  value);
	return 1;
    }
    return 0;
}
static int get_spinbtn_widget_to_attribute(char *attribute,
					   char *widget_name, Agraph_t * g)
{
    float value;
    char buf[25];
    value =
	(float) gtk_spin_button_get_value((GtkSpinButton *)
					  glade_xml_get_widget(xml,
							       widget_name));
    sprintf(buf, "%f", value);
    //      agattr(
    agattr(g, AGRAPH, attribute, buf);
    return 1;
}


int load_settings_from_graph(Agraph_t * g)
{
    char *buf;

    set_color_button_widget("bgcolor", "settingsColorBtn1");
    set_color_button_widget("bordercolor", "settingsColorBtn2");
    set_color_button_widget("gridcolor", "settingsColorBtn3");
    set_color_button_widget("highlightednodecolor", "settingsColorBtn6");
    set_color_button_widget("highlightededgecolor", "settingsColorBtn7");
    set_color_button_widget("selectednodecolor", "settingsColorBtn8");
    set_color_button_widget("selectededgecolor", "settingsColorBtn9");
    get_color_button_widget_to_attribute("topologicaltopviewfinestcolor",
					 "settingsColorBtn9", g);
    get_color_button_widget_to_attribute("topologicaltopviewcoarsestcolor",
					 "settingsColorBtn9", g);

    set_color_button_widget("topologicaltopviewfinestcolor",
			    "settingsColorBtn10");
    set_color_button_widget("topologicaltopviewcoarsestcolor",
			    "settingsColorBtn11");


    set_checkbox_widget("bordervisible", "settingsChkBox2");
    set_checkbox_widget("gridvisible", "settingsChkBox3");
    set_checkbox_widget("randomizenodecolors", "settingsChkBox4");
    set_checkbox_widget("randomizeedgecolors", "settingsChkBox5");

	set_checkbox_widget("drawnodes", "settingsChkBox5-1");
	set_checkbox_widget("drawedges", "settingsChkBox5-2");
	set_checkbox_widget("drawlabels", "settingsChkBox5-3");

	set_checkbox_widget("usermode", "settingsChkBox10");
    set_checkbox_widget("nodesizewithdegree", "settingsChkBox11");
    set_checkbox_widget("antialiasing", "settingsChkBox12");

    set_checkbox_widget("topologicalfisheyedist2limit",
			"settingsChkBox13");
    set_checkbox_widget("topologicalfisheyeanimate", "settingsChkBox14");
    set_checkbox_widget("topologicalfisheyelabelfinenodes",
			"settingsChkBox15");
    set_checkbox_widget("topologicalfisheyecolornodes",
			"settingsChkBox16");
    set_checkbox_widget("topologicalfisheyecoloredges",
			"settingsChkBox17");
    set_checkbox_widget("topologicalfisheyelabelfocus",
			"settingsChkBox18");

    set_spinbtn_widget("defaultmagnifierwidth", "settingsspinbutton1");
    set_spinbtn_widget("defaultmagnifierheight", "settingsspinbutton2");
    set_spinbtn_widget("defaultmagnifierkts", "settingsspinbutton3");
    set_spinbtn_widget("defaultfisheyemagnifierradius",
		       "settingsspinbutton4");
    set_spinbtn_widget("defaultfisheyemagnifierdistort",
		       "settingsspinbutton5");
    set_spinbtn_widget("topologicalfisheyefinenodes",
		       "settingsspinbutton6");
    set_spinbtn_widget("topologicalfisheyecoarseningfactor",
		       "settingsspinbutton7");
    set_spinbtn_widget("topologicalfisheyedistortionfactor",
		       "settingsspinbutton8");


    /*font selection box */
    buf = agget(view->g[view->activeGraph], "defaultfontname");
    if (!buf)
	buf = agget(view->default_attributes, "defaultfontname");
    if (buf)
	gtk_font_selection_set_font_name((GtkFontSelection *)
					 glade_xml_get_widget(xml,
							      "settingsFontSelection"),
					 buf);
    /*Node Shape Combo, 0:opengl dots, 1:circle ,2:box */
    buf = agget(view->g[view->activeGraph], "defaultnodeshape");
    if (!buf)
	buf = agget(view->default_attributes, "defaultnodeshape");
    if (buf) {
	/*select the right item in combo box */
	gtk_combo_box_set_active((GtkComboBox *)
				 glade_xml_get_widget(xml,
						      "settingsFontSelection"),
				 atoi(buf));
    }
    return 1;
}

/*burak
917 749 6080*/


int update_graph_from_settings(Agraph_t * g)
{
    gchar *buf;
    int value;
    char buf2[10];
    buf = '\0';
/*	buf=gtk_font_selection_get_font_name((GtkFontSelection*)glade_xml_get_widget(xml, "settingsFontSelection"));
	agset(g,"defaultfontname",buf);*/
    value =
	gtk_combo_box_get_active((GtkComboBox *)
				 glade_xml_get_widget(xml,
						      "settingsFontSelection"));
    sprintf(buf2, "%i", value);
    agattr(g, AGRAPH, "defaultfontname", buf2);
    get_color_button_widget_to_attribute("bgcolor", "settingsColorBtn1",
					 g);
    get_color_button_widget_to_attribute("bordercolor",
					 "settingsColorBtn2", g);
    get_color_button_widget_to_attribute("gridcolor", "settingsColorBtn3",
					 g);
    get_color_button_widget_to_attribute("highlightednodecolor",
					 "settingsColorBtn6", g);
    get_color_button_widget_to_attribute("highlightededgecolor",
					 "settingsColorBtn7", g);
    get_color_button_widget_to_attribute("selectednodecolor",
					 "settingsColorBtn8", g);
    get_color_button_widget_to_attribute("selectededgecolor",
					 "settingsColorBtn9", g);

    get_color_button_widget_to_attribute("topologicalfisheyefinestcolor",
					 "settingsColorBtn10", g);
    get_color_button_widget_to_attribute("topologicalfisheyecoarsestcolor",
					 "settingsColorBtn11", g);

    get_checkbox_widget_to_attribute("bordervisible", "settingsChkBox2",
				     g);
    get_checkbox_widget_to_attribute("gridvisible", "settingsChkBox3", g);
    get_checkbox_widget_to_attribute("randomizenodecolors",
				     "settingsChkBox4", g);
    get_checkbox_widget_to_attribute("randomizeedgecolors",
				     "settingsChkBox5", g);

    get_checkbox_widget_to_attribute("drawnodes",
				     "settingsChkBox5-1", g);
    get_checkbox_widget_to_attribute("drawedges",
				     "settingsChkBox5-2", g);
    get_checkbox_widget_to_attribute("drawlabels",
				     "settingsChkBox5-3", g);
	
	
	
	get_checkbox_widget_to_attribute("usermode", "settingsChkBox10", g);
    get_checkbox_widget_to_attribute("nodesizewithdegree",
				     "settingsChkBox11", g);
    get_checkbox_widget_to_attribute("antialiasing", "settingsChkBox12",
				     g);

    get_checkbox_widget_to_attribute("topologicalfisheyedist2limit",
				     "settingsChkBox13", g);
    get_checkbox_widget_to_attribute("topologicalfisheyeanimate",
				     "settingsChkBox14", g);
    get_checkbox_widget_to_attribute("topologicalfisheyelabelfinenodes",
				     "settingsChkBox15", g);
    get_checkbox_widget_to_attribute("topologicalfisheyecolornodes",
				     "settingsChkBox16", g);
    get_checkbox_widget_to_attribute("topologicalfisheyecoloredges",
				     "settingsChkBox17", g);
    get_checkbox_widget_to_attribute("topologicalfisheyelabelfocus",
				     "settingsChkBox18", g);

    get_spinbtn_widget_to_attribute("defaultmagnifierwidth",
				    "settingsspinbutton1", g);
    get_spinbtn_widget_to_attribute("defaultmagnifierheight",
				    "settingsspinbutton2", g);
    get_spinbtn_widget_to_attribute("defaultmagnifierkts",
				    "settingsspinbutton3", g);
    get_spinbtn_widget_to_attribute("defaultfisheyemagnifierradius",
				    "settingsspinbutton4", g);
    get_spinbtn_widget_to_attribute("defaultfisheyemagnifierdistort",
				    "settingsspinbutton5", g);
    get_spinbtn_widget_to_attribute("topologicalfisheyefinenodes",
				    "settingsspinbutton6", g);
    get_spinbtn_widget_to_attribute("topologicalfisheyecoarseningfactor",
				    "settingsspinbutton7", g);
    get_spinbtn_widget_to_attribute("topologicalfisheyedistortionfactor",
				    "settingsspinbutton8", g);

    return 1;
}

int show_settings_form()
{
    load_settings_from_graph(view->g[view->activeGraph]);
    gtk_widget_hide(glade_xml_get_widget(xml, "dlgSettings"));
    gtk_widget_show(glade_xml_get_widget(xml, "dlgSettings"));
    gtk_window_set_keep_above((GtkWindow *)
			      glade_xml_get_widget(xml, "dlgSettings"), 1);
    return 1;
}
