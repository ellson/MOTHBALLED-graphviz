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

#include "gui.h"
#include "glTemplate.h"
#include "viewport.h"
#include "topview.h"

xdot* testxdot;
GtkWidget *drawing_area;

static float begin_x = 0.0;
static float begin_y = 0.0;

static float dx = 0.0;
static float dy = 0.0;
static guint idle_id = 0;
static GLfloat w,h;
void
init_view (void)
{
  view_quat[0] = view_quat_diff[0] = 0.0;
  view_quat[1] = view_quat_diff[1] = 0.0;
  view_quat[2] = view_quat_diff[2] = 0.0;
  view_quat[3] = view_quat_diff[3] = 1.0;
  view_scale = 1.0;
}
void
examine_gl_config_attrib (GdkGLConfig *glconfig)
{
  g_print ("\nOpenGL visual configurations :\n\n");

  g_print ("gdk_gl_config_is_rgba (glconfig) = %s\n",
           gdk_gl_config_is_rgba (glconfig) ? "TRUE" : "FALSE");
  g_print ("gdk_gl_config_is_double_buffered (glconfig) = %s\n",
           gdk_gl_config_is_double_buffered (glconfig) ? "TRUE" : "FALSE");
  g_print ("gdk_gl_config_is_stereo (glconfig) = %s\n",
           gdk_gl_config_is_stereo (glconfig) ? "TRUE" : "FALSE");
  g_print ("gdk_gl_config_has_alpha (glconfig) = %s\n",
           gdk_gl_config_has_alpha (glconfig) ? "TRUE" : "FALSE");
  g_print ("gdk_gl_config_has_depth_buffer (glconfig) = %s\n",
           gdk_gl_config_has_depth_buffer (glconfig) ? "TRUE" : "FALSE");
  g_print ("gdk_gl_config_has_stencil_buffer (glconfig) = %s\n",
           gdk_gl_config_has_stencil_buffer (glconfig) ? "TRUE" : "FALSE");
  g_print ("gdk_gl_config_has_accum_buffer (glconfig) = %s\n",
           gdk_gl_config_has_accum_buffer (glconfig) ? "TRUE" : "FALSE");

  g_print ("\n");

  print_gl_config_attrib (glconfig, "GDK_GL_USE_GL",           GDK_GL_USE_GL,           TRUE);
  print_gl_config_attrib (glconfig, "GDK_GL_BUFFER_SIZE",      GDK_GL_BUFFER_SIZE,      FALSE);
  print_gl_config_attrib (glconfig, "GDK_GL_LEVEL",            GDK_GL_LEVEL,            FALSE);
  print_gl_config_attrib (glconfig, "GDK_GL_RGBA",             GDK_GL_RGBA,             TRUE);
  print_gl_config_attrib (glconfig, "GDK_GL_DOUBLEBUFFER",     GDK_GL_DOUBLEBUFFER,     TRUE);
  print_gl_config_attrib (glconfig, "GDK_GL_STEREO",           GDK_GL_STEREO,           TRUE);
  print_gl_config_attrib (glconfig, "GDK_GL_AUX_BUFFERS",      GDK_GL_AUX_BUFFERS,      FALSE);
  print_gl_config_attrib (glconfig, "GDK_GL_RED_SIZE",         GDK_GL_RED_SIZE,         FALSE);
  print_gl_config_attrib (glconfig, "GDK_GL_GREEN_SIZE",       GDK_GL_GREEN_SIZE,       FALSE);
  print_gl_config_attrib (glconfig, "GDK_GL_BLUE_SIZE",        GDK_GL_BLUE_SIZE,        FALSE);
  print_gl_config_attrib (glconfig, "GDK_GL_ALPHA_SIZE",       GDK_GL_ALPHA_SIZE,       FALSE);
  print_gl_config_attrib (glconfig, "GDK_GL_DEPTH_SIZE",       GDK_GL_DEPTH_SIZE,       FALSE);
  print_gl_config_attrib (glconfig, "GDK_GL_STENCIL_SIZE",     GDK_GL_STENCIL_SIZE,     FALSE);
  print_gl_config_attrib (glconfig, "GDK_GL_ACCUM_RED_SIZE",   GDK_GL_ACCUM_RED_SIZE,   FALSE);
  print_gl_config_attrib (glconfig, "GDK_GL_ACCUM_GREEN_SIZE", GDK_GL_ACCUM_GREEN_SIZE, FALSE);
  print_gl_config_attrib (glconfig, "GDK_GL_ACCUM_BLUE_SIZE",  GDK_GL_ACCUM_BLUE_SIZE,  FALSE);
  print_gl_config_attrib (glconfig, "GDK_GL_ACCUM_ALPHA_SIZE", GDK_GL_ACCUM_ALPHA_SIZE, FALSE);

  g_print ("\n");
}

void
print_gl_config_attrib (GdkGLConfig *glconfig,
                        const gchar *attrib_str,
                        int          attrib,
                        gboolean     is_boolean)
{
  int value;

  g_print ("%s = ", attrib_str);
  if (gdk_gl_config_get_attrib (glconfig, attrib, &value))
    {
      if (is_boolean)
        g_print ("%s\n", value == TRUE ? "TRUE" : "FALSE");
      else
        g_print ("%d\n", value);
    }
  else
    g_print ("*** Cannot get %s attribute value\n", attrib_str);
}


static void
realize (GtkWidget *widget,
	 gpointer   data)
{
	GdkGLContext *glcontext = gtk_widget_get_gl_context (widget);
	GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable (widget);

	GLfloat ambient[] = {0.0, 0.0, 0.0, 1.0};
	GLfloat diffuse[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat position[] = {0.0, 3.0, 3.0, 0.0};

	GLfloat lmodel_ambient[] = {0.2, 0.2, 0.2, 1.0};
	GLfloat local_view[] = {0.0};
  GLuint texture;


	init_viewport(&view);

//  if(add_graph_to_viewport_from_file ("C:/GTK/2.0/bin/awilliams.dot"))
//	if(add_graph_to_viewport_from_file ("c:/__tempfileneato.xdot"))
	//load default font
	//
#ifdef WIN32
#define SMYRNA_FONT "C:/arial.tga"
// #else
// using -DSMYRNA_FONT from Makefile.am and configure.ac
#endif

  g_print("loading font....%i\n",fontLoad(SMYRNA_FONT));

  /*** OpenGL BEGIN ***/
  if (!gdk_gl_drawable_gl_begin (gldrawable, glcontext))
    return;

  glClearColor (view.bgColor.R,view.bgColor.G, view.bgColor.B,view.bgColor.A);	//background color
  glClearDepth (1.0);

  glLightfv (GL_LIGHT0, GL_AMBIENT, ambient);
  glLightfv (GL_LIGHT0, GL_DIFFUSE, diffuse);
  glLightfv (GL_LIGHT0, GL_POSITION, position);
  glLightModelfv (GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
  glLightModelfv (GL_LIGHT_MODEL_LOCAL_VIEWER, local_view);

  glFrontFace (GL_CW);
 // glEnable (GL_LIGHTING);
//  glEnable (GL_LIGHT0);
  glEnable (GL_AUTO_NORMAL);
  glEnable (GL_NORMALIZE);
  glEnable (GL_DEPTH_TEST);
  glEnable (GL_BLEND); glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDepthFunc (GL_LESS);

  gdk_gl_drawable_gl_end (gldrawable);
  /*** OpenGL END ***/
//create fonts
  return;
}

static gboolean
configure_event (GtkWidget         *widget,
		 GdkEventConfigure *event,
		 gpointer           data)
{

  GLfloat x,y,z;
    int vPort[4];
  float aspect;
  int id;	

	
GdkGLContext *glcontext = gtk_widget_get_gl_context (widget);
  GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable (widget);


  w = widget->allocation.width;
  h = widget->allocation.height;


  /*** OpenGL BEGIN ***/
  if (!gdk_gl_drawable_gl_begin (gldrawable, glcontext))
    return FALSE;

  glViewport (0, 0, w, h);

    /* get current viewport */
    glGetIntegerv (GL_VIEWPORT, vPort);
    /* setup various opengl things that we need */
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  if (w > h)
    {
      aspect = w / h;
      glFrustum (-aspect*100, aspect*100, -100.0, 100.0, 1 ,90);
//      glFrustum (-aspect, aspect, -1.0, 1.0, 5.0, 90.0);
//		glOrtho(-aspect*100, aspect*100, -100.0, 100.0, 5, 90);

  }
  else
    {
      aspect = h / w;
      glFrustum (-100.0, 100.0, -aspect*100, aspect*100,1, 90);
//      glFrustum (-1.0, 1.0, -aspect, aspect, 5.0, 90.0);
//		glOrtho(-100.0, 100.0, -aspect*100, aspect*100,5, 90);
    }

  glMatrixMode (GL_MODELVIEW);
  gdk_gl_drawable_gl_end (gldrawable);

  /*** OpenGL END ***/


  return TRUE;
}


static GLuint id_text=NULL;

gboolean
expose_event (GtkWidget      *widget,
	      GdkEventExpose *event,
	      gpointer        data)
{
	GdkGLContext *glcontext = gtk_widget_get_gl_context (widget);
	GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable (widget);
	xdot* xDot;
  GLfloat x,y,z;
  GLfloat m[4][4];



	/*** OpenGL BEGIN ***/
	if (!gdk_gl_drawable_gl_begin (gldrawable, glcontext))
		return FALSE;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity ();

	/* View transformation. */
	//0 center for x -> for y to up
//	glTranslatef (view.panx,view.pany,-20);
//	glTranslatef (1,1,view.zoom/5);
	gluLookAt(view.panx,view.pany,view.zoom*-1,view.panx,view.pany,0.0,0.0,1.0,0.0);

//	glScalef (view.zoom/2,view.zoom/2,view.zoom/2*-1);

//	add_quats (view_quat_diff, view_quat, view_quat);
//	build_rotmatrix (m, view_quat);
//	glMultMatrixf (&m[0][0]);

//update view clip region values
  GetOGLPosRef(1,(int)h-5,&view.clipX1,&view.clipY1,&view.clipZ1);
  GetOGLPosRef((int)w-1,1,&view.clipX2,&view.clipY2,&view.clipZ2);
  DrawView();
  if(((view.mouse.mouse_mode==4) || (view.mouse.mouse_mode==5))&& view.mouse.mouse_down)
		draw_selection_box();

/*	if((view.mouse_mode==10) && view.mousedown)
		move_nodes(view.g[view.activeGraph],view.GLx2-view.GLx,view.GLy2-view.GLy);	//move selected nodes*/ 



	//drawing grids
	if(view.gridVisible)
	{
		glPointSize	(1);
		glBegin(GL_POINTS);
		glColor4f(view.grR,view.grG,view.grB,view.grA);
		for (x=view.bdxLeft; x <= view.bdxRight;x=x +view.gridSize)
		{
			for (y=view.bdyBottom; y <=view.bdyTop ;y=y +view.gridSize)
			{
					glVertex3f(x,y,0);			
			}
		}
		glEnd();
	}
	draw_fisheye_magnifier();
	draw_magnifier();
  /* Swap buffers */
if (gdk_gl_drawable_is_double_buffered (gldrawable))
    gdk_gl_drawable_swap_buffers (gldrawable);
  else
    glFlush ();
  gdk_gl_drawable_gl_end (gldrawable);
  /*** OpenGL END ***/
  return TRUE;
}
static gboolean
button_press_event (GtkWidget      *widget,
		    GdkEventButton *event,
		    gpointer        data)
{
	if (animate)
    {
      if (event->button == 1)
        toggle_animation (widget);
    }
  else
    {
      view_quat_diff[0] = 0.0;
      view_quat_diff[1] = 0.0;
      view_quat_diff[2] = 0.0;
      view_quat_diff[3] = 1.0;
    }


	begin_x = event->x;
	begin_y = event->y;


	if(event->button==1)
	{
		view.mouse.mouse_down=1;
		if(GetOGLPos(begin_x,begin_y))
		{
			if (view.mouse.mouse_mode == MM_SINGLE_SELECT)	//single select
			{
				view.Selection.Active=1;
				view.Selection.Type=0;
				view.Selection.AlreadySelected=0;
				view.Selection.X=view.GLx-SINGLE_SELECTION_WIDTH/2;
				view.Selection.Y=view.GLy-SINGLE_SELECTION_WIDTH/2;
				view.Selection.W=SINGLE_SELECTION_WIDTH;
				view.Selection.H=SINGLE_SELECTION_WIDTH;
				expose_event (drawing_area,NULL,NULL);
			}
			view.prevpanx=view.panx;
			view.prevpany=view.pany;
		}
	}	
	return FALSE;
}

static gboolean
button_release_event (GtkWidget      *widget,
                      GdkEventButton *event,
                      gpointer        data)
{

	if(event->button==1)	//left click release
	{
		view.mouse.mouse_down=0;
		if (view.mouse.mouse_mode==MM_PAN)	//pan
		{
/*			view.panx=view.prevpanx-(view.GLx2-view.GLx);
			view.pany=view.prevpany-(view.GLy2-view.GLy);
			expose_event (drawing_area,NULL,NULL);*/
		}


		if ((view.mouse.mouse_mode==MM_RECTANGULAR_SELECT)||(view.mouse.mouse_mode==MM_RECTANGULAR_X_SELECT))
		{
			if (view.GLx <= view.GLx2)
				view.Selection.X=view.GLx;
			else
				view.Selection.X=view.GLx2;
			if(view.GLy <= view.GLy2)
				view.Selection.Y=view.GLy;
			else
				view.Selection.Y=view.GLy2;

			view.Selection.W=view.GLx2-view.GLx;
			if(view.Selection.W < 0)
				view.Selection.W=view.Selection.W*-1;
			view.Selection.H=view.GLy2-view.GLy;
			if(view.Selection.H < 0)
				view.Selection.H=view.Selection.H*-1;
			if(view.mouse.mouse_mode==4)
				view.Selection.Type=1;
			else
				view.Selection.Type=2;
			view.Selection.Active=1;
			expose_event (drawing_area,NULL,NULL);
		}
		if (view.mouse.mouse_mode==MM_MOVE)
		{
			if(((custom_graph_data*)AGDATA(view.g[view.activeGraph]))->TopView == 0)
				move_nodes(view.g[view.activeGraph]);
			else
				move_TVnodes();

		}
		if ((view.mouse.mouse_mode==MM_FISHEYE_MAGNIFIER)||(view.mouse.mouse_mode==MM_MAGNIFIER)) //fisheye mag mouse release, stop distortion
		{
			originate_distorded_coordinates(&Topview);
			expose_event (drawing_area,NULL,NULL);
		}
	}
  dx = 0.0;
  dy = 0.0;
  return FALSE;
}

static gboolean
motion_notify_event (GtkWidget      *widget,
		     GdkEventMotion *event,
		     gpointer        data)
{
	float w = widget->allocation.width;
	float h = widget->allocation.height;
	float x = event->x;
	float y = event->y;

	gboolean redraw = FALSE;

  

	dx = x - begin_x;
	dy = y - begin_y;
  //panning
    if ((event->state & GDK_BUTTON1_MASK) && (view.mouse.mouse_mode==MM_PAN))
	{	

//		GetFixedOGLPos((int)x,(int)y,view.GLDepth);
//		view.panx=view.prevpanx-(view.GLx2-view.GLx);
//		view.pany=view.prevpany-(view.GLy2-view.GLy);
		view.panx=view.panx-dx*pow(view.zoom*-1,(1/1));
		view.pany=view.pany+dy*pow(view.zoom*-1,(1/1));
		redraw = TRUE;
	}
	//zooming
    if ((event->state & GDK_BUTTON1_MASK) && (view.mouse.mouse_mode==MM_ZOOM))
	{	
		view.zoom=view.zoom+dx/10*(view.zoom*-1/20);
			if(view.zoom > MAX_ZOOM)
				view.zoom=MAX_ZOOM;
			if(view.zoom < MIN_ZOOM)
				view.zoom=MIN_ZOOM;
			redraw = TRUE;

	}


  /* Rotation. */
	if ((event->state & GDK_BUTTON1_MASK) && (view.mouse.mouse_mode==MM_ROTATE))
	{	
		trackball (view_quat_diff,
			(2.0 * begin_x - w) / w,
			(h - 2.0 * begin_y) / h,
			(2.0 * x - w) / w,
			(h - 2.0 * y) / h);


		redraw = TRUE;
	}

  /* Scaling. */
  if (event->state & GDK_BUTTON2_MASK)
    {
      view_scale = view_scale * (1.0 + (y - begin_y) / h);
      if (view_scale > VIEW_SCALE_MAX)
	view_scale = VIEW_SCALE_MAX;
      else if (view_scale < VIEW_SCALE_MIN)
	view_scale = VIEW_SCALE_MIN;

      redraw = TRUE;
    }
  /*selection rect*/
	if ((event->state & GDK_BUTTON1_MASK) && ((view.mouse.mouse_mode==MM_RECTANGULAR_SELECT)||(view.mouse.mouse_mode==5)))
	{	
		GetFixedOGLPos((int)x,(int)y,view.GLDepth);
		redraw = TRUE;
	}
	if ((event->state & GDK_BUTTON1_MASK) && (view.mouse.mouse_mode==MM_MOVE))
	{	
		GetFixedOGLPos((int)x,(int)y,view.GLDepth);
		redraw = TRUE;
	}
	if ((event->state & GDK_BUTTON1_MASK) && ((view.mouse.mouse_mode==MM_MAGNIFIER)
		||(view.mouse.mouse_mode==MM_FISHEYE_MAGNIFIER) ))
	{	
	    view.mouse.mouse_X=x;
		view.mouse.mouse_Y=y;
		redraw = TRUE;
	}

	
	begin_x = x;
  begin_y = y;


  if (redraw && !animate)
    gdk_window_invalidate_rect (widget->window, &widget->allocation, FALSE);
  return TRUE;
}

static gboolean
key_press_event (GtkWidget   *widget,
		 GdkEventKey *event,
		 gpointer     data)
{
  switch (event->keyval)
    {
    case GDK_Escape:
      gtk_main_quit ();
      break;

    default:
      return FALSE;
    }

  return TRUE;
}

static gboolean
idle (GtkWidget *widget)
{
  /* Invalidate the whole window. */
  gdk_window_invalidate_rect (widget->window, &widget->allocation, FALSE);

  /* Update synchronously. */
  gdk_window_process_updates (widget->window, FALSE);

  return TRUE;
}

static void
idle_add (GtkWidget *widget)
{
  if (idle_id == 0)
    {
      idle_id = g_idle_add_full (GDK_PRIORITY_REDRAW,
                                 (GSourceFunc) idle,
                                 widget,
                                 NULL);
    }
}

static void
idle_remove (GtkWidget *widget)
{
  if (idle_id != 0)
    {
      g_source_remove (idle_id);
      idle_id = 0;
    }
}




static gboolean
map_event (GtkWidget *widget,
	   GdkEvent  *event,
	   gpointer   data)
{
  if (animate)
    idle_add (widget);

  return TRUE;
}

static gboolean
unmap_event (GtkWidget *widget,
	     GdkEvent  *event,
	     gpointer   data)
{
  idle_remove (widget);

  return TRUE;
}

static gboolean
visibility_notify_event (GtkWidget          *widget,
			 GdkEventVisibility *event,
			 gpointer            data)
{
  if (animate)
    {
      if (event->state == GDK_VISIBILITY_FULLY_OBSCURED)
	idle_remove (widget);
      else
	idle_add (widget);
    }

  return TRUE;
}

/* Toggle animation.*/
static void
toggle_animation (GtkWidget *widget)
{
  animate = !animate;

  if (animate)
    {
      idle_add (widget);
    }
  else
    {
      idle_remove (widget);
      view_quat_diff[0] = 0.0;
      view_quat_diff[1] = 0.0;
      view_quat_diff[2] = 0.0;
      view_quat_diff[3] = 1.0;
      gdk_window_invalidate_rect (widget->window, &widget->allocation, FALSE);
    }
}

static void
change_shape (GtkMenuItem  *menuitem,
              const GLuint *shape)
{
  shape_current = *shape;
  init_view ();
}

static void
change_material (GtkMenuItem  *menuitem,
                 MaterialProp *mat)
{
  mat_current = mat;
}
void 
switch_Mouse (GtkMenuItem *menuitem,int mouse_mode)
{
  GdkCursor* cursor;
	view.mouse.mouse_mode=mouse_mode;

}


/* For popup menu. */
static gboolean
button_press_event_popup_menu (GtkWidget      *widget,
			       GdkEventButton *event,
			       gpointer        data)
{
  if (event->button == 3)
    {
      /* Popup menu. */
      gtk_menu_popup (GTK_MENU (widget), NULL, NULL, NULL, NULL,
		      event->button, event->time);
      return TRUE;
    }

  return FALSE;
}

/* Creates the popup menu.*/
static GtkWidget *
create_popup_menu (GtkWidget *drawing_area)
{
  GtkWidget *shapes_menu;
  GtkWidget *actions_menu;
  GtkWidget *editing_menu;
  GtkWidget *menu;
  GtkWidget *menu_item;
	int mm=0;
  /*actions sub menu*/
	//PAN	

mm=MM_PAN;
  actions_menu = gtk_menu_new ();
  menu_item = gtk_menu_item_new_with_label ("Pan");
  gtk_menu_shell_append (GTK_MENU_SHELL (actions_menu), menu_item);
  g_signal_connect (G_OBJECT (menu_item), "activate",
                    G_CALLBACK (switch_Mouse), (gpointer) mm);
  gtk_widget_show (menu_item);
/**********/
	//ZOOM
  mm=MM_ZOOM;
  menu_item = gtk_menu_item_new_with_label ("Zoom");
  gtk_menu_shell_append (GTK_MENU_SHELL (actions_menu), menu_item);
  g_signal_connect (G_OBJECT (menu_item), "activate",
                    G_CALLBACK (switch_Mouse), (gpointer) mm);
  gtk_widget_show (menu_item);
/**********/
	//ROTATE
	mm=MM_ROTATE;
  menu_item = gtk_menu_item_new_with_label ("rotate");
  gtk_menu_shell_append (GTK_MENU_SHELL (actions_menu), menu_item);
  g_signal_connect (G_OBJECT (menu_item), "activate",
                    G_CALLBACK (switch_Mouse), (gpointer) mm);
  gtk_widget_show (menu_item);
/**********/
/**********/
	//Single Select
	mm=MM_SINGLE_SELECT;
  menu_item = gtk_menu_item_new_with_label ("select");
  gtk_menu_shell_append (GTK_MENU_SHELL (actions_menu), menu_item);
  g_signal_connect (G_OBJECT (menu_item), "activate",
                    G_CALLBACK (switch_Mouse), (gpointer) mm);
  gtk_widget_show (menu_item);
/**********/
	//Rectangle Select
	mm=MM_RECTANGULAR_SELECT;
  menu_item = gtk_menu_item_new_with_label ("rect select");
  gtk_menu_shell_append (GTK_MENU_SHELL (actions_menu), menu_item);
  g_signal_connect (G_OBJECT (menu_item), "activate",
                    G_CALLBACK (switch_Mouse), (gpointer) mm);
  gtk_widget_show (menu_item);
/**********/
/**********/
	//Rectangle -x Select
	mm=MM_RECTANGULAR_X_SELECT;
  menu_item = gtk_menu_item_new_with_label ("rect-x select");
  gtk_menu_shell_append (GTK_MENU_SHELL (actions_menu), menu_item);
  g_signal_connect (G_OBJECT (menu_item), "activate",
                    G_CALLBACK (switch_Mouse), (gpointer) mm);
  gtk_widget_show (menu_item);
/**********/
/**********/
	//Move
	mm=MM_MOVE;
  menu_item = gtk_menu_item_new_with_label ("Move");
  gtk_menu_shell_append (GTK_MENU_SHELL (actions_menu), menu_item);
  g_signal_connect (G_OBJECT (menu_item), "activate",
                    G_CALLBACK (switch_Mouse), (gpointer) mm);
  gtk_widget_show (menu_item);
/**********/
	//activate magnifier
  mm=MM_MAGNIFIER;		//magnifier ,fisheye etc starts at 20
  menu_item = gtk_menu_item_new_with_label ("Magnifier");
  gtk_menu_shell_append (GTK_MENU_SHELL (actions_menu), menu_item);
  g_signal_connect (G_OBJECT (menu_item), "activate",
                    G_CALLBACK (switch_Mouse), (gpointer) mm);
  gtk_widget_show (menu_item);
/**********/
	//activate fisheye magnifier
  mm=MM_FISHEYE_MAGNIFIER;
  menu_item = gtk_menu_item_new_with_label ("Fisheye Magnifier");
  gtk_menu_shell_append (GTK_MENU_SHELL (actions_menu), menu_item);
  g_signal_connect (G_OBJECT (menu_item), "activate",
                    G_CALLBACK (switch_Mouse), (gpointer) mm);
  gtk_widget_show (menu_item);
/**********/
  editing_menu = gtk_menu_new ();
  /* NODE */
  menu_item = gtk_menu_item_new_with_label ("Node");
  gtk_menu_shell_append (GTK_MENU_SHELL (editing_menu), menu_item);
  g_signal_connect (G_OBJECT (menu_item), "activate",
                    G_CALLBACK (change_material), &mat_emerald);
  gtk_widget_show (menu_item);

  /* EDGE */
  menu_item = gtk_menu_item_new_with_label ("Edge");
  gtk_menu_shell_append (GTK_MENU_SHELL (editing_menu), menu_item);
  g_signal_connect (G_OBJECT (menu_item), "activate",
                    G_CALLBACK (change_material), &mat_jade);
  gtk_widget_show (menu_item);


  menu = gtk_menu_new ();

  /* Actions */
  menu_item = gtk_menu_item_new_with_label ("Mouse");
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (menu_item), actions_menu);
  gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
  gtk_widget_show (menu_item);

  /* NEW */
  menu_item = gtk_menu_item_new_with_label ("New");
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (menu_item), editing_menu);
  gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
  gtk_widget_show (menu_item);

  /* Quit */
  menu_item = gtk_menu_item_new_with_label ("Quit");
  gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
  g_signal_connect (G_OBJECT (menu_item), "activate",
		    G_CALLBACK (gtk_main_quit), NULL);
  gtk_widget_show (menu_item);
  return menu;
}



extern GdkGLConfig *
configure_gl (void)
{
  GdkGLConfig *glconfig;

  /* Try double-buffered visual */
  glconfig = gdk_gl_config_new_by_mode (GDK_GL_MODE_RGB    |
					GDK_GL_MODE_DEPTH  |
					GDK_GL_MODE_DOUBLE);
  if (glconfig == NULL)
    {
      g_print ("\n*** Cannot find the double-buffered visual.\n");
      g_print ("\n*** Trying single-buffered visual.\n");

      /* Try single-buffered visual */
      glconfig = gdk_gl_config_new_by_mode (GDK_GL_MODE_RGB   |
					    GDK_GL_MODE_DEPTH);
      if (glconfig == NULL)
	{
	  g_print ("*** No appropriate OpenGL-capable visual found.\n");
	  exit (1);
	}
    }

  return glconfig;
}


void create_window (GdkGLConfig *glconfig,GtkWidget* vbox)
{
  gint major, minor;

  GtkWidget *window;
  GtkWidget *menu;
  GtkWidget *button;
  /*
   * Query OpenGL extension version.
   */

  gdk_gl_query_version (&major, &minor);
  g_print ("\nOpenGL extension version - %d.%d\n",
           major, minor);

  /*
   * Configure OpenGL-capable visual.
   */

  /* Try double-buffered visual */

 if (IS_TEST_MODE_ON)	//printf some gl values, to test if your system has opengl stuff
     examine_gl_config_attrib (glconfig); 
 /* Drawing area for drawing OpenGL scene.
   */
  drawing_area = gtk_drawing_area_new ();
  gtk_widget_set_size_request (drawing_area, 300, 300);
  /* Set OpenGL-capability to the widget. */
  gtk_widget_set_gl_capability (drawing_area,
				glconfig,
				NULL,
				TRUE,
				GDK_GL_RGBA_TYPE);

  gtk_widget_add_events (drawing_area,
			 GDK_BUTTON1_MOTION_MASK    |
			 GDK_BUTTON2_MOTION_MASK    |
			 GDK_BUTTON_PRESS_MASK      |
                         GDK_BUTTON_RELEASE_MASK    |
			 GDK_VISIBILITY_NOTIFY_MASK);

  g_signal_connect_after (G_OBJECT (drawing_area), "realize",
                          G_CALLBACK (realize), NULL);
  g_signal_connect (G_OBJECT (drawing_area), "configure_event",
		    G_CALLBACK (configure_event), NULL);
  g_signal_connect (G_OBJECT (drawing_area), "expose_event",
		    G_CALLBACK (expose_event), NULL);

  g_signal_connect (G_OBJECT (drawing_area), "button_press_event",
		    G_CALLBACK (button_press_event), NULL);
  g_signal_connect (G_OBJECT (drawing_area), "button_release_event",
		    G_CALLBACK (button_release_event), NULL);
  g_signal_connect (G_OBJECT (drawing_area), "motion_notify_event",
		    G_CALLBACK (motion_notify_event), NULL);

  g_signal_connect (G_OBJECT (drawing_area), "map_event",
		    G_CALLBACK (map_event), NULL);
  g_signal_connect (G_OBJECT (drawing_area), "unmap_event",
		    G_CALLBACK (unmap_event), NULL);
  g_signal_connect (G_OBJECT (drawing_area), "visibility_notify_event",
		    G_CALLBACK (visibility_notify_event), NULL);





  gtk_box_pack_start (GTK_BOX (vbox), drawing_area, TRUE, TRUE, 0);

  gtk_widget_show (drawing_area);

  /*
   * Popup menu.
   */

  menu = create_popup_menu (drawing_area);

  /* Signal handler */
  g_signal_connect_swapped (G_OBJECT (drawing_area), "button_press_event",
                            G_CALLBACK (button_press_event_popup_menu), menu);

}

//this piece of code returns the opengl coordinates of mouse coordinates
int GetOGLPos(int x, int y)
{
	GLdouble wwinX;
	GLdouble wwinY;
	GLdouble wwinZ;


	int ind;
	GLdouble depth[5];
	GLdouble raster[5];
	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];
	GLfloat winX, winY;
	GLfloat winZ[36];
	GLdouble posX, posY, posZ;
	char buffer [200];
	float kts=1;
	//glTranslatef (0.0,0.0,0.0);
	glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
	glGetDoublev( GL_PROJECTION_MATRIX, projection );
	glGetIntegerv( GL_VIEWPORT, viewport );

	//draw a point	to a not important location to get window coordinates
	glBegin(GL_POINTS);
		glVertex3f(10.00,10.00,0.00);
	glEnd();
    gluProject(10.0,10.0,0.00,modelview,projection,viewport,&wwinX,&wwinY,&wwinZ );
	winX = (float)x;
	winY = (float)viewport[3] - (float)y;
	gluUnProject( winX, winY, wwinZ, modelview, projection, viewport, &posX, &posY, &posZ);

	view.GLx=posX;
	view.GLy=posY;
	view.GLz=posZ;
	view.GLDepth=kts;
		return 1;

}
//some functions set the caches the depth value ,view.GLDepth
int GetFixedOGLPos(int x, int y,float kts)
{
	GLdouble wwinX;
	GLdouble wwinY;
	GLdouble wwinZ;

	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];
	GLfloat winX, winY;
	GLdouble posX, posY, posZ;

	glBegin(GL_POINTS);
		glVertex3f(10.00,10.00,0.00);
	glEnd();

	glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
	glGetDoublev( GL_PROJECTION_MATRIX, projection );
	glGetIntegerv( GL_VIEWPORT, viewport );

	gluProject(10.0,10.0,0.00,modelview,projection,viewport,&wwinX,&wwinY,&wwinZ );


//	glTranslatef (0.0,0.0,0.0);

	winX = (float)x;
	winY = (float)viewport[3] - (float)y;
	gluUnProject( winX, winY, wwinZ, modelview, projection, viewport, &posX, &posY, &posZ);
	view.GLx2=posX;
	view.GLy2=posY;
	view.GLz2=posZ;
	return 1;

}

int GetOGLPosRef(int x, int y,float* X,float* Y,float* Z)
{

	GLdouble wwinX;
	GLdouble wwinY;
	GLdouble wwinZ;
	GLdouble posX, posY, posZ;


	int ind;
	GLdouble depth[5];
	GLdouble raster[5];
	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];
	GLfloat winX, winY;
	GLfloat winZ[36];
	char buffer [200];
	float kts=1;
	//glTranslatef (0.0,0.0,0.0);
	glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
	glGetDoublev( GL_PROJECTION_MATRIX, projection );
	glGetIntegerv( GL_VIEWPORT, viewport );

	//draw a point	to a not important location to get window coordinates
	glBegin(GL_POINTS);
		glVertex3f(10.00,10.00,0.00);
	glEnd();
    gluProject(10.0,10.0,0.00,modelview,projection,viewport,&wwinX,&wwinY,&wwinZ );
	winX = (float)x;
	winY = (float)viewport[3] - (float)y;
	gluUnProject( winX, winY, wwinZ, modelview, projection, viewport, &posX, &posY, &posZ);

	*X=posX;
	*Y=posY;
	*Z=posZ;
	return 1;

}


float GetOGLDistance(int l)
{

	int x,y;
	float*  X,Y, Z;
	GLdouble wwinX;
	GLdouble wwinY;
	GLdouble wwinZ;
	GLdouble posX, posY, posZ;
	GLdouble posXX, posYY, posZZ;


	int ind;
	GLdouble depth[5];
	GLdouble raster[5];
	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];
	GLfloat winX, winY;
	GLfloat winZ[36];
	char buffer [200];
	float kts=1;
	//glTranslatef (0.0,0.0,0.0);
	glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
	glGetDoublev( GL_PROJECTION_MATRIX, projection );
	glGetIntegerv( GL_VIEWPORT, viewport );

	//draw a point	to a not important location to get window coordinates
	glBegin(GL_POINTS);
		glVertex3f(10.00,10.00,0.00);
	glEnd();
    gluProject(10.0,10.0,0.00,modelview,projection,viewport,&wwinX,&wwinY,&wwinZ );
	x=50;
	y=50;
	winX = (float)x;
	winY = (float)viewport[3] - (float)y;
	gluUnProject( winX, winY, wwinZ, modelview, projection, viewport, &posX, &posY, &posZ);
	x=x+l;
	y=50;
	winX = (float)x;
	winY = (float)viewport[3] - (float)y;
	gluUnProject( winX, winY, wwinZ, modelview, projection, viewport, &posXX, &posYY, &posZZ);
	return (posXX-posX);
}



//////////////////////////GGGGGG////////////////////////////////////////////////
/////////////////////////GG/////G///////////////////////////////////////////////
/////////////////////////GG/////////////////////////////////////////////////////
/////////////////////////GG//GGG////////////////////////////////////////////////
/////////////////////////GG/////G///////////////////////////////////////////////
/////////////////////////GGG////G///////////////////////////////////////////////
///////////////////////////GGGGG////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//2D PRIMITIVES
//all these functions should be moved to another file
//for testing i ll ise this file


#if 0
void loadDLL (char* file)
{
	if(lt_dlinit ()== 0)
	{
	//	g_print("libltdl has been successfully initialized\n");

		if(lt_dlopen (file))
           g_print("%s has been loaded\n", file);
		else
           g_print("%s failed\n", file);
	}
}
#endif

void DrawView()
{
	if(view.activeGraph > -1)
	{
		
		if(((custom_graph_data*)AGDATA(view.g[view.activeGraph]))->TopView)
			drawTopViewGraph(view.g[view.activeGraph]);//topview style dots and straight lines
		else	
			drawGraph(view.g[view.activeGraph]);//xdot based drawing functions
	}

}



