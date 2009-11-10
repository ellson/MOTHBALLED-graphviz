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
#include "hotkeymap.h"
#include "selection.h"


    static float prevX=0;
    static float prevY=0;
static void apply_actions(ViewInfo* v,int x,int y)
{
    int a;
    a=get_mode(v);
    if (a==MM_ROTATE)

    {
	view->arcball->MousePt.s.X = (GLfloat) x;
	view->arcball->MousePt.s.Y = (GLfloat) y;
	if (!view->arcball->isDragging) {
	    arcmouseClick(view);
        view->arcball->isDragging = 1;
	} else 
		    arcmouseDrag(view);
	return;
    }
    if (a==MM_PAN)
    {
	glmotion_pan(v);
    }
    if (a==MM_MOVE)
        move_TVnodes();

    if(a==MM_RECTANGULAR_SELECT)
    {
	if (!view->mouse.down)
	    rectangle_select(v);
    }

    if (a==MM_SINGLE_SELECT) 
        pick_node_from_coords(view->mouse.GLfinalPos.x,view->mouse.GLfinalPos.y,view->mouse.GLfinalPos.z);

    if (a==MM_FISHEYE_PICK) 
    {

	if (view->activeGraph >= 0) 
	{
	    if (view->Topview->is_top_fisheye) 
		changetopfishfocus(view->Topview,&view->mouse.GLpos.x,&view->mouse.GLpos.y,  0, 1);
	    else    /*single right click*/
		/*	pick_node_from_coords(view->mouse.GLpos.x, view->mouse.GLpos.y,view->mouse.GLpos.z);*/

	    {;}


	}
    }




}

static int singleclick(ViewInfo* v)
{
       return(((int)v->mouse.initPos.x == (int)v->mouse.finalPos.x) && ((int)v->mouse.initPos.y == (int)v->mouse.finalPos.y));

}
static void appmouse_left_click(ViewInfo* v,int x,int y)
{

}
static void appmouse_right_click(ViewInfo* v,int x,int y)
{
}
static void appmouse_down(ViewInfo* v,int x,int y)
{
    view->mouse.dragX = 0;
    view->mouse.dragY = 0;
    v->mouse.down=1;
    v->mouse.initPos.x=x;
    v->mouse.initPos.y=y;
    v->mouse.pos.x=x;
    v->mouse.pos.y=y;

    GetFixedOGLPos((float) x,y, v->GLDepth,&v->mouse.GLinitPos.x,&v->mouse.GLinitPos.y,&v->mouse.GLinitPos.z);
    GetFixedOGLPos((float) x,y,v->GLDepth, &v->mouse.GLpos.x,&v->mouse.GLpos.y,&v->mouse.GLpos.z);

    prevX=0;
    prevY=0;

    view->Selection.X = view->mouse.GLpos.x;
    view->Selection.Y = view->mouse.GLpos.y;
}
static void appmouse_up(ViewInfo* v,int x,int y)
{

    int a;
    v->mouse.down=0;
    v->mouse.finalPos.x=x;
    v->mouse.finalPos.y=y;
    a=get_mode(v);
    GetFixedOGLPos((float) x,y,v->GLDepth, &v->mouse.GLfinalPos.x,&v->mouse.GLfinalPos.y,&v->mouse.GLfinalPos.z);
    if(singleclick(v))
    {
	if (v->mouse.t==glMouseLeftButton)
	    appmouse_left_click(v,x,y);
	if (v->mouse.t==glMouseRightButton)
	    appmouse_right_click(v,x,y);
    }
    if ((a== MM_FISHEYE_MAGNIFIER) || (a == MM_MAGNIFIER))	//fisheye mag mouse release, stop distortion
        originate_distorded_coordinates(v->Topview);
    apply_actions(v,x,y);
    view->mouse.dragX = 0;
    view->mouse.dragY = 0;

}
static void appmouse_drag(ViewInfo* v,int x,int y)
{
    static float x2,y2;
    v->mouse.pos.x=x;
    v->mouse.pos.y=y;
    GetFixedOGLPos((float) x,y,v->GLDepth, &v->mouse.GLpos.x,&v->mouse.GLpos.y,&v->mouse.GLpos.z);
    x2=v->mouse.GLpos.x;
    y2=v->mouse.GLpos.y;
    prevX=x2;
    prevY=y2;
    apply_actions(v,x,y);
}

void appmouse_left_click_down(ViewInfo* v,int x,int y)
{
       v->mouse.t=glMouseLeftButton;
       appmouse_down(v,x,y);


}
void appmouse_left_click_up(ViewInfo* v,int x,int y)
{
	int a=get_mode(v);
    appmouse_up(v,x,y);
/*	if (v->mouse.mouse_mode == MM_MOVE)
	    move_TVnodes();*/
}
void appmouse_left_drag(ViewInfo* v,int x,int y)
{
    appmouse_drag(v,x,y);



}
void appmouse_right_click_down(ViewInfo* v,int x,int y)
{
    v->mouse.t=glMouseRightButton;
    appmouse_down(v,x,y);


}
void appmouse_right_click_up(ViewInfo* v,int x,int y)
{
    appmouse_up(v,x,y);
 
}
void appmouse_right_drag(ViewInfo* v,int x,int y)
{
        
    appmouse_drag(v,x,y);

}


void appmouse_middle_click_down(ViewInfo* v,int x,int y)
{
    v->mouse.t=glMouseMiddleButton;
    appmouse_down(v,x,y);


}
void appmouse_middle_click_up(ViewInfo* v,int x,int y)
{
    appmouse_up(v,x,y);
 
}
void appmouse_middle_drag(ViewInfo* v,int x,int y)
{
        
    appmouse_drag(v,x,y);

}

