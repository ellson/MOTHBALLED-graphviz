#include "glmotion.h"
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtkgl.h>
#include <gdk/gdkcursor.h>
#include "draw.h"
#include "topview.h"
#include "glutils.h"

gboolean redraw = FALSE;

int glmotion_main(ViewInfo * v,GdkEventMotion * event,GtkWidget * widget)
{
	
	redraw = FALSE;
	/*panning */
    if ((event->state & GDK_BUTTON1_MASK)&& (v->mouse.mouse_mode == MM_PAN))
		glmotion_pan(v);

	/*rotating, only in 3d v */
    if ((v->active_camera >=0)&&(v->mouse.mouse_mode == MM_ROTATE))
		glmotion_rotate(v);

	/*zooming */
    if ((event->state & GDK_BUTTON1_MASK)&& (v->mouse.mouse_mode == MM_ZOOM))
		glmotion_zoom(v);
	return 1;

	/*selection rect */
    if ((event->state & GDK_BUTTON1_MASK)
	&& ((v->mouse.mouse_mode == MM_RECTANGULAR_SELECT)
	    || (v->mouse.mouse_mode == 5)))
	{
			GetFixedOGLPos((int) event->x, (int)event->y, v->GLDepth, &(v->GLx2),
		       &(v->GLy2), &(v->GLz2));
		redraw = TRUE;
    }
    if ((event->state & GDK_BUTTON1_MASK)
	&& (v->mouse.mouse_mode == MM_MOVE)) {
	GetFixedOGLPos((int)event->x, (int) event->y, v->GLDepth, &(v->GLx2),
		       &(v->GLy2), &(v->GLz2));
	redraw = TRUE;
    }
    if ((event->state & GDK_BUTTON1_MASK)
	&& ((v->mouse.mouse_mode == MM_MAGNIFIER)
	    || (v->mouse.mouse_mode == MM_FISHEYE_MAGNIFIER)))
	{
		v->mouse.mouse_X = (int)event->x;
		v->mouse.mouse_Y = (int) event->y;
		redraw = TRUE;
    }

    if (redraw)
	gdk_window_invalidate_rect(widget->window, &widget->allocation,
				   FALSE);

	return 1;
}
int glmotion_zoom(ViewInfo * v)
{



/*		if (view->active_camera == -1) {
			old_zoom = view->zoom;
			real_zoom = view->zoom + dx / 10 * (view->zoom * -1 / 20);
		}
		else
		{
			old_zoom = view->cameras[view->active_camera]->r;
			real_zoom =
			(view->cameras[view->active_camera]->r +
			dx / 10 * (view->cameras[view->active_camera]->r / 20)) *
			-1;
		}

		if (real_zoom > MAX_ZOOM)
		    real_zoom = (float) MAX_ZOOM;
		if (real_zoom < MIN_ZOOM)
			real_zoom = (float) MIN_ZOOM;

		if (view->active_camera == -1)
		    view->zoom = real_zoom;
		else {
			view->cameras[view->active_camera]->r = real_zoom * -1;

			}
		view->panx = old_zoom * view->panx / real_zoom;
		view->pany = old_zoom * view->pany / real_zoom;

		x = ((float) 100.0 - (float) 1.0) * (view->zoom -
					     (float) MIN_ZOOM) /
			((float) MAX_ZOOM - (float) MIN_ZOOM) + (float) 1.0;
		sprintf(buf, "%i", (int) x);
		glCompLabelSetText((glCompLabel *) view->Topview->customptr, buf);
		redraw = TRUE;*/







	char buf[256];

	float real_zoom,old_zoom,xx;
	if (view->active_camera == -1) {
			old_zoom = view->zoom;
			real_zoom = view->zoom + view->mouse.dx / 10 * (view->zoom * -1 / 20);
	}
	else
	{
		old_zoom = view->cameras[view->active_camera]->r;
		real_zoom =
			(view->cameras[view->active_camera]->r +
			view->mouse.dx / 10 * (view->cameras[view->active_camera]->r / 20)) *
			-1;
	}

	if (real_zoom > MAX_ZOOM)
		real_zoom = (float) MAX_ZOOM;
	if (real_zoom < MIN_ZOOM)
		real_zoom = (float) MIN_ZOOM;
	if(v->active_camera==-1)
		v->zoom = real_zoom;
	else
		v->cameras[v->active_camera]->r=real_zoom*-1;
	/*adjust pan values*/
	view->panx = old_zoom * view->panx / real_zoom;
	view->pany = old_zoom * view->pany / real_zoom;

	/*set label to new zoom value */
	xx = ((float) 100.0 - (float) 1.0) * (v->zoom -
		     (float) MIN_ZOOM) / ((float) MAX_ZOOM - (float) MIN_ZOOM) + (float) 1.0;
	sprintf(buf, "%i", (int) xx);
	glCompLabelSetText((glCompLabel *) v->Topview->customptr, buf);
	return 1;
}
int glmotion_pan(ViewInfo * v)
{
		float gldx,gldy;
		if(v->active_camera ==-1)
		{
			gldx=GetOGLDistance((int)v->mouse.dx)/v->zoom*-1;
			gldy=GetOGLDistance((int)v->mouse.dy)/v->zoom*-1;
			v->panx=v->panx-gldx;
			v->pany=v->pany+gldy;
		}
		else
		{
			gldx=GetOGLDistance((int)v->mouse.dx)/v->cameras[v->active_camera]->r;
			gldy=GetOGLDistance((int)v->mouse.dy)/v->cameras[v->active_camera]->r;
			v->cameras[v->active_camera]->x-=gldx;
			v->cameras[v->active_camera]->y-=gldy;
			v->cameras[v->active_camera]->targetx-=gldx;
			v->cameras[v->active_camera]->targety+=gldy;
		}

		redraw = TRUE;
		return 1;
}
int glmotion_adjust_pan(ViewInfo* v,float panx,float pany)
{
		float gldx,gldy;
		if(v->active_camera ==-1)
		{
			gldx=GetOGLDistance((int)panx)/v->zoom*-1;
			gldy=GetOGLDistance((int)pany)/v->zoom*-1;
			v->panx=v->panx-gldx;
			v->pany=v->pany+gldy;
		}
		else
		{
			gldx=GetOGLDistance((int)panx)/v->cameras[v->active_camera]->r;
			gldy=GetOGLDistance((int)pany)/v->cameras[v->active_camera]->r;
			v->cameras[v->active_camera]->x-=gldx;
			v->cameras[v->active_camera]->y-=gldy;
			v->cameras[v->active_camera]->targetx-=gldx;
			v->cameras[v->active_camera]->targety+=gldy;
		}

		redraw = TRUE;
		return 1;


}
int glmotion_rotate(ViewInfo * v)
{
	if(v->mouse.rotate_axis==MOUSE_ROTATE_XY)
	{
		v->cameras[v->active_camera]->angley-=v->mouse.dy/7;
		v->cameras[v->active_camera]->anglex-=v->mouse.dx/7;
	}
	if(v->mouse.rotate_axis==MOUSE_ROTATE_Y)
	{
		v->cameras[v->active_camera]->anglex-=v->mouse.dx/7;
	}
	if(v->mouse.rotate_axis==MOUSE_ROTATE_X)
	{
		v->cameras[v->active_camera]->angley-=v->mouse.dy/7;
	}
	if(v->mouse.rotate_axis==MOUSE_ROTATE_Z)
	{
		v->cameras[v->active_camera]->anglez-=v->mouse.dx/7;
		v->cameras[v->active_camera]->anglez-=v->mouse.dy/7;
	}

	return 1;
}
