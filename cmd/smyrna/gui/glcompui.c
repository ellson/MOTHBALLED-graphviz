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


/* static glCompPanel *controlPanel; */
/* static glCompButton *rotatebutton; */
static glCompPanel* sel=NULL;
static glCompButton* to3DBtn;
static glCompButton* to2DBtn;
static glCompButton* rotateBtn;
static glCompButton* toFisheye;
static glCompButton* toNormal;
static glCompImage* imgFisheye;
static glCompButton* img3D;
static glCompButton* panBtn;


static void menu_click_pan(void* obj,GLfloat x,GLfloat y,glMouseButtonType t)
{
	switch_Mouse(NULL,MM_PAN);

}

#ifdef UNUSED
static void menu_click_zoom(void* obj,GLfloat x,GLfloat y,glMouseButtonType t)
{
	switch_Mouse(NULL,MM_ZOOM);
}
#endif

static void menu_click_fisheye_magnifier(void* obj,GLfloat x,GLfloat y,glMouseButtonType t)
{
    view->mouse.mouse_mode = MM_FISHEYE_MAGNIFIER;
}

static void menu_click_zoom_minus(void* obj,GLfloat x,GLfloat y,glMouseButtonType t)
{
	glmotion_zoom_inc(0);
}

static void menu_click_zoom_plus(void* obj,GLfloat x,GLfloat y,glMouseButtonType t)
{
	glmotion_zoom_inc(1);
}


static void menu_switch_to_fisheye(void* obj,GLfloat x,GLfloat y,glMouseButtonType t)
{
	if (!view->Topview->is_top_fisheye == 1)
	{
		if (!view->Topview->h) 
		{
			prepare_topological_fisheye(view->Topview);
			g_timer_start(view->timer);
		}
		view->Topview->is_top_fisheye = 1;
			glCompButtonShow(toNormal);
			glCompButtonHide(toFisheye);
			imgFisheye->common.visible=1;


	}
	else
	{
		view->Topview->is_top_fisheye = 0;
		g_timer_stop(view->timer);
			glCompButtonHide(toNormal);
			glCompButtonShow(toFisheye);
			imgFisheye->common.visible=0;


	}
}

static void menu_click_rotate(void* obj,GLfloat x,GLfloat y,glMouseButtonType t)
{
	switch_Mouse(NULL,MM_ROTATE);
	view->mouse.mouse_mode = MM_ROTATE;
}


static void menu_click_center(void* obj,GLfloat x,GLfloat y,glMouseButtonType t)
{
	if (view->active_camera == -1)	/*2D mode*/
	{	
		btnToolZoomFit_clicked(NULL,NULL);
	}
	else	/*there is active camera , adjust it to look at the center*/
	{
		view->cameras[view->active_camera]->targetx=0;
		view->cameras[view->active_camera]->targety=0;
		view->cameras[view->active_camera]->r=20;

	}
}
static void switch2D3D(void* obj,GLfloat x,GLfloat y,glMouseButtonType t)
{
	if(t==glMouseLeftButton)
	{

		if (view->active_camera == -1)
		{

			if (view->camera_count == 0)
			{
				menu_click_add_camera(obj);
			}
			else
			{
				view->active_camera = 0 ;	/*set to camera*/
			}
			glCompButtonShow(to2DBtn);
			glCompButtonHide(to3DBtn);
			glCompButtonShow(rotateBtn);
			img3D->common.visible=1;
		}
		else /*switch to 2d*/
		{
				view->active_camera = -1 ;	/*set to camera*/
			glCompButtonShow(to3DBtn);
			glCompButtonHide(to2DBtn);
			glCompButtonHide(rotateBtn);
			panBtn->common.callbacks.click(panBtn,(GLfloat)0,(GLfloat)0,(glMouseButtonType)0);
			img3D->common.visible=0;


		}
	}
}


void CBglCompMouseUp (void* obj,GLfloat x,GLfloat y,glMouseButtonType t)
{
	/* glCompMouse* m=&((glCompSet*)obj)->mouse; */
	sel->common.visible=0;
	sel->common.pos.x=-5000;

}

void CBglCompMouseRightClick (void* obj,GLfloat x,GLfloat y,glMouseButtonType t)
{
	if(t==glMouseRightButton)
	{
		GLfloat X,Y,Z=0;
		to3D((int)x,(int)y,&X,&Y,&Z);
	}
}




void glCompMouseMove (void* obj,GLfloat x,GLfloat y)
{
	glCompMouse* m=&((glCompSet*)obj)->mouse;

	sel->common.visible=1;


	if((m->down) &&(m->t==glMouseRightButton))
	{
		sel->common.pos.x=m->pos.x-m->dragX;
		sel->common.pos.y=m->pos.y-m->dragY;
		sel->common.width=m->dragX;
		sel->common.height=m->dragY;
		glexpose();
	}
}


glCompSet *glcreate_gl_topview_menu(void)
{
	/* static char* icondir[512]; */
	/* int ind=0; */
	GLfloat y=5;
	GLfloat off=43;
	glCompSet *s = glCompSetNew(view->w,view->h);
	glCompPanel *p=NULL;
    glCompButton *b=NULL;
    /* glCompLabel *l=NULL; */
	glCompImage *i=NULL;
	glCompColor c;
	s->common.callbacks.click=CBglCompMouseRightClick;

	p = glCompPanelNew((glCompObj*)s,25, 25, 53, 47);
	p->common.align=glAlignLeft;
    p->common.data = 0;

	/*pan*/
	b=glCompButtonNew((glCompObj*)p,1,y,42,42,"");
	glCompButtonAddPngGlyph(b,smyrnaPath("pan.png"));
	b->common.callbacks.click=menu_click_pan;
	panBtn=b;

	b->groupid=1;
	y=y+off;

	b=glCompButtonNew((glCompObj*)p,1,y,42,42,"");
	glCompButtonAddPngGlyph(b,smyrnaPath("magnifier.png"));
	b->common.callbacks.click=menu_click_fisheye_magnifier;
	b->groupid=1;

	y=y+off;

	b=glCompButtonNew((glCompObj*)p,1,y,42,42,"");
	glCompButtonAddPngGlyph(b,smyrnaPath("rotate.png"));
	b->groupid=1;
	b->common.callbacks.click=menu_click_rotate;
	glCompButtonHide(b);
	rotateBtn=b;

	
	p = glCompPanelNew((glCompObj*)p,1, 25, 80, 225);
	p->common.align=glAlignTop;
    p->common.data = 0;
	p->common.borderWidth=1;
	p->shadowwidth=0;

	c.R=0.80;
	c.G=0.6;
	c.B=0.6;
	c.A=1.6;
	
	y=1;
	b=glCompButtonNew((glCompObj*)p,1,y,42,42,"");
	glCompButtonAddPngGlyph(b,smyrnaPath("zoomin.png"));
	b->groupid=0;
	b->common.callbacks.click=menu_click_zoom_plus;

	copy_glcomp_color(&c,&b->common.color);
	y=y+off;


	b=glCompButtonNew((glCompObj*)p,1,y,42,42,"");
	glCompButtonAddPngGlyph(b,smyrnaPath("zoomout.png"));
	b->common.callbacks.click=menu_click_zoom_minus;

	y=y+off;
	copy_glcomp_color(&c,&b->common.color);

	b=glCompButtonNew((glCompObj*)p,1,y,42,42,"");
	glCompButtonAddPngGlyph(b,smyrnaPath("fit.png"));
	copy_glcomp_color(&c,&b->common.color);

	y=y+off;


	b=glCompButtonNew((glCompObj*)p,1,y,42,42,"");
	glCompButtonAddPngGlyph(b,smyrnaPath("center.png"));
	b->common.callbacks.click=menu_click_center;
	copy_glcomp_color(&c,&b->common.color);


	y=y+off;


	b=glCompButtonNew((glCompObj*)p,1,y,42,42,"");
	glCompButtonAddPngGlyph(b,smyrnaPath("3D.png"));
	b->common.callbacks.click=switch2D3D;
	copy_glcomp_color(&c,&b->common.color);
	to3DBtn=b;


	b=glCompButtonNew((glCompObj*)p,1,y,42,42,"");
	glCompButtonAddPngGlyph(b,smyrnaPath("2D.png"));
	b->common.callbacks.click=switch2D3D;
	glCompButtonHide(b);
	copy_glcomp_color(&c,&b->common.color);
	to2DBtn=b;





	p = glCompPanelNew((glCompObj*)s,-250, 550, 150, 175);
	p->common.borderWidth=0;
	p->shadowwidth=0;
	p->common.color.R=0;
	p->common.color.G=0;
	p->common.color.B=1;
	p->common.color.A=0.2;
	p->common.visible=0;
	sel=p;

	s->common.callbacks.mouseover=glCompMouseMove;
	s->common.callbacks.mouseup=CBglCompMouseUp;


	p = glCompPanelNew((glCompObj*)s,25, 25, 52, 47);
	p->common.align=glAlignRight;
    p->common.data = 0;
	p->common.color.A=0;

	/*switch to fisheye*/
	b=glCompButtonNew((glCompObj*)p,2,1,42,42,"");
	glCompButtonAddPngGlyph(b,smyrnaPath("fisheye.png"));
	b->common.callbacks.click=menu_switch_to_fisheye;
	toFisheye=b;


	/*switch to normal mode*/
	b=glCompButtonNew((glCompObj*)p,2,1,42,42,"");
	glCompButtonAddPngGlyph(b,smyrnaPath("no_fisheye.png"));
	b->common.callbacks.click=menu_switch_to_fisheye;
	b->common.visible=0;
	toNormal=b;


	p = glCompPanelNew((glCompObj*)p,25, 0, 52, 110);
	p->common.align=glAlignTop;
    p->common.data = 0;
	p->common.color.A=0;
	p->shadowwidth=0;

	i=glCompImageNew((glCompObj*)p,0,0);
	glCompImageLoadPng(i,smyrnaPath("mod_fisheye.png"));
	imgFisheye=i;
	i->common.visible=0;

	i=glCompImageNew((glCompObj*)p,0,52);
	glCompImageLoadPng(i,smyrnaPath("mod_3D.png"));
	img3D=i;
	i->common.visible=0;
	return s;
}

int getIconsDirectory(char* bf)
{
#ifdef WIN32
	int a=GetCurrentDirectory(512,bf);
	if ((a > 512) || (a==0))
		return 0;
#else
	//code *nix implementation to retrieve the icon directory, possibly some /share dir.
    /* FIXME */
#endif
	return 1;

}
