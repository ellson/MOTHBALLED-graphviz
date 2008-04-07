#ifndef VIEWPORTCAMERA_H
#define VIEWPORTCAMERA_H
#include "smyrnadefs.h"
#include "glcompset.h"
viewport_camera* add_camera_to_viewport(ViewInfo * view);
int delete_camera_from_viewport(ViewInfo * view,viewport_camera* c);
int activate_viewport_camera (ViewInfo * view,int cam_index);
int refresh_viewport_camera (ViewInfo * view);
void attach_camera_widget(ViewInfo * view);
int show_camera_settings(viewport_camera* c);
int save_camera_settings(viewport_camera* c);
_BB void dlgcameraokbutton_clicked_cb(GtkWidget * widget, gpointer user_data);
_BB void dlgcameracancelbutton_clicked_cb(GtkWidget * widget, gpointer user_data);

#endif