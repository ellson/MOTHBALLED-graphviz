#include "viewportcamera.h"
static viewport_camera* new_viewport_camera(ViewInfo * view)
{
	viewport_camera* c=malloc(sizeof(viewport_camera));
	return c;
}

viewport_camera* add_camera_to_viewport(ViewInfo * view)
{
	view->cameras=(viewport_camera**)realloc(view->cameras,sizeof(viewport_camera *)*view->camera_count);

	view->cameras[view->camera_count]=new_viewport_camera(view);
	view->camera_count++;
	return view->cameras[view->camera_count-1];
}
int delete_camera_from_viewport(ViewInfo * view,viewport_camera* c)
{
    int ind = 0;
    int found = 0;
    for (ind; ind < view->camera_count - 1; ind++)
	{
		if ((view->cameras[ind] == c) && found == 0)
		    found = 1;
		if (found == 1)
			view->cameras[ind] =view->cameras[ind + 1];
    }
    if (found)
	{
		free(c);
		view->camera_count--;
		view->cameras =
			realloc(view->cameras, sizeof(viewport_camera*) * view->camera_count);
		return 1;
    }
    return 0;
}
int activate_viewport_camera (ViewInfo * view,int cam_index)
{
	if (cam_index < view)
	{
		view->active_camera=cam_index;
		return refresh_viewport_camera(view);
	}
	else
		return 0;
}
int refresh_viewport_camera (ViewInfo * view)
{
	if(view->active_camera >=0)
	{
		
/*		view->panx=view->cameras[view->active_camera]->panx;
		view->pany=view->cameras[view->active_camera]->pany;
		view->panz=view->cameras[view->active_camera]->panz;
		view->zoom=view->cameras[view->active_camera]->zoom;*/
		return 1;
	}
	else
		return 0;



}
