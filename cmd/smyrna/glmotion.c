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
	return 1;

	/*zooming */
    if ((event->state & GDK_BUTTON1_MASK)&& (v->mouse.mouse_mode == MM_ZOOM))
		glmotion_zoom(v);


	/*selection rect */
    if ((event->state & GDK_BUTTON1_MASK)
	&& ((v->mouse.mouse_mode == MM_RECTANGULAR_SELECT)
	    || (v->mouse.mouse_mode == 5)))
	{
			GetFixedOGLPos((int) v->mouse.mouse_X, (int) v->mouse.mouse_Y, v->GLDepth, &(v->GLx2),
		       &(v->GLy2), &(v->GLz2));
		redraw = TRUE;
    }
    if ((event->state & GDK_BUTTON1_MASK)
	&& (v->mouse.mouse_mode == MM_MOVE)) {
	GetFixedOGLPos((int) v->mouse.mouse_X, (int) v->mouse.mouse_Y, v->GLDepth, &(v->GLx2),
		       &(v->GLy2), &(v->GLz2));
	redraw = TRUE;
    }
/*    if ((event->state & GDK_BUTTON1_MASK)
	&& ((v->mouse.mouse_mode == MM_MAGNIFIER)
	    || (v->mouse.mouse_mode == MM_FISHEYE_MAGNIFIER))) {
	v->mouse.mouse_X = (int) x;
	v->mouse.mouse_Y = (int) y;
	redraw = TRUE;
    }*/

    if (redraw)
	gdk_window_invalidate_rect(widget->window, &widget->allocation,
				   FALSE);

	return 1;
}
int glmotion_zoom(ViewInfo * v)
{
	char buf[256];

	float real_zoom,xx;
	if(v->active_camera==-1)
		real_zoom=v->zoom + v->mouse.dx / 10 * (v->zoom * -1 / 20);
	else
		real_zoom=(v->cameras[v->active_camera]->r + v->mouse.dx / 10 * (v->cameras[v->active_camera]->r  / 20))*-1;

	if (real_zoom > MAX_ZOOM)
		real_zoom = (float) MAX_ZOOM;
	if (real_zoom < MIN_ZOOM)
		real_zoom = (float) MIN_ZOOM;
	if(v->active_camera==-1)
		v->zoom = real_zoom;
	else
		v->cameras[v->active_camera]->r=real_zoom*-1;
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
			gldx=GetOGLDistance(v->mouse.dx)/v->zoom*-1;
			gldy=GetOGLDistance(v->mouse.dy)/v->zoom*-1;
			v->panx=v->panx-gldx;
			v->pany=v->pany+gldy;
		}
		else
		{
			gldx=GetOGLDistance(v->mouse.dx)/v->cameras[v->active_camera]->r;
			gldy=GetOGLDistance(v->mouse.dy)/v->cameras[v->active_camera]->r;
			v->cameras[v->active_camera]->x-=gldx;
			v->cameras[v->active_camera]->y-=gldy;
			v->cameras[v->active_camera]->targetx-=gldx;
			v->cameras[v->active_camera]->targety+=gldy;
		}

		redraw = TRUE;
}
int glmotion_adjust_pan(ViewInfo* v,float panx,float pany)
{
		float gldx,gldy;
		if(v->active_camera ==-1)
		{
			gldx=GetOGLDistance(panx)/v->zoom*-1;
			gldy=GetOGLDistance(pany)/v->zoom*-1;
			v->panx=v->panx-gldx;
			v->pany=v->pany+gldy;
		}
		else
		{
			gldx=GetOGLDistance(panx)/v->cameras[v->active_camera]->r;
			gldy=GetOGLDistance(pany)/v->cameras[v->active_camera]->r;
			v->cameras[v->active_camera]->x-=gldx;
			v->cameras[v->active_camera]->y-=gldy;
			v->cameras[v->active_camera]->targetx-=gldx;
			v->cameras[v->active_camera]->targety+=gldy;
		}

		redraw = TRUE;

}
int glmotion_rotate(ViewInfo * v)
{
	if(v->mouse.rotate_axis==MOUSE_ROTATE_XY)
	{
		v->cameras[v->active_camera]->angley-=v->mouse.dy/5;
		v->cameras[v->active_camera]->anglex-=v->mouse.dx/5;
	}
	if(v->mouse.rotate_axis==MOUSE_ROTATE_X)
	{
		v->cameras[v->active_camera]->anglex-=v->mouse.dx/5;
	}
	if(v->mouse.rotate_axis==MOUSE_ROTATE_Y)
	{
		v->cameras[v->active_camera]->angley-=v->mouse.dy/5;
	}
	if(v->mouse.rotate_axis==MOUSE_ROTATE_Z)
	{
		v->cameras[v->active_camera]->anglez-=v->mouse.dx/5;
	}

	return 1;
}
