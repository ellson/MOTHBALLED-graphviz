#ifndef VIEWPORTCAMERA_H
#define VIEWPORTCAMERA_H
#include "smyrnadefs.h"
viewport_camera* add_camera_to_viewport(ViewInfo * view);
int delete_camera_from_viewport(ViewInfo * view,viewport_camera* c);
int activate_viewport_camera (ViewInfo * view,int cam_index);
int refresh_viewport_camera (ViewInfo * view);

#endif