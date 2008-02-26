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
#include "glTexFont.h"
#include "glTexFontTGA.h"
#include "glTexFontDefs.h"
#include "glTexFontInclude.h"
#include "topview.h"
#include "math.h"
#include "btree.h"
#include "viewport.h"
#include "draw.h"
#include "selection.h"

static float dx=0.0;
static float dy=0.0;
void cleartopview(topview* t)
{
	int ind=0;
	/*clear nodes*/
	free (t->Nodes);
	/*clear edges*/
	free(t->Edges);
	/*clear gl menu*/
	glCompSetClear(t->topviewmenu);
	free (t);
}
void preparetopview(Agraph_t *g,topview* t)
{
	int GP_flag=0;	//general purpose flag
	char* str;
	char* d_attr1;
	char* d_attr2;
	float a,b,c;
	char* pch;
	Agnode_t *v;
	Agedge_t *e;
	GtkTreeIter iter;
	int ind,ind2,data_type_count;	//number of columns for custom view->Topview data ,IP ,HOST, etc
	char buf[256];
	ind=0;ind2=0;
	gtk_widget_hide(glade_xml_get_widget(xml, "layout6"));	//hide top panel
	gtk_widget_hide(glade_xml_get_widget(xml, "menubar1"));	//hide menu
	data_type_count=0;
	d_attr1=agget(g, "DataAttribute1");
	d_attr2=agget(g, "DataAttribute2");
	
	t->Edges=NULL;
	t->Nodes=NULL;
	/*initialize node and edge array*/
	printf ("edge memory required %i \n",sizeof(topview_edge)*agnedges(g));
	t->Edges=malloc(sizeof(topview_edge)*agnedges(g));
	if (t->Edges)
		printf ("Edge malloc is ok\n");

	t->Nodes=malloc(sizeof(topview_node)*agnnodes(g));
	if (t->Nodes)
		printf ("Nodes malloc is ok\n");

	printf ("# of edges :%i\n",agnnodes(g));
	printf ("# of edges :%i\n",agnedges(g));

		/*malloc topviewdata*/
	t->TopviewData=malloc (sizeof(topviewdata));

	for (v = agfstnode(g); v; v = agnxtnode(g, v))
	{
		//set node TV reference
    	((custom_object_data*)AGDATA(v))->TVRef=ind;		//view->Topview reference
		strcpy(buf,agget(v, "pos"));
		if(strlen(buf))
		{
			a=atof(strtok (buf,"," ));
			b=atof(strtok (NULL,"," ));
			str=strtok (NULL,"," );
			if(str)
				c=atof(str);
			else
				c=0.0;
		}
		/*initialize group index, -1 means no group*/		
		t->Nodes[ind].GroupIndex=-1;
		randomize_color(&(t->Nodes[ind].Color),2);
		t->Nodes[ind].Node=v;
		t->Nodes[ind].x=a;
		t->Nodes[ind].y=b;
		t->Nodes[ind].z=c;
		t->Nodes[ind].distorted_x=a;
		t->Nodes[ind].distorted_y=b;
		t->Nodes[ind].zoom_factor=1;
		t->Nodes[ind].degree=agdegree(g,v,1,1);
		t->Nodes[ind].node_alpha=log((float)t->Nodes[ind].degree+0.3);
		if(d_attr1)
		{
			str=agget(v,d_attr1);
			if(str)
			{
				t->Nodes[ind].Label=strdup(str);

			}

		}
		if(d_attr2)
		{
			str=agget(v,d_attr2);
			if(str)
			{
				t->Nodes[ind].Label2=strdup(str);
			}
		}
		for (e = agfstout(g,v) ; e ; e = agnxtout (g,e))
		{
			aghead(e);
			t->Edges[ind2].Hnode=aghead(e);
			t->Edges[ind2].Tnode=agtail(e);
			t->Edges[ind2].Edge=e;
			strcpy(buf,agget(aghead(e), "pos"));
			if(strlen(buf))
			{
				a=atof(strtok (buf,"," ));
				b=atof(strtok (NULL,"," ));
				str=strtok (NULL,"," );
				if(str)
					c=atof(str);
				else
					c=0.0;

				t->Edges[ind2].x1=a;
				t->Edges[ind2].y1=b;
				t->Edges[ind2].z1=b;
			}
			strcpy(buf,agget(agtail(e), "pos"));
			if(strlen(buf))
			{
				a=atof(strtok (buf,"," ));
				b=atof(strtok (NULL,"," ));
				str=strtok (NULL,"," );
				if(str)
					c=atof(str);
				else
					c=0.0;
				t->Edges[ind2].x2=a;
				t->Edges[ind2].y2=b;
				t->Edges[ind2].z2=c;
			}
			ind2++;
		}
		ind++;
	}
	//attach edge node references	loop one more time
	ind=0;ind2=0;
	for (v = agfstnode(g); v; v = agnxtnode(g, v))
	{
		//set node TV reference
		for (e = agfstout(g,v) ; e ; e = agnxtout (g,e))
		{
			t->Edges[ind2].Node1=  &t->Nodes[((custom_object_data*)AGDATA(t->Edges[ind2].Tnode))->TVRef];
			t->Edges[ind2].Node2=  &t->Nodes[((custom_object_data*)AGDATA(t->Edges[ind2].Hnode))->TVRef];
			ind2++;

		}
		ind++;
	}
	t->Nodecount=ind;
	t->Edgecount=ind2;
	view->fmg.fisheye_distortion_fac=5; //need to be hooked to a widget
	set_boundries(t);
	set_update_required(t);
	t->topviewmenu=glcreate_gl_topview_menu();
	load_host_buttons(t,g,t->topviewmenu);



}
void drawTopViewGraph(Agraph_t *g)
{
	RGBColor c;
//	DWORD t1,t2;
	topview_node *v;
	topview_edge *e;
	float ddx,ddy;
	float dddx,dddy;
	int ind=0;
	char buf[50];
	GLfloat a,b;
	if(view->zoom > NODE_ZOOM_LIMIT)
	{
		glPointSize(15/view->zoom*-1);
		//draw nodes
		set_topview_options();
		if(view->zoom < NODE_CIRCLE_LIMIT)
			glBegin(GL_POINTS);

		//drawing labels
		for (ind=0;ind < view->Topview->Nodecount;ind ++)
		{

			v=&view->Topview->Nodes[ind];
			if(!node_visible(v->Node))
				break;

			draw_topview_label(v,1);

		}
		for (ind=0;ind < view->Topview->Nodecount;ind ++)
		{

			if((view->Topview->Nodes[ind].x > view->clipX1) && (view->Topview->Nodes[ind].x < view->clipX2) &&(view->Topview->Nodes[ind].y > view->clipY1)&&(view->Topview->Nodes[ind].y < view->clipY2))
			{
				float zdepth;

				if(1)
				{
					v=&view->Topview->Nodes[ind];
					if(!node_visible(v->Node))
						break;

					select_topview_node(v);
					//UPDATE view->Topview data from cgraph
					if (v->update_required)
						update_topview_node_from_cgraph(v);
					if( ((custom_object_data*)AGDATA(v->Node))->Selected==1)
					{
						glColor4f(view->selectColor.R,view->selectColor.G,view->selectColor.B,view->selectColor.A);
						ddx=dx;ddy=dy;
					}
					else		//get the color from node
					{
						glColor4f(v->Color.R,v->Color.G,v->Color.B,v->node_alpha);
		//					glColor4f(1,0,0,v->node_alpha);
		//				glColor4f (log((double)v->degree+0.5),v->Color.G,v->Color.B,);
						ddx=0;ddy=0;
					}

					if (v->distorted_x!=v->x)
						zdepth=Z_FORWARD_PLANE;
					else
						zdepth=Z_BACK_PLANE;
					if(view->zoom < NODE_CIRCLE_LIMIT)
						glVertex3f(v->distorted_x-ddx,v->distorted_y-ddy,zdepth); 
					else
						drawCircle(v->distorted_x-ddx,v->distorted_y-ddy,v->node_alpha*v->zoom_factor,zdepth);
				}
			}
		}
		if(view->zoom< NODE_CIRCLE_LIMIT)
			glEnd();

		
	}
	//draw edges
//	glLineWidth(5/view->zoom*-1);
	glBegin(GL_LINES);
	set_topview_options();
	for (ind=0;ind < view->Topview->Edgecount;ind ++)
	{
		if(
			((view->Topview->Edges[ind].x1 > view->clipX1) && (view->Topview->Edges[ind].x1 < view->clipX2) &&(view->Topview->Edges[ind].y1 > view->clipY1)&&(view->Topview->Edges[ind].y1 < view->clipY2))
							||
			((view->Topview->Edges[ind].x2 > view->clipX1) && (view->Topview->Edges[ind].x2 < view->clipX2) &&(view->Topview->Edges[ind].y2 > view->clipY1)&&(view->Topview->Edges[ind].y2 < view->clipY2))

			)
		{
			float zdepth1,zdepth2;
			e=&view->Topview->Edges[ind];
			select_topview_edge(e);
			if( ((custom_object_data*)AGDATA(e->Node1->Node))->Selected==1)	//tail is selected
			{		 ddx=dx;ddy=dy;}
			else
			{		ddx=0;ddy=0;}
			if( ((custom_object_data*)AGDATA(e->Node2->Node))->Selected==1)	//head
			{		 dddx=dx;dddy=dy;}
			else
			{		dddx=0;dddy=0;}

			//zdepth
			if (e->Node1->distorted_x!=e->Node1->x)
						zdepth1=Z_FORWARD_PLANE;
					else
						zdepth1=Z_BACK_PLANE;
			if (e->Node2->distorted_x!=e->Node2->x)
						zdepth2=Z_FORWARD_PLANE;
					else
						zdepth2=Z_BACK_PLANE;


			if(get_color_from_edge(e))
			{
				glVertex3f(e->Node1->distorted_x-ddx,e->Node1->distorted_y-ddy,zdepth1);
				glVertex3f(e->Node2->distorted_x-dddx,e->Node2->distorted_y-dddy,zdepth2);
			}
		}
	}
	glEnd();
	if((view->Selection.Active>0) && (!view->SignalBlock))
	{
			view->Selection.Active=0;
			drawTopViewGraph(g);
			view->SignalBlock=1;
			expose_event (view->drawing_area,NULL,NULL);
			view->SignalBlock=0;
	}

}


int select_topview_node(topview_node *n)
{
	if(!view->Selection.Active)
		return 0;
	if(is_point_in_rectangle(n->x,n->y,view->Selection.X,view->Selection.Y,view->Selection.W,view->Selection.H))
	{
			
		switch (view->Selection.Type)
		{
		case 0:

			if ( ((custom_object_data*)AGDATA(n->Node))->Selected==0)
			{
				((custom_object_data*)AGDATA(n->Node))->Selected=1;
				select_object (view->g[view->activeGraph],n->Node);
			}
			else
			{
				((custom_object_data*)AGDATA(n->Node))->Selected=1;
				deselect_object (view->g[view->activeGraph],n->Node);
			}
			break;

		case 1:
		case 2:
			if(view->Selection.Anti==0)
			{
				select_object (view->g[view->activeGraph],n->Node);
				view->Selection.AlreadySelected=1;
			}
			else
			{
				deselect_object (view->g[view->activeGraph],n->Node);
				view->Selection.AlreadySelected=1;
			}
			break;

		}
	}
}
int select_topview_edge(topview_edge *e)
{
	int r=0;
	if(!view->Selection.Active)
		return 0;
	r=(lineintersects(e->x1,e->y1,e->x2,e->y2));
	if(r >= 0)
	{
			
		switch (view->Selection.Type)
		{
		case 0:
			if ( ((custom_object_data*)AGDATA(e->Edge))->Selected==0)
			{
				((custom_object_data*)AGDATA(e->Edge))->Selected=1;
				select_object (view->g[view->activeGraph],e->Edge);
			}
			else
			{
				((custom_object_data*)AGDATA(e->Edge))->Selected=1;
				deselect_object (view->g[view->activeGraph],e->Edge);
			}
			break;

/*		case 1:
		case 2:
			if(view->Selection.Anti==0)
			{
				select_object (view->g[view->activeGraph],n->Node);
				view->Selection.AlreadySelected=1;
			}
			else
			{
				deselect_object (view->g[view->activeGraph],n->Node);
				view->Selection.AlreadySelected=1;
			}
			break;*/

		}
	}


/*	else if (view->Selection.Type==0)
	{
			deselect_object (view->g[view->activeGraph],n->Node);
			view->Selection.AlreadySelected=1;
	} */
}
int update_topview_node_from_cgraph(topview_node* Node)
{
	//for now just color, maybe i need more later
	int i=0;
	char* buf;
	buf=agget(Node->Node,"color");
	if(buf)
		Node->Color=GetRGBColor(buf);
/*	else
	{
		randomize_color(&(Node->Color),2);

		Node->Color.R=view->penColor.R;
		Node->Color.G=view->penColor.G;
		Node->Color.B=view->penColor.B;
		Node->Color.A=view->penColor.A;
	}*/
	Node->update_required=0;
}
int update_topview_edge_from_cgraph(topview_edge* Edge)
{
	//for now just color , maybe i need more later
	int i=0;
	char buf[124];
	strcpy(buf, (agget(Edge->Edge,"color")==NULL) ? "black" : agget(Edge->Edge,"color"));
	if(strlen(buf) > 0)
		Edge->Color=GetRGBColor(buf);
	else
	{
		Edge->Color.R=view->penColor.R;
		Edge->Color.G=view->penColor.G;
		Edge->Color.B=view->penColor.B;
		Edge->Color.A=view->penColor.A;
	}
	Edge->update_required=0;
}



int set_update_required(topview* t)
{
	int i=0;
	char buf[124];
	int ilimit;
	ilimit=	(t->Nodecount > t->Edgecount) ? t->Nodecount : t->Edgecount;

	for (i=0; i < ilimit; i ++)
	{
		if( t->Nodecount > i)

			t->Nodes[i].update_required=1;
		if( t->Edgecount > i)
			t->Edges[i].update_required=1;
	}

}

int draw_topview_label(topview_node* v,float zdepth)
{

	float fs=0;
	float ddx=0;
	float ddy=0;
	if(!v->Label)
		return 0;
	if((view->zoom*-1/v->degree/v->zoom_factor) > 2)
		return 0;
	if((v->distorted_x > view->clipX1) && (v->distorted_x < view->clipX2) &&(v->distorted_y > view->clipY1)&&(v->distorted_y < view->clipY2))
	{

		fs=(v->degree==1) ?log((double)v->degree+1)*7:log((double)v->degree+0.5)*7;
		fs=fs*v->zoom_factor;
/*		if(fs > 12)
			fs=24;*/
	 	if( ((custom_object_data*)AGDATA(v->Node))->Selected==1)
		{	
			ddx=dx;ddy=dy;
		}

		fontSize (fs);
		if ((log((float)v->degree)*-0.6*view->zoom) >0)
			fontColorA (log((double)v->degree+1),view->penColor.G,view->penColor.B,view->penColor.A/log((float)v->degree)*-0.6*view->zoom);
		else
			fontColorA (log((double)v->degree+1),view->penColor.G,view->penColor.B,1);

//		printf("%f \n",view->penColor.A/log((float)v->degree)*-0.02*view->zoom);
		fontDrawString (v->distorted_x-ddx,v->distorted_y-ddy,v->Label,fs*5);

		return 1;
	}
	else
		return 0;
}



void set_topview_options()
{
	
	if ((view->mouse.mouse_mode==10) && (view->mouse.mouse_down==1))	//selected, if there is move move it, experimental
	{
		dx=view->GLx-view->GLx2;
		dy=view->GLy-view->GLy2;
	}
	else
	{
		dx=0;
		dy=0;
	}

}
/*refreshes limits of the graph call it when node locations are changed*/
void set_boundries(topview* t)
{

	int ind=0;
	float left,right,top,bottom;
	left=t->Nodes[0].distorted_x;
	right=t->Nodes[0].distorted_x;
	top=t->Nodes[0].distorted_y;
	bottom=t->Nodes[0].distorted_y;
	for (ind=0;ind < t->Nodecount;ind ++)
	{
		if(left > t->Nodes[ind].distorted_x)
			left=t->Nodes[ind].distorted_x;
		if(right < t->Nodes[ind].distorted_x)
			right=t->Nodes[ind].distorted_x;
		if(bottom > t->Nodes[ind].distorted_y)
			bottom=t->Nodes[ind].distorted_y;
		if(top < t->Nodes[ind].distorted_y)
			top=t->Nodes[ind].distorted_y;
	}
	view->bdxLeft=left;
	view->bdyTop=top;
	view->bdxRight=right;
	view->bdyBottom=bottom;
	view->bdzTop=0;
	view->bdzBottom=0;
/*	printf("top view graph boundries\n");
	printf("------------------------\n");
	printf("lower left (%f,%f)\n",view->bdxLeft,view->bdyBottom);
	printf("upper right(%f,%f)\n",view->bdxRight,view->bdyTop);*/
}



int get_color_from_edge(topview_edge *e)
{
	RGBColor c;
	GdkColor color;
	char* color_string;
	int return_value=0;
	float Alpha=0;
	GtkHScale* AlphaScale=glade_xml_get_widget(xml, "frmHostAlphaScale");
	Alpha=gtk_range_get_value((GtkRange*) AlphaScale);

	//check visibility;
	if(	(node_visible(e->Node1->Node))
							&&
		(node_visible(e->Node2->Node)) )
			return_value=1;


	if(		( ((custom_object_data*)AGDATA(e->Node1->Node))->Selected==1)
							&&
			( ((custom_object_data*)AGDATA(e->Node2->Node))->Selected==1)
			)
	{
//		glColor4f(0,0,1,1);
		glColor4f(view->selectColor.R,view->selectColor.G,view->selectColor.B,view->selectColor.A);
		return return_value;
	}
	if(		( ((custom_object_data*)AGDATA(e->Node1->Node))->Highlighted==1)
							&&
			( ((custom_object_data*)AGDATA(e->Node2->Node))->Highlighted==1)
			)
	{
		glColor4f(0,0,1,1);
		return return_value;
	}
	color_string=agget(e->Node1->Node,"fillcolor");
	//group colors
	if((e->Node1->GroupIndex >=0) || (e->Node2->GroupIndex >=0))
	{
		if (view->Topview->TopviewData->hostactive[e->Node1->GroupIndex]==1)
		{
			gtk_color_button_get_color(view->Topview->TopviewData->gtkhostcolor[e->Node1->GroupIndex],&color);
			glColor4f(color.red/65535.0,color.green/65535.0,color.blue/65535.0,1);
			return return_value;
		}else
		{
			if (view->Topview->TopviewData->hostactive[e->Node2->GroupIndex]==1)
			{
				gtk_color_button_get_color(view->Topview->TopviewData->gtkhostcolor[e->Node2->GroupIndex],&color);
				glColor4f(color.red/65535.0,color.green/65535.0,color.blue/65535.0,1);
				return return_value;
			}
		}

	}


	if(color_string)
	{
		c=GetRGBColor(color_string);
		glColor4f(c.R,c.G,c.B,Alpha);
	}
	else
		glColor4f(e->Node1->Color.R,e->Node1->Color.G,e->Node1->Color.B,Alpha);
	return return_value;
}
int node_visible(Agnode_t* n)
{
	return ((custom_object_data*)AGDATA(n))->Visible;

}
int move_TVnodes()
{
	topview_node *v;
	int ind=0;
	for (ind=0;ind < view->Topview->Nodecount;ind ++)
	{
		v=&view->Topview->Nodes[ind];
		if( ((custom_object_data*)AGDATA(v->Node))->Selected==1)
		{
			v->x=v->x-dx;
			v->y=v->y-dy;
		}
	}
}


/*int load_host_buttons(Agraph_t *g,glCompSet* s)
{
	GtkLayout* layout;
	int btncount=0;
	int i=0;
	char buf[255];
	char *str;
	char hostbtncaption[50];
	char hostbtnregex[50];
	char hostbtncolorR[50];
	char hostbtncolorG[50];
	char hostbtncolorB[50];
	char hostbtncolorA[50];
	int X=10;
	int Y=25;
	GdkColor color;
	glCompPanel* p;
	glCompButton* b;

	layout=glade_xml_get_widget(xml, "frmHostSelectionFixed");
	str='\0';
	str=agget(g, "hostbtncount");
	if (str)	
		btncount=atoi(str);

//	Graph [hostbtncaption1="AT&T",hostbtnregex1="*.ATT*",hostbtncolorR1="1",hostbtncolorG1="0",hostbtncolorB1="0",hostbtncolorA1="1"];

	hostregex=malloc (sizeof(char**)*btncount);
	gtkhostbtn=malloc(sizeof(GtkButton*)*btncount);
	gtkhostcolor=malloc(sizeof(GtkColorButton*)*btncount);
	gtkhostbtncount=btncount;
	if (btncount > 0)
	{
		p=glCompPanelNew(25,75,165,400);
		p->data=2;		//data panel
		p->color.R=0.80;
		p->color.B=0,2;
		glCompSetAddPanel(s,p);
	}
	for (i=0;i < btncount ; i++)
	{
		sprintf(hostbtncaption,"hostbtncaption%i",i);
		sprintf(hostbtnregex,"hostbtnregex%i",i);
		sprintf(hostbtncolorR,"hostbtncolorR%i",i);
		sprintf(hostbtncolorG,"hostbtncolorG%i",i);
		sprintf(hostbtncolorB,"hostbtncolorB%i",i);
		sprintf(hostbtncolorA,"hostbtncolorA%i",i);
		printf ("caption:%s regex:%s Color(%s,%s,%s,%s)\n",
			agget(g,hostbtncaption ),
			agget(g,hostbtnregex),
			agget(g,hostbtncolorR),
			agget(g,hostbtncolorG),
			agget(g,hostbtncolorB),
			agget(g,hostbtncolorA));
		hostregex[i]=agget(g,hostbtnregex);

		b=glCompButtonNew(5,7+(i+1)*36,150,35,agget(g,hostbtncaption ),'\0',0,0);
		b->color.R=atof(agget(g,hostbtncolorR));
		b->color.G=atof(agget(g,hostbtncolorG));
		b->color.B=atof(agget(g,hostbtncolorB));
		b->color.A=1;
		b->panel=p;
		b->groupid=-1;
		b->callbackfunc=glhost_button_clicked_Slot;
		b->data=i;
		glCompSetAddButton(s,b);

		gtkhostbtn[i]=gtk_button_new_with_label(agget(g,hostbtncaption ));
		g_signal_connect ((gpointer) gtkhostbtn[i], "clicked", G_CALLBACK(host_button_clicked_Slot),i);

		color.blue=65535*atof(agget(g,hostbtncolorB));
		color.red=65535*atof(agget(g,hostbtncolorR));
		color.green=65535*atof(agget(g,hostbtncolorG));

		gtkhostcolor[i]=gtk_color_button_new_with_color(&color);

		gtk_color_button_set_alpha(gtkhostbtn[i],65535*atof(agget(g,hostbtncolorA)));


		gtk_layout_put	(layout,gtkhostbtn[i],X,Y);
		gtk_widget_set_size_request(gtkhostbtn[i],200,35);

		gtk_layout_put	(layout,gtkhostcolor[i],X+225,Y);
		gtk_widget_set_size_request(gtkhostcolor[i],40,35);

		gtk_widget_show(gtkhostbtn[i]);
		gtk_widget_show(gtkhostcolor[i]);
		Y=Y+40;
		hostactive[i]=0;
	}
	p->height=15+(btncount+1)*36;
/*	for (i=0;i < btncount ; i++)
	{
		prepare_nodes_for_groups(i);
	}
}*/



int validate_group_node(tv_node* TV_Node,char* regex_string)
{
	btree_node* n=0;
	char* data_attr1;
	char* data_attr2;
	char* data1;
	char* data2;
//		n=tree_from_filter_string("([IP=\"^10.*\",min=\"0\",max=\"0\"])");
	int valid=0;
	n=tree_from_filter_string(regex_string);
	valid=evaluate_expresions (TV_Node,n);
//	delete_node(n);	
	return valid;
}



void
on_host_alpha_change (GtkWidget *widget,gpointer     user_data)
{
	expose_event (view->drawing_area,NULL,NULL);
}

void local_zoom(topview* t)
{
	int i;
	double delx,dely,w,h,tempx,tempy;
	w=view->mg.width;
	h=view->mg.height;
	for (i=1; i< t->Nodecount; i++) 
	{
		if(is_point_in_rectangle(t->Nodes[i].x,t->Nodes[i].y,view->mg.x-view->mg.GLwidth,view->mg.y-view->mg.GLheight,
			view->mg.GLwidth*2,view->mg.GLheight*2))
		{
			delx=t->Nodes[i].x-view->mg.x;
			dely=t->Nodes[i].y-view->mg.y;
			tempx=view->mg.x+delx*view->mg.kts;
			tempy=view->mg.y+dely*view->mg.kts;
			if(is_point_in_rectangle(tempx,tempy,view->mg.x-view->mg.GLwidth,view->mg.y-view->mg.GLheight,
				view->mg.GLwidth*2,view->mg.GLheight*2))
			{
				t->Nodes[i].distorted_x=view->mg.x+delx*view->mg.kts;
				t->Nodes[i].distorted_y=view->mg.y+dely*view->mg.kts;
				t->Nodes[i].zoom_factor=view->mg.kts;
			}
			else//get intersections and move nodes to edges of magnifier box
			{
				t->Nodes[i].distorted_x =t->Nodes[i].x;
				t->Nodes[i].distorted_y =t->Nodes[i].y;
				t->Nodes[i].zoom_factor=1;
			}

		}
		else
		{
			t->Nodes[i].distorted_x =t->Nodes[i].x;
			t->Nodes[i].distorted_y =t->Nodes[i].y;
			t->Nodes[i].zoom_factor=1;
		}
	}
}
void fisheye_polar(double x_focus, double y_focus,topview* t)
{
	int i;
	double distance, distorted_distance, ratio,range;

	range=0;
	for (i=1; i< t->Nodecount; i++) 
	{
		if(point_within_ellips_with_coords(x_focus,y_focus,view->fmg.R,view->fmg.R,t->Nodes[i].x,t->Nodes[i].y))
		{
			range = MAX(range,dist(t->Nodes[i].x,t->Nodes[i].y, x_focus, y_focus));
		}
	}

	for (i=1; i< t->Nodecount; i++) 
	{

		if(point_within_ellips_with_coords(x_focus,y_focus,view->fmg.R,view->fmg.R,t->Nodes[i].x,t->Nodes[i].y))
		{
			distance = dist(t->Nodes[i].x, t->Nodes[i].y, x_focus, y_focus);
			distorted_distance = G(distance/range)*range;
			if (distance!=0) {
				ratio = distorted_distance/distance;		
			}
			else {
				ratio = 0;
			}
			t->Nodes[i].distorted_x = x_focus +(t->Nodes[i].x-x_focus)*ratio;
			t->Nodes[i].distorted_y = y_focus + (t->Nodes[i].y-y_focus)*ratio;
			t->Nodes[i].zoom_factor=1*distorted_distance/distance;
		}
		else
		{
			t->Nodes[i].distorted_x =t->Nodes[i].x;
			t->Nodes[i].distorted_y =t->Nodes[i].y;
			t->Nodes[i].zoom_factor=1;
		}
	}	
}
void originate_distorded_coordinates(topview* t)
{
	//sets original coordinates values to distorded coords. this happens when lieft mouse click is released in geometrical fisyehey mode
	int i;
	for (i=1; i< t->Nodecount; i++) 
	{
			t->Nodes[i].distorted_x =t->Nodes[i].x;
			t->Nodes[i].distorted_y =t->Nodes[i].y;
			t->Nodes[i].zoom_factor=1;
	}	
}





double dist(double x1, double y1, double x2, double y2)
{
	return sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
}
double G(double x)
{
	// distortion function for fisheye display
	return (view->fmg.fisheye_distortion_fac+1)*x/(view->fmg.fisheye_distortion_fac*x+1);
}

void test_callback()
{
	printf ("test button click\n");
}
void menu_click_control(void* p)
{
	glCompSet* s;
	int ind=0;
	s=((glCompButton*)p)->parentset;
	for (ind=0;ind < s->panelcount;ind++)
	{
		if (s->panels[ind]->data > 0)
			glCompPanelHide(s->panels[ind]);//hide all panels
		if (s->panels[ind]->data==1)	//control panel
		{
			glCompPanelShow(s->panels[ind]);
		}
	}
}
void menu_click_data(void* p)
{
	glCompSet* s;
	int ind=0;
	s=((glCompButton*)p)->parentset;
	for (ind=0;ind < s->panelcount;ind++)
	{
		if (s->panels[ind]->data > 0)
			glCompPanelHide(s->panels[ind]);//hide all panels
		if (s->panels[ind]->data==2)	//data panel
		{
			glCompPanelShow(s->panels[ind]);
		}
	}
}
void menu_click_hide(void* p)
{
	glCompSet* s;
	int ind=0;
	s=((glCompButton*)p)->parentset;
	for (ind=0;ind < s->panelcount;ind++)
	{
		if (s->panels[ind]->data > 0)
			glCompPanelHide(s->panels[ind]);//hide all panels
	}
}
void menu_click_pan(void* p)
{
	view->mouse.mouse_mode=MM_PAN;

}
void menu_click_zoom(void* p)
{
	view->mouse.mouse_mode=MM_ZOOM;

}
void menu_click_fisheye_magnifier(void* p)
{
	view->mouse.mouse_mode=MM_FISHEYE_MAGNIFIER;

}
void menu_click_zoom_minus(void* p)
{
	if((view->zoom - ZOOM_STEP) > MIN_ZOOM)
		view->zoom=view->zoom-ZOOM_STEP;
	else
		view->zoom=MIN_ZOOM;
}
void menu_click_zoom_plus(void* p)
{
	if((view->zoom + ZOOM_STEP) < MAX_ZOOM)
		view->zoom=view->zoom+ZOOM_STEP;
	else
		view->zoom=MAX_ZOOM;

}
void menu_click_alpha_plus(void* p)
{
	if((view->zoom + ZOOM_STEP) < MAX_ZOOM)
		view->zoom=view->zoom+ZOOM_STEP;
	else
		view->zoom=MAX_ZOOM;

}


glCompSet* glcreate_gl_topview_menu()
{
	glCompSet* s=malloc(sizeof(glCompSet));
	glCompPanel* p;
	glCompButton* b;
	glCompLabel* l;
	s->panelcount=0;
	s->panels='\0';
	s->buttoncount=0;
	s->buttons='\0';
	s->labels='\0';
	s->labelcount=0;
	/*panel data 
		0 :small bottom
		1 :control
		2 :data


	*/

	//small panel left bottom
	p=glCompPanelNew(25,25,245,40);
	p->data=0;
	glCompSetAddPanel(s,p);
	
	b=glCompButtonNew(5,7,75,25,"CONTROL",'\0',0,0);
	b->panel=p;
	b->groupid=1;
	b->customptr=view;
	glCompSetAddButton(s,b);
	b->callbackfunc=menu_click_control;

	b=glCompButtonNew(85,7,75,25,"DATA",'\0',0,0);
	b->panel=p;
	b->customptr=view;
	b->groupid=1;
	b->callbackfunc=menu_click_data;
	glCompSetAddButton(s,b);


	b=glCompButtonNew(165,7,75,25,"HIDE",'\0',0,0);
	b->color.R=1;
	b->customptr=view;
	b->panel=p;
	b->groupid=1;
	b->callbackfunc=menu_click_hide;
	glCompSetAddButton(s,b);

	//control panel
	p=glCompPanelNew(25,75,165,200);
	p->data=1;		//control panel
	glCompSetAddPanel(s,p);

	//view mode normal button
	b=glCompButtonNew(5,7,75,25,"NORMAL",'\0',0,0);
	b->color.R=0;
	b->color.G=1;
	b->color.B=0.1;
	b->customptr=view;
	b->panel=p;
	b->groupid=2;
	glCompSetAddButton(s,b);
	//view mode fisheye button
	b=glCompButtonNew(85,7,75,25,"FISHEYE",'\0',0,0);
	b->color.R=0;
	b->color.G=1;
	b->color.B=0.1;
	b->customptr=view;
	b->panel=p;
	b->groupid=2;
	glCompSetAddButton(s,b);
	//pan button
	b=glCompButtonNew(5,120,72,72,"adasasds","c:/pan.raw",72,72);
	b->groupid=3;
	b->customptr=view;
	b->panel=p;
	b->callbackfunc=menu_click_pan;
	glCompSetAddButton(s,b);
	//zoom
	b=glCompButtonNew(85,120,72,72,"adasasds","c:/zoom.raw",72,72);
	b->groupid=3;
	b->customptr=view;
	b->panel=p;
	b->callbackfunc=menu_click_zoom;
	glCompSetAddButton(s,b);
	//zoom +
	b=glCompButtonNew(85,82,36,36,"adasasds","c:/zoomplus.raw",36,36);
	b->groupid=0;
	b->customptr=view;
	b->panel=p;
	b->callbackfunc=menu_click_zoom_plus;
	glCompSetAddButton(s,b);
	//zoom -
	b=glCompButtonNew(121,82,36,36,"adasasds","c:/zoomminus.raw",36,36);
	b->groupid=0;
	b->panel=p;
	b->customptr=view;
	b->callbackfunc=menu_click_zoom_minus;
	glCompSetAddButton(s,b);

	b=glCompButtonNew(5,45,72,72,"adasasds","c:/fisheye.raw",72,72);
	b->groupid=3;
	b->panel=p;
	b->customptr=view;
	b->callbackfunc=menu_click_fisheye_magnifier;
	glCompSetAddButton(s,b);
	//zoom percantage label
	l=glCompLabelNew(100,45,24,"100");
	l->panel=p;
	l->fontsizefactor=0.4;
	glCompSetAddLabel(s,l);
	view->Topview->customptr=l;
	l=glCompLabelNew(93,65,20,"zoom");
	l->panel=p;
	l->fontsizefactor=0.4;
	glCompSetAddLabel(s,l);

	glCompPanelHide(p);

/*	//Data Panel
	p=glCompPanelNew(25,75,165,400);
	p->data=2;		//data panel
	glCompSetAddPanel(s,p);
	//alpha plus button
	b=glCompButtonNew(75,3,36,36,"","c:/zoomplus.raw",36,36);
	b->groupid=0;
	b->panel=p;
	b->callbackfunc=menu_click_alpha_plus;
	glCompSetAddButton(s,b);
	//alpha minus button
	b=glCompButtonNew(113,3,36,36,"","c:/zoomminus.raw",36,36);
	b->groupid=0;
	b->panel=p;
	b->callbackfunc=menu_click_alpha_plus;
	glCompSetAddButton(s,b);

	l=glCompLabelNew(5,8,18,"ALPHA");
	l->panel=p;
	l->fontsizefactor=0.4;
	glCompSetAddLabel(s,l);*/


	return s;

}
//calbacks




