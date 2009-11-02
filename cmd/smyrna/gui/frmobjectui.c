/* $Id$ $Revision$ */
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

#include <stdio.h>

#include "compat.h"
#include <stdlib.h>
#include "gui.h"
/* #include "abstring.h" */
#include <glade/glade.h>
#include <gdk/gdkkeysyms.h>
#include <gdk/gdk.h>
#include "viewport.h"
#include "memory.h"
#include "frmobjectui.h"

#ifdef WIN32
#define STRCASECMP stricmp
#else
#include <strings.h>
#define STRCASECMP strcasecmp
#endif

static attr_t *binarySearch(attr_list * l, char *searchKey);
static int sel_node;
static int sel_edge;
static int sel_graph;

static char *safestrdup(char *src)
{
    if (!src)
	return NULL;
    else
	return strdup(src);
}
static int get_object_type()
{
    if (gtk_toggle_button_get_active
	((GtkToggleButton *) glade_xml_get_widget(xml, "attrRB0")))
	return AGRAPH;
    if (gtk_toggle_button_get_active
	((GtkToggleButton *) glade_xml_get_widget(xml, "attrRB1")))
	return AGNODE;
    if (gtk_toggle_button_get_active
	((GtkToggleButton *) glade_xml_get_widget(xml, "attrRB2")))
	return AGEDGE;
    return -1;
}

void free_attr(attr_t * at)
{
    free(at->defValG);
    free(at->defValN);
    free(at->defValE);

    free(at->name);
    free(at);
}


attr_t *new_attr()
{
    attr_t *attr = malloc(sizeof(attr_t));
    attr->defValG = (char *) 0;
    attr->defValN = (char *) 0;
    attr->defValE = (char *) 0;
    attr->name = (char *) 0;
    attr->value = (char *) 0;
    attr->propagate = 0;
    attr->objType[0] = 0;
    attr->objType[1] = 0;
    attr->objType[2] = 0;
    return attr;
}


attr_t *new_attr_with_ref(Agsym_t * sym)
{
    attr_t *attr = new_attr();
    attr->name = safestrdup(sym->name);
    switch (sym->kind) {
    case AGRAPH:
	attr->objType[0] = 1;
	if (sym->defval)
	    attr->defValG = safestrdup(sym->defval);
	break;
    case AGNODE:
	attr->objType[1] = 1;
	if (sym->defval)
	    attr->defValN = safestrdup(sym->defval);
	break;
    case AGEDGE:
	attr->objType[2] = 1;
	if (sym->defval)
	    attr->defValE = safestrdup(sym->defval);
	break;
    }
    return attr;
}

attr_t *new_attr_ref(attr_t * refAttr)
{
    attr_t *attr = malloc(sizeof(attr_t));
    *attr = *refAttr;
    attr->defValG = safestrdup(refAttr->defValG);
    attr->defValN = safestrdup(refAttr->defValN);
    attr->defValE = safestrdup(refAttr->defValE);
    attr->name = safestrdup(refAttr->name);
    if (refAttr->value)
	attr->value = safestrdup(refAttr->value);
    return attr;
}

static void reset_attr_list_widgets(attr_list * l)
{
    int id = 0;
    for (id; id < MAX_FILTERED_ATTR_COUNT; id++) {
	gtk_label_set_text(l->fLabels[id], "");
    }
}



static void free_attr_list_widgets(attr_list * l)
{
    int id = 0;
    for (id; id < MAX_FILTERED_ATTR_COUNT; id++) {
	gtk_object_destroy((GtkObject *) l->fLabels[id]);
    }
}


void free_attr_list(attr_list * l)
{
    int id = 0;
    for (id; id < l->attr_count; id++) {
	free_attr(l->attributes[id]);
    }
    if (l->with_widgets)
	free_attr_list_widgets(l);
    free(l);
}
attr_list *attr_list_new(Agraph_t * g, int with_widgets)
{
    int id = 0;
    attr_list *l = malloc(sizeof(attr_list));
    l->attr_count = 0;
    l->capacity = DEFAULT_ATTR_LIST_CAPACITY;
    l->attributes = malloc(DEFAULT_ATTR_LIST_CAPACITY * sizeof(attr_t *));
    l->with_widgets = with_widgets;
    /*create filter widgets */

/*			gtk_widget_add_events(glade_xml_get_widget(xml, "frmObject"),
			//  GDK_BUTTON_MOTION_MASK      = 1 << 4,
			  GDK_BUTTON_MOTION_MASK |
			  GDK_POINTER_MOTION_MASK |
			  GDK_BUTTON_PRESS_MASK | GDK_KEY_PRESS |
			  GDK_BUTTON_RELEASE_MASK |
			  GDK_SCROLL | GDK_VISIBILITY_NOTIFY_MASK);

    g_signal_connect((gpointer)glade_xml_get_widget(xml, "frmObject"), "motion_notify_event",  G_CALLBACK(attr_label_motion), NULL);*/
    if (with_widgets) {
	for (id; id < MAX_FILTERED_ATTR_COUNT; id++) {
	    l->fLabels[id] = (GtkLabel *) gtk_label_new("");

	    gtk_widget_add_events((GtkWidget *) l->fLabels[id],
				  //  GDK_BUTTON_MOTION_MASK      = 1 << 4,
				  GDK_BUTTON_MOTION_MASK |
				  GDK_POINTER_MOTION_MASK |
				  GDK_BUTTON_PRESS_MASK | GDK_KEY_PRESS |
				  GDK_BUTTON_RELEASE_MASK |
				  GDK_SCROLL | GDK_VISIBILITY_NOTIFY_MASK);

	    gtk_widget_show((GtkWidget *) l->fLabels[id]);
	    gtk_fixed_put((GtkFixed *) glade_xml_get_widget(xml, "fixed6"),
			  (GtkWidget *) l->fLabels[id], 10, 110 + id * 13);
	}
    }
    return l;
}

void print_attr_list(attr_list * l)
{
    int id = 0;
    for (id; id < l->attr_count; id++) {
	printf("%d  %s (%d %d %d) \n", l->attributes[id]->index,
	       l->attributes[id]->name, l->attributes[id]->objType[0],
	       l->attributes[id]->objType[1],
	       l->attributes[id]->objType[2]);
	printf("defG:%s defN:%s defE:%s\n", l->attributes[id]->defValG,
	       l->attributes[id]->defValN, l->attributes[id]->defValE);
    }
}

int attr_compare(const void *a, const void *b)
{
    attr_t *a1 = *(attr_t **) a;
    attr_t *a2 = *(attr_t **) b;
    return STRCASECMP(a1->name, a2->name);
}

static void attr_list_sort(attr_list * l)
{
    qsort(l->attributes, l->attr_count, sizeof(attr_t *), attr_compare);

}

void attr_list_add(attr_list * l, attr_t * a)
{
    int id = 0;
    if ((!l) || (!a))
	return;
    l->attr_count++;
    if (l->attr_count == l->capacity) {
	l->capacity = l->capacity + EXPAND_CAPACITY_VALUE;
	l->attributes =
	    realloc(l->attributes, l->capacity * sizeof(attr_t *));
    }
    l->attributes[l->attr_count - 1] = a;
    if (l->attr_count > 1)
	attr_list_sort(l);
    /*update indices */
    for (id; id < l->attr_count; id++)
	l->attributes[id]->index = id;



}
static attr_data_type get_attr_data_type(char c)
{
//typedef enum {attr_alpha,attr_float,attr_int,attr_bool,attr_drowdown,attr_color} attr_data_type;
    switch (c) {
    case 'A':
	return attr_alpha;
	break;
    case 'F':
	return attr_float;
	break;
    case 'B':
	return attr_bool;
	break;
    case 'I':
	return attr_int;
	break;
    }
    return attr_alpha;

}
static void object_type_helper(char *a, int *t)
{
    if (strcmp(a, "GRAPH") == 0)
	t[0] = 1;
    if (strcmp(a, "CLUSTER") == 0)
	t[0] = 1;
    if (strcmp(a, "NODE") == 0)
	t[1] = 1;
    if (strcmp(a, "EDGE") == 0)
	t[2] = 1;
    if (strcmp(a, "ANY_ELEMENT") == 0) {
	t[0] = 1;
	t[1] = 1;
	t[2] = 1;
    }
}

static void set_attr_object_type(char *str, int *t)
{

    char *a;
    a = strtok(str, " ");
    object_type_helper(a, t);
    while (a = strtok(NULL, " or "))
	object_type_helper(a, t);

}

static attr_t *binarySearch(attr_list * l, char *searchKey)
{
    int middle, low, high, res;
    low = 0;
    high = l->attr_count - 1;

    while (low <= high) {
	middle = (low + high) / 2;
	res = STRCASECMP(searchKey, l->attributes[middle]->name);
	if (res == 0) {
	    return l->attributes[middle];
	} else if (res < 0) {
	    high = middle - 1;
	} else {
	    low = middle + 1;
	}
    }
    return NULL;
}






static attr_t *pBinarySearch(attr_list * l, char *searchKey)
{
    char buf[512];
    int middle, low, high, res;
    low = 0;
    high = l->attr_count - 1;

    while (low <= high) {
	middle = (low + high) / 2;
	strncpy(buf, l->attributes[middle]->name, strlen(searchKey));
	buf[strlen(searchKey)] = '\0';
	res = STRCASECMP(searchKey, buf);
	if (res == 0) {
	    return l->attributes[middle];
	}
//        else if               ( searchKey < b[ middle ] ) {
	else if (res < 0) {
	    high = middle - 1;
	} else {
	    low = middle + 1;
	}
    }
    return NULL;

}




void create_filtered_list(char *prefix, attr_list * sl, attr_list * tl)
{
    int res;
    char buf[512];
    attr_t *at;
    int objKind = get_object_type();

    if (strlen(prefix) == 0)
	return;
    /*locate first occurance */
    at = pBinarySearch(sl, prefix);
    if (!at)
	return;

    res = 0;
    /*go backward to get the first */
    while ((at->index > 0) && (res == 0)) {
	at = sl->attributes[at->index - 1];
	strncpy(buf, at->name, strlen(prefix));
	buf[strlen(prefix)] = '\0';;
	res = STRCASECMP(prefix, buf);
    }
    res = 0;
    while ((at->index < sl->attr_count) && (res == 0)) {
	at = sl->attributes[at->index + 1];
	strncpy(buf, at->name, strlen(prefix));
	buf[strlen(prefix)] = '\0';
	res = STRCASECMP(prefix, buf);
	if ((res == 0) && (at->objType[objKind] == 1))
	    attr_list_add(tl, new_attr_ref(at));
    }
}
void filter_attributes(char *prefix, topview * t)
//void filter_attributes(char* prefix, attr_list* l)
{

    int ind = 0;
    int tmp;

    attr_list *l = t->attributes;
    attr_list *fl = t->filtered_attr_list;
    int objKind = get_object_type();

    if (fl)
	free_attr_list(fl);
    fl = attr_list_new(NULL, 0);
    reset_attr_list_widgets(l);
    create_filtered_list(prefix, l, fl);
    for (ind; ind < fl->attr_count; ind++) {
	gtk_label_set_text(l->fLabels[ind], fl->attributes[ind]->name);
    }

    /*a new attribute can be entered */

    gtk_widget_show(glade_xml_get_widget(xml, "txtValue"));
    gtk_widget_show(glade_xml_get_widget(xml, "txtDefValue"));

    gtk_entry_set_text((GtkEntry *)
		       glade_xml_get_widget(xml, "txtDefValue"), "");
    gtk_entry_set_text((GtkEntry *) glade_xml_get_widget(xml, "txtValue"),
		       "");
    gtk_widget_set_sensitive(glade_xml_get_widget(xml, "txtDefValue"), 1);
    gtk_widget_show(glade_xml_get_widget(xml, "attrAddBtn"));
    gtk_widget_hide(glade_xml_get_widget(xml, "attrApplyBtn"));


    if (strlen(prefix) == 0) {
	gtk_widget_hide(glade_xml_get_widget(xml, "attrAddBtn"));
	gtk_widget_hide(glade_xml_get_widget(xml, "attrApplyBtn"));
	gtk_widget_hide(glade_xml_get_widget(xml, "attrAddBtn"));
	gtk_widget_hide(glade_xml_get_widget(xml, "txtValue"));
	gtk_widget_hide(glade_xml_get_widget(xml, "txtDefValue"));
    }


    for (ind = 0; ind < fl->attr_count; ind++) {
	if (STRCASECMP(prefix, fl->attributes[ind]->name) == 0) {	/*an existing attribute */

	    if (get_object_type() == AGRAPH)
		gtk_entry_set_text((GtkEntry *)
				   glade_xml_get_widget(xml,
							"txtDefValue"),
				   fl->attributes[0]->defValG);
	    if (get_object_type() == AGNODE)
		gtk_entry_set_text((GtkEntry *)
				   glade_xml_get_widget(xml,
							"txtDefValue"),
				   fl->attributes[0]->defValN);
	    if (get_object_type() == AGEDGE)
		gtk_entry_set_text((GtkEntry *)
				   glade_xml_get_widget(xml,
							"txtDefValue"),
				   fl->attributes[0]->defValE);
	    gtk_entry_set_text((GtkEntry *)
			       glade_xml_get_widget(xml, "txtValue"),
			       agget(view->g[view->activeGraph], prefix));
	    gtk_widget_set_sensitive(glade_xml_get_widget
				     (xml, "txtDefValue"), 0);
	    gtk_widget_hide(glade_xml_get_widget(xml, "attrAddBtn"));
	    gtk_widget_show(glade_xml_get_widget(xml, "attrApplyBtn"));

	    break;

	}
    }

    tmp = (((objKind == AGNODE) && (sel_node))
	   || ((objKind == AGEDGE) && (sel_edge)) || ((objKind == AGRAPH)
						      && (sel_graph)));
    gtk_widget_set_sensitive(glade_xml_get_widget(xml, "txtValue"), tmp);
}

/*asttribute text changed call back*/

_BB void on_txtAttr_changed(GtkWidget * widget, gpointer user_data)
{
//      printf ("attr text has been changed to %s \n",gtk_entry_get_text((GtkEntry*)widget));
    filter_attributes((char *) gtk_entry_get_text((GtkEntry *) widget),
		      view->Topview);
}

_BB void on_attrApplyBtn_clicked(GtkWidget * widget, gpointer user_data)
{
    int ind = 0;
    int cnt = 0;
    char *attr_name;
    char *value;
    int prog;
    topview *t;
    topview_node *n;
    topview_edge *e;
    Agraph_t *g;
    /*values to be applied to selected objects */
    attr_name =
	(char *) gtk_entry_get_text((GtkEntry *)
				    glade_xml_get_widget(xml, "txtAttr"));
    value =
	(char *) gtk_entry_get_text((GtkEntry *)
				    glade_xml_get_widget(xml, "txtValue"));
    prog =
	gtk_toggle_button_get_active((GtkToggleButton *)
				     glade_xml_get_widget(xml,
							  "attrProg"));
    g = view->g[view->activeGraph];
    t = view->Topview;
    /*nodes */
    for (ind; ind < view->Topview->Nodecount; ind++) {
	n = &t->Nodes[ind];
	if (n->data.Selected) {
	    cnt++;
	    if (cnt == 1)
		agattr(g, AGNODE, attr_name, "");
	    agset(n->Node, attr_name, value);
	}
    }
    /*edges */
    for (ind; ind < view->Topview->Edgecount; ind++) {
	e = &t->Edges[ind];
	if (e->data.Selected) {
	    cnt++;
	    if (cnt == 1)
		agattr(g, AGEDGE, attr_name, "");
	    agset(e->Edge, attr_name, value);
	}
    }
}
_BB on_attrRB0_clicked(GtkWidget * widget, gpointer user_data)
{
    filter_attributes((char *)
		      gtk_entry_get_text((GtkEntry *)
					 glade_xml_get_widget(xml,
							      "txtAttr")),
		      view->Topview);

}
_BB void on_attrProg_toggled(GtkWidget * widget, gpointer user_data)
{
    printf("%s \n", agget(view->Topview->Nodes[5].Node, "testattr"));

}

_BB void attr_label_motion(GtkWidget * widget, GdkEventMotion * event,
			   gpointer data)
{
    float x = (float) event->x;
    float y = (float) event->y;
//      printf ("%f %f \n",x,y);
}
_BB void on_attrAddBtn_clicked(GtkWidget * widget, gpointer user_data)
{
    int ind = 0;
    int cnt = 0;
    char *attr_name;
    char *value;
    char *defValue;
    static int objKind;


    int prog;
    topview *t;
    topview_node *n;
    topview_edge *e;
    Agraph_t *g;
    attr_t *attr;
    objKind = get_object_type();


    /*values to be applied to selected objects */
    attr_name =
	(char *) gtk_entry_get_text((GtkEntry *)
				    glade_xml_get_widget(xml, "txtAttr"));
    value =
	(char *) gtk_entry_get_text((GtkEntry *)
				    glade_xml_get_widget(xml, "txtValue"));
    defValue =
	(char *) gtk_entry_get_text((GtkEntry *)
				    glade_xml_get_widget(xml,
							 "txtDefValue"));
    prog =
	gtk_toggle_button_get_active((GtkToggleButton *)
				     glade_xml_get_widget(xml,
							  "attrProg"));
    g = view->g[view->activeGraph];
    t = view->Topview;
    attr =
	/*try to find first */
	attr = binarySearch(t->attributes, attr_name);
    if (!attr) {
	attr = new_attr();
	attr->index = 0;
	attr->name = safestrdup(attr_name);
	attr->type = attr_alpha;
	attr->value = safestrdup("");
	attr->widget = NULL;
	attr_list_add(t->attributes, attr);
    }
    attr->propagate = 0;

    if (objKind == AGRAPH) {
	agattr(g, AGRAPH, attr_name, defValue);
	attr->defValG = safestrdup(defValue);
	attr->objType[0] = 1;
    }

    /*nodes */
    if (objKind == AGNODE) {
	for (ind; ind < view->Topview->Nodecount; ind++) {
	    n = &t->Nodes[ind];
	    if (n->data.Selected) {
		cnt++;
		if (cnt == 1) {
		    agattr(g, AGNODE, attr_name, defValue);
		    break;
		}

	    }
	}
	attr->defValN = safestrdup(defValue);
	attr->objType[1] = 1;



    }
    if (objKind == AGEDGE) {

	cnt = 0;
	/*edges */
	for (ind; ind < view->Topview->Edgecount; ind++) {
	    e = &t->Edges[ind];
	    if (e->data.Selected) {
		cnt++;
		if (cnt == 1) {
		    agattr(g, AGEDGE, attr_name, defValue);
		    break;
		}
	    }
	}
	attr->defValE = safestrdup(defValue);
	attr->objType[2] = 1;

    }
    filter_attributes(attr_name, view->Topview);


}

attr_list *load_attr_list(Agraph_t * g)
{
    attr_t *attr;
    attr_list *l;
    FILE *file;
    Agsym_t *sym;		/*cgraph atttribute */
    char line[BUFSIZ];
    char *smyrna_attrs = smyrnaPath("attrs.txt");
    char *a;

    g = view->g[view->activeGraph];
    file = fopen(smyrna_attrs, "r");

    l = attr_list_new(NULL, 1);

    if (file != NULL) {
	int i = 0;
	while (fgets(line, BUFSIZ, file) != NULL) {
	    int idx = 0;
	    attr = new_attr();
	    a = strtok(line, ",");
	    attr->index = i;
	    attr->type = get_attr_data_type(a[0]);
	    while (a = strtok(NULL, ",")) {
		/*C,(0)color, (1)black, (2)EDGE Or NODE Or CLUSTER, (3)ALL_ENGINES */

		switch (idx) {
		case 0:
		    /**/ attr->name = safestrdup(a);
		    break;
		case 1:
		    /**/ attr->defValG = safestrdup(a);
		    attr->defValN = safestrdup(a);
		    attr->defValE = safestrdup(a);
		    break;
		case 2:
		    /**/ set_attr_object_type(a, attr->objType);
		    break;
		}
		idx++;
	    }
	    i++;
	    attr_list_add(l, attr);

	}
    }
    sym = NULL;
    while ((sym = agnxtattr(g, AGRAPH, sym))) {
	if (!binarySearch(l, sym->name)) {
	    attr = new_attr_with_ref(sym);
	    attr_list_add(l, attr);

	}
    }
    sym = NULL;
    while ((sym = agnxtattr(g, AGNODE, sym))) {
	if (!binarySearch(l, sym->name)) {
	    attr = new_attr_with_ref(sym);
	    attr_list_add(l, attr);

	}
    }
    sym = NULL;
    while ((sym = agnxtattr(g, AGEDGE, sym))) {
	if (!binarySearch(l, sym->name)) {
	    attr = new_attr_with_ref(sym);
	    attr_list_add(l, attr);
	}
    }

    print_attr_list(l);
    return l;
}

 /**/ static void set_header_text()
{
    int nodeCnt;
    int edgeCnt;
    static char buf[512];
    int ind = 0;

    topview *t;
    topview_node *n;
    topview_edge *e;



    nodeCnt = 0;
    edgeCnt = 0;
    t = view->Topview;


    for (ind; ind < view->Topview->Nodecount; ind++) {
	n = &t->Nodes[ind];
	if (n->data.Selected) {
	    nodeCnt++;
	}
    }
    for (ind; ind < view->Topview->Edgecount; ind++) {
	e = &t->Edges[ind];
	if (e->data.Selected) {
	    edgeCnt++;
	}
    }
    sel_node = nodeCnt;
    sel_edge = edgeCnt;
    sel_graph = 1;

    sprintf(buf, "%d Nodes and %d edges selected", nodeCnt, edgeCnt);
    gtk_label_set_text((GtkLabel *) glade_xml_get_widget(xml, "label124"),
		       buf);
    gtk_entry_set_text((GtkEntry *) glade_xml_get_widget(xml, "txtAttr"),
		       "");


}

void showAttrsWidget(topview * t)
{
    gtk_widget_hide(glade_xml_get_widget(xml, "dlgSettings"));
    gtk_widget_show(glade_xml_get_widget(xml, "dlgSettings"));
    gtk_notebook_set_current_page((GtkNotebook *)
				  glade_xml_get_widget(xml, "notebook3"),
				  ATTR_NOTEBOOK_IDX);
    set_header_text();
    filter_attributes("", view->Topview);


}


#if 0

static char guibuffer[BUFSIZ];	//general purpose buffer

#ifdef WIN32
extern int STRCASECMP(const char *s1, const char *s2);
extern int strncasecmp(const char *s1, const char *s2, unsigned int n);
#endif

GdkWindow *window1;
GtkWidget *statusbar1;

GladeXML *xml;			//global libglade vars
GtkWidget *gladewidget;

//1 subgraph 2 node 3 edge
int frmObjectTypeIndex;
Agraph_t *frmObjectg;


GtkComboBox *cbSelectGraph;	//combo at top left

GtkWidget *AttrWidgets[MAXIMUM_WIDGET_COUNT];
GtkWidget *AttrLabels[MAXIMUM_WIDGET_COUNT];
int attr_widgets_modified[MAXIMUM_WIDGET_COUNT];
int widgetcounter;		//number of attributes counted dynamically, might be removed in the future 
attribute attr[MAXIMUM_WIDGET_COUNT];



//loads object properties form and does some fixes
//call this function only ones
void create_object_properties()
{
#ifdef UNUSED
    char *data0 = "TEST0";
    char *data1 = "TEST1";
    char *data2 = "TEST2";
    char *data3 = "TEST3";
    char *data4 = "TEST4";
#endif
    GladeXML *xml;
    GtkWidget *widget;
    xml = glade_xml_new(smyrnaGlade, NULL, NULL);

    widget = glade_xml_get_widget(xml, "listPoints");
    gtk_clist_set_column_title((GtkCList *) widget, 0, "Def");
    gtk_clist_set_column_title((GtkCList *) widget, 1, "x");
    gtk_clist_set_column_title((GtkCList *) widget, 2, "y");
    gtk_clist_set_column_title((GtkCList *) widget, 3, "z");
    gtk_clist_column_titles_show((GtkCList *) widget);
    gtk_widget_show(widget);
    widget = glade_xml_get_widget(xml, "win");
    gtk_widget_show(widget);


}

//call this after create_object_properties()
void object_properties_node_init()
{

}
void object_properties_edge_init()	//customize window for Edges
{
}
void object_properties_cluster_init()	//customize window for Cluster
{

}
void object_properties_graph_init()	//customize window for Graph , this shows the graph default values
{

}


void graph_properties_init(int newgraph)	//initialize little open graph dialog
{
    //newgraph 0 : open graph mode
    //newgraph 1 : new graph mode


    gint result = 0;
    xml = glade_xml_new(smyrnaGlade, NULL, NULL);
    gladewidget = glade_xml_get_widget(xml, "entryGraphFileName");

    //signals
    //OK
    gladewidget = glade_xml_get_widget(xml, "btnOK");


    g_signal_connect((gpointer) gladewidget, "clicked",
		     G_CALLBACK(dlgOpenGraph_OK_Clicked), &newgraph);



    if (newgraph) {
	gladewidget = glade_xml_get_widget(xml, "entryGraphName");
	gtk_entry_set_text((GtkEntry *) gladewidget, "Untitled");
	gladewidget = glade_xml_get_widget(xml, "entryGraphFileName");
	gtk_entry_set_text((GtkEntry *) gladewidget, "Untitled.dot");
    } else {
	gladewidget = glade_xml_get_widget(xml, "entryGraphName");
	gtk_entry_set_text((GtkEntry *) gladewidget, "");
	gladewidget = glade_xml_get_widget(xml, "entryGraphFileName");
	gtk_entry_set_text((GtkEntry *) gladewidget, "");
    }
    gladewidget = glade_xml_get_widget(xml, "dlgOpenGraph");
    result = gtk_dialog_run(GTK_DIALOG(gladewidget));
}

GtkComboBox *get_SelectGraph()
{
    GtkComboBox *cb;
    if (!cbSelectGraph) {

	cb = (GtkComboBox *) gtk_combo_box_new_text();
	gtk_widget_show((GtkWidget *) cb);
	gladewidget = glade_xml_get_widget(xml, "layout6");
	gtk_box_pack_start(GTK_BOX(gladewidget), (GtkWidget *) cb, FALSE,
			   FALSE, 0);


	gtk_layout_put((GtkLayout *) gladewidget, (GtkWidget *) cb, 780,
		       3);
	//signal
	g_signal_connect((gpointer) cb, "changed",
			 G_CALLBACK(graph_select_change), NULL);


	return cb;
    } else
	return cbSelectGraph;
}


void Color_Widget_bg(char *colorstring, GtkWidget * widget)
{
    GdkColor color;
    gdk_color_parse(colorstring, &color);
    gtk_widget_modify_bg(widget, GTK_STATE_NORMAL, &color);
    gtk_widget_modify_base(widget, GTK_STATE_NORMAL, &color);
}


#if 0
void Color_Widget_fg(int r, int g, int b, GtkWidget * widget)
{
    GdkColor color;
    gdk_color_parse("red", &color);
    gtk_widget_modify_fg(widget, GTK_STATE_NORMAL, &color);

}
#endif

void load_graph_properties(Agraph_t * graph)
{
    //dlgOpenGraph , GtkDialog
    gtk_entry_set_text((GtkEntry *)
		       glade_xml_get_widget(xml, "entryGraphName"),
		       GD_GraphName(graph));
    gtk_entry_set_text((GtkEntry *)
		       glade_xml_get_widget(xml, "entryGraphFileName"),
		       GD_GraphFileName(graph));
    gtk_combo_box_set_active((GtkComboBox *)
			     glade_xml_get_widget(xml, "cbLayout"),
			     GD_Engine(graph));
    gtk_toggle_button_set_active((GtkToggleButton *)
				 glade_xml_get_widget(xml, "chkVisible"),
				 GD_AlwaysShow(graph));
    gtk_toggle_button_set_active((GtkToggleButton *)
				 glade_xml_get_widget(xml, "chkLocked"),
				 GD_Locked(graph));
    gtk_toggle_button_set_active((GtkToggleButton *)
				 glade_xml_get_widget(xml, "chkTopView"),
				 GD_TopView(graph));
}

int update_graph_properties(Agraph_t * graph)	//updates graph from gui
{
    FILE *file;
    int respond = 0;
    int id = 0;
    //check the graph name  should not be duplicated graph names
    for (id = 0; id < view->graphCount; id++) {
	if (graph != view->g[id]) {
	    if (STRCASECMP
		(gtk_entry_get_text
		 ((GtkEntry *)
		  glade_xml_get_widget(xml, "entryGraphName")),
		 GD_GraphName(view->g[id])) == 0) {
		Dlg =
		    (GtkMessageDialog *) gtk_message_dialog_new(NULL,
								GTK_DIALOG_MODAL,
								GTK_MESSAGE_WARNING,
								GTK_BUTTONS_OK,
								"There is another graph with this name!");
		respond = gtk_dialog_run((GtkDialog *) Dlg);
		gtk_object_destroy((GtkObject *) Dlg);
		return 0;

	    }

	}
    }

    //check if file is changed
    if (STRCASECMP
	(gtk_entry_get_text
	 ((GtkEntry *) glade_xml_get_widget(xml, "entryGraphFileName")),
	 GD_GraphFileName(graph)) != 0) {


	if ((file = fopen(gtk_entry_get_text((GtkEntry *)
					     glade_xml_get_widget(xml,
								  "entryGraphFileName")),
			  "r"))) {
	    fclose(file);
	    Dlg = (GtkMessageDialog *) gtk_message_dialog_new(NULL,
							      GTK_DIALOG_MODAL,
							      GTK_MESSAGE_QUESTION,
							      GTK_BUTTONS_YES_NO,
							      "File name you have entered already exists\n,this will cause overwriting on existing file.\nAre you sure?");
	    respond = gtk_dialog_run((GtkDialog *) Dlg);
	    gtk_object_destroy((GtkObject *) Dlg);

	    if (respond == GTK_RESPONSE_NO)
		return 0;
	}
	//now check if filename is legal, try to open it to write
	if ((file = fopen(gtk_entry_get_text((GtkEntry *)
					     glade_xml_get_widget(xml,
								  "entryGraphFileName")),
			  "w")))
	    fclose(file);
	else {
	    Dlg = (GtkMessageDialog *) gtk_message_dialog_new(NULL,
							      GTK_DIALOG_MODAL,
							      GTK_MESSAGE_WARNING,
							      GTK_BUTTONS_OK,
							      "File name is invalid or I/O error!");

	    respond = gtk_dialog_run((GtkDialog *) Dlg);
	    gtk_object_destroy((GtkObject *) Dlg);
	    GTK_DIALOG(Dlg);

	    return 0;
	}

    }

    //if it comes so far graph deserves new values

    GD_GraphName(graph) = (char *) gtk_entry_get_text((GtkEntry *)
						      glade_xml_get_widget
						      (xml,
						       "entryGraphName"));
    GD_GraphFileName(graph) = (char *) gtk_entry_get_text((GtkEntry *)
							  glade_xml_get_widget
							  (xml,
							   "entryGraphFileName"));

    GD_AlwaysShow(graph) = gtk_toggle_button_get_active((GtkToggleButton *)
							glade_xml_get_widget
							(xml,
							 "chkVisible"));
    GD_Locked(graph) = gtk_toggle_button_get_active((GtkToggleButton *)
						    glade_xml_get_widget
						    (xml, "chkLocked"));
    GD_TopView(graph) = gtk_toggle_button_get_active((GtkToggleButton *)
						     glade_xml_get_widget
						     (xml, "chkTopView"));


    //check if the engine has been changed, if so do new layout
    if (GD_Engine(graph) != gtk_combo_box_get_active((GtkComboBox *)
						     glade_xml_get_widget
						     (xml, "cbLayout"))) {
	Dlg =
	    (GtkMessageDialog *) gtk_message_dialog_new(NULL,
							GTK_DIALOG_MODAL,
							GTK_MESSAGE_QUESTION,
							GTK_BUTTONS_YES_NO,
							"You have changed the layout of the graph,this will change the graph layout\n all your position changes will be lost\n Are you sure?");
	respond = gtk_dialog_run((GtkDialog *) Dlg);
	if (respond == GTK_RESPONSE_YES)
	    do_graph_layout(graph, gtk_combo_box_get_active((GtkComboBox *)
							    glade_xml_get_widget
							    (xml,
							     "cbLayout")),
			    0);
	gtk_object_destroy((GtkObject *) Dlg);
    }
    return 1;
}



int load_object_properties(gve_element typeIndex, Agraph_t * g)	//load  from object to gui;
{
    //load attr from first selected object
    GtkLayout *layout;
    GdkColor color;
    char buf[BUFSIZ];
    /* int ind=0; */
    int Y = 45;
    int X = 90;
    int Yinc = 25;
    /* int Xinc=30; */
    /* int OriginalX= 90; */
    int OriginalY = 45;
    int widget_per_page = 21;
    void *obj;
    char line[BUFSIZ];
    float a, b;
    layout = (GtkLayout *) glade_xml_get_widget(xml, "layout4");
    frmObjectTypeIndex = typeIndex;
    frmObjectg = g;
    widgetcounter = 0;
    //values should be set from first selected object
    //according to object type (typeIndex) set the reference object
    switch (typeIndex)		//typeindex 0 means new object
    {
    case GVE_CLUSTER:		//graph  sub graph (cluster)
	obj = GD_selectedGraphs(g)[0];
	break;
    case GVE_NODE:		//Node
	obj = GD_selectedNodes(g)[0];
	break;
    case GVE_EDGE:		//Edge
	obj = GD_selectedEdges(g)[0];
	break;
    default:
	break;
    }
    for (widgetcounter = 0; widgetcounter < MAXIMUM_WIDGET_COUNT;
	 widgetcounter++) {
	//create the labels and widget here
	attr[widgetcounter].ComboValuesCount = 0;
	attr[widgetcounter].ComboValues = '\0';

	if (!AttrWidgets[widgetcounter]) {
	    AttrLabels[widgetcounter] =
		gtk_label_new(attr[widgetcounter].Name);
	    switch (attr[widgetcounter].Type) {
	    case 'F':		//float
		AttrWidgets[widgetcounter] =
		    gtk_spin_button_new_with_range(0, 100, 0.001);
		g_signal_connect((gpointer) AttrWidgets[widgetcounter],
				 "value-changed",
				 G_CALLBACK(attr_widgets_modifiedSlot),
				 (gpointer) widgetcounter);

		break;
	    case 'C':		//color box
		AttrWidgets[widgetcounter] = gtk_color_button_new();
		gtk_widget_set_size_request(AttrWidgets[widgetcounter], 50,
					    23);
		g_signal_connect((gpointer) AttrWidgets[widgetcounter],
				 "color-set",
				 G_CALLBACK(attr_widgets_modifiedSlot),
				 (gpointer) widgetcounter);

		break;
	    default:		//alphanumreric         GTK Entry
		AttrWidgets[widgetcounter] = gtk_entry_new();
		gtk_widget_set_size_request(AttrWidgets[widgetcounter],
					    130, 23);
		g_signal_connect((gpointer) AttrWidgets[widgetcounter],
				 "changed",
				 G_CALLBACK(attr_widgets_modifiedSlot),
				 (gpointer) widgetcounter);
		break;
	    }
	    attr[widgetcounter].attrWidget = AttrWidgets[widgetcounter];
	}
	//locate widget on the GtkLayout* layout
	if (attr[widgetcounter].ApplyTo[typeIndex] == 1) {
	    gtk_layout_put(layout, AttrWidgets[widgetcounter], X, Y);
	    gtk_layout_put(layout, AttrLabels[widgetcounter], X - 80, Y);
	    gtk_widget_show(AttrWidgets[widgetcounter]);
	    gtk_widget_show(AttrLabels[widgetcounter]);
	    Y = Y + Yinc;
	    switch (attr[widgetcounter].Type) {
	    case 'F':
		if (agget(obj, attr[widgetcounter].Name))
		    gtk_spin_button_set_value((GtkSpinButton *)
					      AttrWidgets[widgetcounter],
					      atof(agget
						   (obj,
						    attr[widgetcounter].
						    Name)));
		else
		    gtk_spin_button_set_value((GtkSpinButton *)
					      AttrWidgets[widgetcounter],
					      atof(attr[widgetcounter].
						   Default));
		break;
	    case 'C':
		if (agget(obj, attr[widgetcounter].Name))
		    setGdkColor(&color,
				agget(obj, attr[widgetcounter].Name));
		else
		    setGdkColor(&color, attr[widgetcounter].Default);

		gtk_color_button_set_color((GtkColorButton *)
					   AttrWidgets[widgetcounter],
					   &color);
		break;
	    default:
		if (agget(obj, attr[widgetcounter].Name))
		    gtk_entry_set_text((GtkEntry *)
				       AttrWidgets[widgetcounter],
				       agget(obj,
					     attr[widgetcounter].Name));
		else
		    gtk_entry_set_text((GtkEntry *)
				       AttrWidgets[widgetcounter],
				       attr[widgetcounter].Default);
	    }
	    gtk_widget_show(AttrWidgets[widgetcounter]);
	    gtk_widget_show(AttrLabels[widgetcounter]);
	} else {
	    gtk_widget_hide(AttrWidgets[widgetcounter]);
	    gtk_widget_hide(AttrLabels[widgetcounter]);
	}
	if (Y > widget_per_page * Yinc) {
	    X = 320;
	    Y = OriginalY;
	}
	attr_widgets_modified[widgetcounter] = 0;	//set to unmodified
    }


    //first part, common attributes
    sprintf(buf, "%i", OD_ID(obj));
    gtk_entry_set_text((GtkEntry *)
		       glade_xml_get_widget(xml, "objEntryName"),
		       OD_ObjName(obj));
    gtk_entry_set_text((GtkEntry *)
		       glade_xml_get_widget(xml, "objEntryLabel"),
		       agnameof(obj));
    gtk_toggle_button_set_active((GtkToggleButton *)
				 glade_xml_get_widget(xml,
						      "frmObjectchkVisible"),
				 OD_Visible(obj));
    gtk_toggle_button_set_active((GtkToggleButton *)
				 glade_xml_get_widget(xml,
						      "frmObjectchkLocked"),
				 OD_Locked(obj));
    gtk_toggle_button_set_active((GtkToggleButton *)
				 glade_xml_get_widget(xml,
						      "frmObjectchkHighlighted"),
				 OD_Highlighted(obj));
    //get the position info // show only one item is selected
    if (((GD_selectedNodesCount(g) == 1) && (typeIndex == GVE_NODE))
	|| ((GD_selectedEdgesCount(g) == 1) && (typeIndex == GVE_EDGE))
	|| ((GD_selectedGraphsCount(g) == 1) && (typeIndex == GVE_EDGE))) {
	sprintf(line, "%s", agget(obj, "pos"));
	a = (float) atof(strtok(line, ","));
	b = (float) atof(strtok(NULL, ","));
	gtk_spin_button_set_value((GtkSpinButton *)
				  glade_xml_get_widget(xml,
						       "frmObjectPosX"),
				  a);
	gtk_spin_button_set_value((GtkSpinButton *)
				  glade_xml_get_widget(xml,
						       "frmObjectPosY"),
				  b);
	gtk_spin_button_set_value((GtkSpinButton *)
				  glade_xml_get_widget(xml,
						       "frmObjectPosZ"),
				  0);
	gtk_widget_show(glade_xml_get_widget(xml, "frmObjectPosX"));
	gtk_widget_show(glade_xml_get_widget(xml, "frmObjectPosY"));
	gtk_widget_show(glade_xml_get_widget(xml, "frmObjectPosZ"));
	gtk_widget_show(glade_xml_get_widget(xml, "frmObjectlabel3"));
	gtk_label_set_text((GtkLabel *)
			   glade_xml_get_widget(xml, "frmObjectPosLabelX"),
			   "X:");
    } else {
	gtk_widget_hide(glade_xml_get_widget(xml, "frmObjectPosX"));
	gtk_widget_hide(glade_xml_get_widget(xml, "frmObjectPosY"));
	gtk_widget_hide(glade_xml_get_widget(xml, "frmObjectPosZ"));
	gtk_widget_hide(glade_xml_get_widget(xml, "frmObjectlabel3"));
	gtk_widget_hide(glade_xml_get_widget(xml, "frmObjectPosLabelY"));
	gtk_widget_hide(glade_xml_get_widget(xml, "frmObjectPosLabelZ"));
	switch (typeIndex)	//typeindex 0 means new object
	{
	case GVE_CLUSTER:	//graph  sub graph (cluster)
	    gtk_label_set_text((GtkLabel *)
			       glade_xml_get_widget(xml,
						    "frmObjectPosLabelX"),
			       "Changes that you make will be applied to all selected clusters");
	    break;
	case GVE_NODE:		//Node
	    gtk_label_set_text((GtkLabel *)
			       glade_xml_get_widget(xml,
						    "frmObjectPosLabelX"),
			       "Changes that you make will be applied to all selected nodes!");
	    break;
	case GVE_EDGE:		//Edge
	    gtk_label_set_text((GtkLabel *)
			       glade_xml_get_widget(xml,
						    "frmObjectPosLabelX"),
			       "Changes that you make will be applied to all selected edges!");
	    break;
	default:
	    break;
	}
    }
    return 1;
}
void update_object_properties(int typeIndex, Agraph_t * g)	//updates objects from gui(node ,edge, cluster)
{
    int ind = 0;
    for (ind = 0; ind < widgetcounter; ind++) {
	//if widget has been changed
	if (attr_widgets_modified[ind] == 1) {
	    switch (typeIndex)	//typeindex 0 means new object
	    {
	    case GVE_CLUSTER:	//graph  sub graph (cluster)
		change_selected_graph_attributes(g, attr[ind].Name,
						 get_attribute_string_value_from_widget
						 (&attr[ind]));
		break;
	    case GVE_NODE:	//Node
		change_selected_node_attributes(g, attr[ind].Name,
						get_attribute_string_value_from_widget
						(&attr[ind]));
		break;
	    case GVE_EDGE:	//Edge
		change_selected_edge_attributes(g, attr[ind].Name,
						get_attribute_string_value_from_widget
						(&attr[ind]));
		break;
	    }
	}

    }
}
char *get_attribute_string_value_from_widget(attribute * att)
{
    GdkColor color;
    switch (att->Type) {
    case 'F':
	sprintf(guibuffer, "%f",
		gtk_spin_button_get_value((GtkSpinButton *) att->
					  attrWidget));
	return guibuffer;
	break;
    case 'C':
	gtk_color_button_get_color((GtkColorButton *) att->attrWidget,
				   &color);
	sprintf(guibuffer, "#%x%x%x", color.red / 255, color.green / 255,
		color.blue / 255);
	return guibuffer;
	break;
    default:
	strcpy(guibuffer,
	       gtk_entry_get_text((GtkEntry *) att->attrWidget));
	return guibuffer;
    }
}
void change_selected_graph_attributes(Agraph_t * g, char *attrname,
				      char *attrvalue)
{
    int ind = 0;
    agattr(g, AGRAPH, attrname, "");

    for (ind = 0; ind < GD_selectedGraphsCount(g); ind++) {
	agset(GD_selectedGraphs(g)[ind], attrname, attrvalue);
    }


}
void change_selected_node_attributes(Agraph_t * g, char *attrname,
				     char *attrvalue)
{
    int ind = 0;
    agattr(g, AGNODE, attrname, "");

    for (ind = 0; ind < GD_selectedNodesCount(g); ind++) {
	agset(GD_selectedNodes(g)[ind], attrname, attrvalue);
    }
}
void change_selected_edge_attributes(Agraph_t * g, char *attrname,
				     char *attrvalue)
{
    int ind = 0;
    agattr(g, AGEDGE, attrname, "");

    for (ind = 0; ind < GD_selectedEdgesCount(g); ind++) {
	agset(GD_selectedEdges(g)[ind], attrname, attrvalue);

    }
}





void load_attributes()
{
    FILE *file;
    char line[BUFSIZ];
    char *ss;
    char *pch;
    int ind = 0;
    int attrcount = 0;
    static char *smyrna_attrs;

    if (!smyrna_attrs) {
#if WIN32
	smyrna_attrs = SMYRNA_ATTRS;
#else
	smyrna_attrs = smyrnaPath("attrs.txt");
#endif
    }
    //loads attributes from a text file
    file = fopen(smyrna_attrs, "r");
    if (file != NULL) {
	while (fgets(line, sizeof line, file) != NULL) {
	    pch = strtok(line, ",");
	    ind = 0;
	    while (pch != NULL) {
		ss = safestrdup(pch);
//                              ABRemove(&ss,'\"');
//                              ABRemove(&ss,' ');
		pch = strtok(NULL, ",");
		switch (ind) {
		case 0:
		    attr[attrcount].Type = ss[0];
		    break;
		case 1:
		    attr[attrcount].Name = safestrdup(ss);
		    break;
		case 2:
		    attr[attrcount].Default = safestrdup(ss);
		    break;
		case 3:
		    if (strstr(ss, "ANY_ELEMENT")) {
			attr[attrcount].ApplyTo[GVE_GRAPH] = 1;
			attr[attrcount].ApplyTo[GVE_CLUSTER] = 1;
			attr[attrcount].ApplyTo[GVE_NODE] = 1;
			attr[attrcount].ApplyTo[GVE_EDGE] = 1;
		    } else {
			attr[attrcount].ApplyTo[GVE_GRAPH] =
			    strstr(ss, "GRAPH") ? 1 : 0;
			attr[attrcount].ApplyTo[GVE_CLUSTER] =
			    strstr(ss, "CLUSTER") ? 1 : 0;
			attr[attrcount].ApplyTo[GVE_NODE] =
			    strstr(ss, "NODE") ? 1 : 0;
			attr[attrcount].ApplyTo[GVE_EDGE] =
			    strstr(ss, "EDGE") ? 1 : 0;
		    }
		    break;
		case 4:
		    if (strstr(ss, "ALL_ENGINES")) {
			attr[attrcount].Engine[GVK_DOT] = 1;
			attr[attrcount].Engine[GVK_NEATO] = 1;
			attr[attrcount].Engine[GVK_TWOPI] = 1;
			attr[attrcount].Engine[GVK_CIRCO] = 1;
			attr[attrcount].Engine[GVK_FDP] = 1;
		    } else {
			attr[attrcount].Engine[GVK_DOT] =
			    strstr(ss, "DOT") ? 1 : 0;
			attr[attrcount].Engine[GVK_NEATO] =
			    strstr(ss, "NEATO") ? 1 : 0;
			attr[attrcount].Engine[GVK_TWOPI] =
			    strstr(ss, "TWOPI") ? 1 : 0;
			attr[attrcount].Engine[GVK_CIRCO] =
			    strstr(ss, "CIRCO") ? 1 : 0;
			attr[attrcount].Engine[GVK_FDP] =
			    strstr(ss, "FDP") ? 1 : 0;
		    }
		    break;
		default:
		    attr[attrcount].ComboValues =
			RALLOC(attr[attrcount].ComboValuesCount,
			       attr[attrcount].ComboValues, char *);
		    attr[attrcount].ComboValues[attr[attrcount].
						ComboValuesCount] =
			safestrdup(ss);
		    attr[attrcount].ComboValuesCount++;
		    break;
		}
		ind++;
	    }
	    attrcount++;
	}
    }
}

#endif
