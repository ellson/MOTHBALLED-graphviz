#include "viewportcamera.h"
static viewport_camera* new_viewport_camera(ViewInfo * view)
{
	viewport_camera* c=malloc(sizeof(viewport_camera));
	return c;
}

viewport_camera* add_camera_to_viewport(ViewInfo * view)
{
	view->camera_count++;
	view->cameras=(viewport_camera**)realloc(view->cameras,sizeof(viewport_camera *)*view->camera_count);
	view->cameras[view->camera_count-1]=new_viewport_camera(view);
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
	if (cam_index < view->camera_count)
	{
		view->active_camera=cam_index;
		return 1;
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

void menu_click_add_camera(void *p)
{

}

void menu_click_2d (void *p)
{
	view->active_camera=-1;
}

void menu_click_camera_select(void *p)
{
	view->active_camera=((glCompButton*)p)->data;
}


void menu_click_camera_remove(void *p)
{

}
void menu_click_camera_edit(void *p)
{

}


void attach_camera_widget(ViewInfo * view)
{

#define PANEL_PADDING			5
#define	CAMERA_BUTTON_HEIGHT	25
#define	CAMERA_BUTTON_WIDTH		75 


	int ind,ind2,x,y;
	char* buf[256];
	glCompPanel *p;
    glCompButton *b;
    glCompLabel *l;	
    glCompSet *s = view->Topview->topviewmenu;
	viewport_camera*	c;
	int p_height;
	/*first we need to get rid of the old menu*/
	for (ind=0;ind < s->panelcount; ind ++)
	{
		if (s->panels[ind]->data==3)
		{
			/*remove buttons in the panel*/
			for (ind2=0;ind < s->buttoncount;ind2++)
			{
				if(s->buttons[ind2]->panel==s->panels[ind])
					glCompSetRemoveButton(s,s->buttons[ind2]);
			}
			/*remove panel itself*/
			glCompSetRemovePanel(s,s->panels[ind]);
		}
	}



	/*add test cameras*/
	c=add_camera_to_viewport(view);
	c->targetx=view->panx;c->targety=view->pany;c->targetz=0;
	c->x=100;c->y=+200;c->z=view->zoom;
	c=add_camera_to_viewport(view);
	c->targetx=view->panx;c->targety=view->pany;c->targetz=0;
	c->x=-300;c->y=+200;c->z=view->zoom;
	c=add_camera_to_viewport(view);
	c->targetx=view->panx;c->targety=view->pany;c->targetz=0;
	c->x=-100;c->y=-200;c->z=view->zoom;

	/*calculate height of panel*/
	p_height=2*PANEL_PADDING+view->camera_count*(CAMERA_BUTTON_HEIGHT+PANEL_PADDING)+CAMERA_BUTTON_HEIGHT;


	/*container for camera buttons*/
	p = glCompPanelNew((GLfloat)25, (GLfloat)75, (GLfloat)4*PANEL_PADDING+3*CAMERA_BUTTON_WIDTH,(GLfloat)p_height);
    p->data = 3;
    glCompSetAddPanel(s, p);

    b = glCompButtonNew((GLfloat)PANEL_PADDING,(GLfloat)PANEL_PADDING, (GLfloat)CAMERA_BUTTON_WIDTH, (GLfloat)CAMERA_BUTTON_HEIGHT, "ADD", '\0', 0, 0);
    b->panel = p;
    b->groupid = 0;
    b->customptr = view;
    glCompSetAddButton(s, b);
    b->callbackfunc = menu_click_add_camera;

    b = glCompButtonNew((GLfloat)PANEL_PADDING*2+(GLfloat)CAMERA_BUTTON_WIDTH,(GLfloat)PANEL_PADDING, (GLfloat)CAMERA_BUTTON_WIDTH, (GLfloat)CAMERA_BUTTON_HEIGHT, "2D", '\0', 0, 0);
    b->panel = p;
    b->groupid = 4;	//4 is assigned to all camera buttons 
    b->customptr = view;
    glCompSetAddButton(s, b);
    b->callbackfunc = menu_click_2d;

	for (ind=0;ind < view->camera_count;ind ++)
	{
		y=p->height-((GLfloat)PANEL_PADDING + ind*((GLfloat)CAMERA_BUTTON_HEIGHT+(GLfloat)PANEL_PADDING))-CAMERA_BUTTON_HEIGHT;
		x=PANEL_PADDING;
		sprintf(buf,"CAM%i",ind+1);
		b = glCompButtonNew(x,y, (GLfloat)CAMERA_BUTTON_WIDTH, (GLfloat)CAMERA_BUTTON_HEIGHT, buf, '\0', 0, 0);
		b->panel = p;
		b->groupid = 4;	//4 is assigned to all camera buttons 
		b->data=ind;	//assign camera id to custom data to use single call back
		b->customptr = view;
		glCompSetAddButton(s, b);
		b->callbackfunc = menu_click_camera_select;

		x=PANEL_PADDING*2+CAMERA_BUTTON_WIDTH;
		b = glCompButtonNew(x,y, (GLfloat)CAMERA_BUTTON_WIDTH, (GLfloat)CAMERA_BUTTON_HEIGHT, "Remove", '\0', 0, 0);
		b->panel = p;
		b->groupid = 0;	
		b->data=ind;	//assign camera id to custom data to use single call back
		b->customptr = view;
		glCompSetAddButton(s, b);
		b->callbackfunc = menu_click_camera_remove;

		x=PANEL_PADDING*3+CAMERA_BUTTON_WIDTH*2;
		b = glCompButtonNew((GLfloat)x,(GLfloat)y, (GLfloat)CAMERA_BUTTON_WIDTH, (GLfloat)CAMERA_BUTTON_HEIGHT, "Edit", '\0', 0, 0);
		b->panel = p;
		b->groupid = 0;	
		b->data=ind;	//assign camera id to custom data to use single call back
		b->customptr = view;
		glCompSetAddButton(s, b);
		b->callbackfunc = menu_click_camera_edit;
	}







}

