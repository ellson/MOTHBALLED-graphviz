/* $Id$Revision: */
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

#include "appmouse.h"
#include "topfisheyeview.h"
#include "arcball.h"
#include "topview.h"
#include "glmotion.h"
#include "beacon.h"


    static float prevX=0;
    static float prevY=0;


static int singleclick(ViewInfo* v)
{
       return(((int)v->mouse.initPos.x == (int)v->mouse.finalPos.x) && ((int)v->mouse.initPos.y == (int)v->mouse.finalPos.y));

}
static void appmouse_left_click(ViewInfo* v,int x,int y)
{
	if (v->mouse.mouse_mode == MM_SINGLE_SELECT)	//single select
	{
	    v->Selection.Active = 1;
	    v->Selection.Type = 0;
	    v->Selection.AlreadySelected = 0;
	}

}
static void appmouse_right_click(ViewInfo* v,int x,int y)
{
    
    pick_node_from_coords(view->mouse.GLfinalPos.x,view->mouse.GLfinalPos.y,view->mouse.GLfinalPos.z);
}


static void appmouse_down(ViewInfo* v,int x,int y)
{
    v->mouse.down=1;
    v->mouse.initPos.x=x;
    v->mouse.initPos.y=y;
    v->mouse.pos.x=x;
    v->mouse.pos.y=y;

    GetFixedOGLPos((float) x,y, v->GLDepth,&v->mouse.GLinitPos.x,&v->mouse.GLinitPos.y,&v->mouse.GLinitPos.z);
    GetFixedOGLPos((float) x,y,v->GLDepth, &v->mouse.GLpos.x,&v->mouse.GLpos.y,&v->mouse.GLpos.z);

    prevX=0;
    prevY=0;



}
static void appmouse_up(ViewInfo* v,int x,int y)
{
       v->mouse.down=0;
       v->mouse.finalPos.x=x;
       v->mouse.finalPos.y=y;
       GetFixedOGLPos((float) x,y,v->GLDepth, &v->mouse.GLfinalPos.x,&v->mouse.GLfinalPos.y,&v->mouse.GLfinalPos.z);
	if(singleclick(v))
	{
	   if (v->mouse.t==glMouseLeftButton)
	       appmouse_left_click(v,x,y);
	   if (v->mouse.t==glMouseRightButton)
	       appmouse_right_click(v,x,y);
       }

}
static void appmouse_drag(ViewInfo* v,int x,int y)
{
    static float x2,y2;
    v->mouse.pos.x=x;
    v->mouse.pos.y=y;
    GetFixedOGLPos((float) x,y,v->GLDepth, &v->mouse.GLpos.x,&v->mouse.GLpos.y,&v->mouse.GLpos.z);
    x2=v->mouse.GLpos.x;
    y2=v->mouse.GLpos.y;
//    v->mouse.dragX=x2-prevX;
//    v->mouse.dragY=y2-prevY;
    //printf ("(%f,%f)->(%f,%f) : %f  %f \n",prevX,prevY,v->mouse.GLpos.x,v->mouse.GLpos.y,v->mouse.dragX,v->mouse.dragY);

    prevX=x2;
    prevY=y2;
//    printf ("(%f,%f)->(%f,%f) : %f  %f \n",v->mouse.pos.x,v->mouse.GLinitPos.y,v->mouse.GLfinalPos.x,v->mouse.GLfinalPos.y,v->mouse.dragX,v->mouse.dragY);

}

void appmouse_left_click_down(ViewInfo* v,int x,int y)
{
       v->mouse.t=glMouseLeftButton;
       appmouse_down(v,x,y);


}
void appmouse_left_click_up(ViewInfo* v,int x,int y)
{
	appmouse_up(v,x,y);
	if (v->mouse.mouse_mode == MM_MOVE)
	    move_TVnodes();

	if ((v->mouse.mouse_mode == MM_FISHEYE_MAGNIFIER) || (v->mouse.mouse_mode == MM_MAGNIFIER))	//fisheye mag mouse release, stop distortion
	{
	    originate_distorded_coordinates(v->Topview);
	}


}
void appmouse_left_drag(ViewInfo* v,int x,int y)
{
    appmouse_drag(v,x,y);
    switch (view->mouse.mouse_mode)
    {
	case MM_PAN:
		glmotion_pan(v);
		break;
	case  MM_ROTATE:
		view->arcball->MousePt.s.X = (GLfloat) x;
		view->arcball->MousePt.s.Y = (GLfloat) y;
		if (!view->arcball->isDragging) {
		    arcmouseClick(view);
		    view->arcball->isDragging = 1;

		} else 
		    arcmouseDrag(view);
		break;
    }

}
void appmouse_right_click_down(ViewInfo* v,int x,int y)
{
    v->mouse.t=glMouseRightButton;
    appmouse_down(v,x,y);

    view->Selection.X = view->mouse.GLpos.x;
    view->Selection.Y = view->mouse.GLpos.y;

}
void appmouse_right_click_up(ViewInfo* v,int x,int y)
{
    appmouse_up(v,x,y);
    if(singleclick(v))
    {
	if (view->activeGraph >= 0) 
	{
	    if (view->Topview->is_top_fisheye) 
		changetopfishfocus(view->Topview,&view->mouse.pos.x,&view->mouse.pos.y,  0, 1);
	    else    /*single right click*/
/*	pick_node_from_coords(view->mouse.GLpos.x, view->mouse.GLpos.y,
			      view->mouse.GLpos.z);*/

	    {;}


	}

    }
    else    /*update selection values*/
    {
	if (v->mouse.GLinitPos.x <= v->mouse.GLfinalPos.x)
	    v->Selection.X = v->mouse.GLinitPos.x;
	else
	    v->Selection.X = v->mouse.GLfinalPos.x;
	if (v->mouse.GLinitPos.y <= v->mouse.GLfinalPos.y)
	    v->Selection.Y = v->mouse.GLinitPos.y;
	else
	    v->Selection.Y = v->mouse.GLfinalPos.y;
	v->Selection.W = ABS(v->mouse.GLfinalPos.x - v->mouse.GLinitPos.x);
	v->Selection.H = ABS(v->mouse.GLfinalPos.y - v->mouse.GLinitPos.y);
	if (v->Selection.H < 0)
	    v->Selection.H = v->Selection.H * -1;
	if (v->mouse.mouse_mode == 4)
		v->Selection.Type = 1;
	else
	    v->Selection.Type = 2;
	v->Selection.Active = 1;
    }



}
void appmouse_right_drag(ViewInfo* v,int x,int y)
{
        
    appmouse_drag(v,x,y);

}
