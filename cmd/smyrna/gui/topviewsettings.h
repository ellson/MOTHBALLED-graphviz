#ifndef TOPVIEWSETTINGS_H
#define TOPVIEWSETTINGS_H


#include "smyrnadefs.h"


_BB void on_settingsOKBtn_clicked (GtkWidget *widget,gpointer user_data);
_BB void on_settingsCancelBtn_clicked (GtkWidget *widget,gpointer user_data);
int load_settings_from_graph(Agraph_t *g);
int update_graph_from_settings(Agraph_t *g);
int show_settings_form();




#endif
