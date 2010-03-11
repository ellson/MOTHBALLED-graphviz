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

#include "glcompui.h"
#include "glcompbutton.h"
#include "glcomppanel.h"
#include "glcomplabel.h"
#include "glcompimage.h"
#include "gltemplate.h"
#include "glutils.h"
#include "glmotion.h"
#include "topfisheyeview.h"
#include "toolboxcallbacks.h"
#include "viewportcamera.h"
#include "selectionfuncs.h"

#include "frmobjectui.h"


/* static glCompPanel *controlPanel; */
/* static glCompButton *rotatebutton; */
static glCompPanel *sel = NULL;
static glCompButton *to3DBtn;
static glCompButton *to2DBtn;
static glCompButton *toFisheye;
static glCompButton *toNormal;
static glCompImage *imgFisheye;
static glCompImage *img3D;
static glCompButton *panBtn;


static void menu_click_pan(glCompObj *obj, GLfloat x, GLfloat y,
			   glMouseButtonType t)
{
        deselect_all(view->g[view->activeGraph]);
}

#ifdef UNUSED
static void menu_click_zoom(void *obj, GLfloat x, GLfloat y,
			    glMouseButtonType t)
{
    switch_Mouse(NULL, MM_ZOOM);
}
#endif


static void menu_click_zoom_minus(glCompObj *obj, GLfloat x, GLfloat y,
				  glMouseButtonType t)
{
    glmotion_zoom_inc(0);
}

static void menu_click_zoom_plus(glCompObj *obj, GLfloat x, GLfloat y,
				 glMouseButtonType t)
{
    glmotion_zoom_inc(1);
}

static void menu_switch_to_fisheye(glCompObj *obj, GLfloat x, GLfloat y,
				   glMouseButtonType t)
{
    if (!view->Topview->fisheyeParams.active)
	{
	    if (!view->Topview->fisheyeParams.h) {
	    prepare_topological_fisheye(view->Topview);
	    g_timer_start(view->timer);
	}
	view->Topview->fisheyeParams.active = 1;
	glCompButtonShow(toNormal);
	glCompButtonHide(toFisheye);
	imgFisheye->common.visible = 1;


    } else {
	view->Topview->fisheyeParams.active = 0;
	g_timer_stop(view->timer);
	glCompButtonHide(toNormal);
	glCompButtonShow(toFisheye);
	imgFisheye->common.visible = 0;


    }
}



static void menu_click_center(glCompObj *obj, GLfloat x, GLfloat y,
			      glMouseButtonType t)
{
    if (view->active_camera == -1) {	/*2D mode */
	btnToolZoomFit_clicked(NULL, NULL);
    } else {			/*there is active camera , adjust it to look at the center */

	view->cameras[view->active_camera]->targetx = 0;
	view->cameras[view->active_camera]->targety = 0;
	view->cameras[view->active_camera]->r = 20;

    }
}
static void switch2D3D(glCompObj *obj, GLfloat x, GLfloat y,
		       glMouseButtonType t)
{
    if (t == glMouseLeftButton) {

	if (view->active_camera == -1) {

	    if (view->camera_count == 0) {
		menu_click_add_camera(obj);
	    } else {
		view->active_camera = 0;	/*set to camera */
	    }
	    glCompButtonShow(to2DBtn);
	    glCompButtonHide(to3DBtn);
	    img3D->common.visible = 1;
	} else {		/*switch to 2d */

	    view->active_camera = -1;	/*set to camera */
	    glCompButtonShow(to3DBtn);
	    glCompButtonHide(to2DBtn);
	    panBtn->common.callbacks.click((glCompObj*)panBtn, (GLfloat) 0,
					   (GLfloat) 0,
					   (glMouseButtonType) 0);
	    img3D->common.visible = 0;


	}
    }
}


void CBglCompMouseUp(glCompObj *obj, GLfloat x, GLfloat y, glMouseButtonType t)
{
    /* glCompMouse* m=&((glCompSet*)obj)->mouse; */
    sel->common.visible = 0;
    sel->common.pos.x = -5000;

}


void CBglCompMouseRightClick(glCompObj *obj, GLfloat x, GLfloat y,
			     glMouseButtonType t)
{
    if (t == glMouseRightButton) 
	{
		GLfloat X, Y, Z = 0;
		to3D((int) x, (int) y, &X, &Y, &Z);
    }
}

void attrList(glCompObj *obj, GLfloat x, GLfloat y, glMouseButtonType t)
{
	showAttrsWidget(view->Topview);
}




void glCompMouseMove(glCompObj *obj, GLfloat x, GLfloat y)
{
    glCompMouse *m = &((glCompSet *) obj)->mouse;

    sel->common.visible = 1;


    if ((m->down) && (m->t == glMouseRightButton)) 
    {
	sel->common.pos.x = m->pos.x - m->dragX;
	sel->common.pos.y = m->pos.y - m->dragY;
	sel->common.width = m->dragX;
	sel->common.height = m->dragY;
	glexpose();
    }
}
void selectedges(glCompObj *obj, GLfloat x, GLfloat y)
{
    if(view->Topview->sel.selectEdges==0)
	view->Topview->sel.selectEdges=1;
    else
	view->Topview->sel.selectEdges=0;

}
void selectnodes(glCompObj *obj, GLfloat x, GLfloat y)
{
    if(view->Topview->sel.selectNodes==0)
	view->Topview->sel.selectNodes=1;
    else
	view->Topview->sel.selectNodes=0;


}


glCompSet *glcreate_gl_topview_menu(void)
{
    /* static char* icondir[512]; */
    /* int ind=0; */
    GLfloat y = 5;
    GLfloat off = 43;
    glCompSet *s = glCompSetNew(view->w, view->h);
    glCompPanel *p = NULL;
    glCompButton *b = NULL;
    /* glCompLabel *l=NULL; */
    glCompImage *i = NULL;
    /* glCompLabel* l; */
    glCompColor c;
    s->common.callbacks.click = CBglCompMouseRightClick;

    p = glCompPanelNew((glCompObj *) s, 25, 25, 45, 47);
    p->common.align = glAlignLeft;
    p->common.data = 0;

    /*pan */
    b = glCompButtonNew((glCompObj *) p, 1, y, 42, 42, "");
    glCompButtonAddPngGlyph(b, smyrnaPath("pan.png"));
    b->common.callbacks.click = menu_click_pan;
    panBtn = b;

    y = y + off;

    /*switch to fisheye */
    b = glCompButtonNew((glCompObj *) p, 1, y, 42, 42, "");
    glCompButtonAddPngGlyph(b, smyrnaPath("fisheye.png"));
    b->common.callbacks.click = menu_switch_to_fisheye;
    toFisheye = b;


    /*switch to normal mode */
    b = glCompButtonNew((glCompObj *) p, 1, y, 42, 42, "");
    glCompButtonAddPngGlyph(b, smyrnaPath("no_fisheye.png"));
    b->common.callbacks.click = menu_switch_to_fisheye;
    b->common.visible = 0;
    toNormal = b;

    y=y+off;
    b = glCompButtonNew((glCompObj *) p, 1, y, 42, 42, "");
    glCompButtonAddPngGlyph(b, smyrnaPath("3D.png"));
    b->common.callbacks.click = switch2D3D;
    to3DBtn = b;


    b = glCompButtonNew((glCompObj *) p, 1, y, 42, 42, "");
    glCompButtonAddPngGlyph(b, smyrnaPath("2D.png"));
    b->common.callbacks.click = switch2D3D;
    glCompButtonHide(b);
    to2DBtn = b;

    y=y+off;
    b = glCompButtonNew((glCompObj *) p, 1, y, 42, 42, "N");
    b->common.callbacks.click = selectnodes;
    b->groupid=-1;
    b->status=1;

    y=y+off;
    b = glCompButtonNew((glCompObj *) p, 1, y, 42, 42, "E");
    b->common.callbacks.click = selectedges;
    b->groupid=-1;



    p = glCompPanelNew((glCompObj *) p, 1, 325, 45, 180);
    p->common.align = glAlignTop;
    p->common.data = 0;
    p->common.borderWidth = 1;
    p->shadowwidth = 0;

    c.R = 0.80;
    c.G = 0.6;
    c.B = 0.6;
    c.A = 1.6;

    y = 1;

    b = glCompButtonNew((glCompObj *) p, 1, y, 42, 42, "");
    glCompButtonAddPngGlyph(b, smyrnaPath("details.png"));
    b->common.callbacks.click = attrList;
    copy_glcomp_color(&c, &b->common.color);

	
    y = y + off;
	
	b = glCompButtonNew((glCompObj *) p, 1, y, 42, 42, "");
    glCompButtonAddPngGlyph(b, smyrnaPath("zoomin.png"));
    b->groupid = 0;
    b->common.callbacks.click = menu_click_zoom_plus;
    copy_glcomp_color(&c, &b->common.color);
    y = y + off;


    b = glCompButtonNew((glCompObj *) p, 1, y, 42, 42, "");
    glCompButtonAddPngGlyph(b, smyrnaPath("zoomout.png"));
    b->common.callbacks.click = menu_click_zoom_minus;
    copy_glcomp_color(&c, &b->common.color);

    y = y + off;


    b = glCompButtonNew((glCompObj *) p, 1, y, 42, 42, "");
    glCompButtonAddPngGlyph(b, smyrnaPath("center.png"));
    b->common.callbacks.click = menu_click_center;
    copy_glcomp_color(&c, &b->common.color);



	
	







    p = glCompPanelNew((glCompObj *) s, -250, 550, 150, 175);
    p->common.borderWidth = 0;
    p->shadowwidth = 0;
    p->common.color.R = 0;
    p->common.color.G = 0;
    p->common.color.B = 1;
    p->common.color.A = 0.2;
    p->common.visible = 0;
    sel = p;
    s->common.callbacks.mouseover = glCompMouseMove;
    s->common.callbacks.mouseup = CBglCompMouseUp;


    p = glCompPanelNew((glCompObj *) s, 25, 25, 52, 47);
    p->common.align = glAlignRight;
    p->common.data = 0;
    p->common.color.A = 0;




	
    p = glCompPanelNew((glCompObj *) p, 25, 0, 52, 110);
    p->common.align = glAlignTop;
    p->common.data = 0;
    p->common.color.A = 0;
    p->shadowwidth = 0;

    i = glCompImageNew((glCompObj *) p, 0, 0);
    glCompImageLoadPng(i, smyrnaPath("mod_fisheye.png"));
    imgFisheye = i;
    i->common.visible = 0;

    i = glCompImageNew((glCompObj *) p, 0, 52);
    glCompImageLoadPng(i, smyrnaPath("mod_3D.png"));
    img3D = i;
    i->common.visible = 0;
/*    l=glCompLabelNew((glCompObj *)s,100,5,view->Topview->Graphdata.GraphFileName);
    glCompLabelSetFontSize(l,24);*/
    return s;



}

int getIconsDirectory(char *bf)
{
#ifdef WIN32
    int a = GetCurrentDirectory(512, bf);
    if ((a > 512) || (a == 0))
	return 0;
#else
    //code *nix implementation to retrieve the icon directory, possibly some /share dir.
    /* FIXME */
#endif
    return 1;

}
