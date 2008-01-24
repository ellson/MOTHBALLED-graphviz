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
#define DEFINE_MATERIALS
#include "topview.h"
#include "math.h"
#include "btree.h"
topview Topview;
GtkButton** gtkhostbtn;
int gtkhostbtncount;
GtkColorButton** gtkhostcolor;
int hostactive[50];   //temporary static, convert to dynamic,realloc
char** hostregex;
int fisheye_distortion_fac;

static int font=0;
static float dx=0.0;
static float dy=0.0;
#define NODE_ZOOM_LIMIT	-25.3
#define NODE_CIRCLE_LIMIT	-7.3
extern void PrepareTopview(Agraph_t *g)
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
	int ind,ind2,data_type_count;	//number of columns for custom topview data ,IP ,HOST, etc
	char buf[256];
	ind=0;ind2=0;
	//data attributes are read from graph's attributes DataAttribute1 and DataAttribute2
	//node_data_attribute1;	//for topview graphs this is the node data attribute to put as label
	//node_data_attribute2;	//for topview graphs this is the node data attribute to be stored and used for something else

	//data list

	/*	if ( agget(g, "DataCount"))
	{
		data_type_count=AToInt(agget(g, "DataCount"));
	}*/
	gtk_widget_hide(glade_xml_get_widget(xml, "layout6"));	//hide top panel
	data_type_count=0;
	d_attr1=agget(g, "DataAttribute1");
	d_attr2=agget(g, "DataAttribute2");



	for (v = agfstnode(g); v; v = agnxtnode(g, v))
	{
		//set node TV reference
    	((custom_object_data*)AGDATA(v))->TVRef=ind;		//Topview reference
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
			if (!GP_flag)
			{
				Topview.limits[0]=a;
				Topview.limits[2]=a;
				Topview.limits[1]=b;
				Topview.limits[3]=b;
				GP_flag=1;
			}
			if (a < Topview.limits[0])
				Topview.limits[0]=a;
			if (a > Topview.limits[2])
				Topview.limits[2]=a;
			if (b < Topview.limits[1])
				Topview.limits[1]=b;
			if (b > Topview.limits[3])
				Topview.limits[3]=b;
		}
		Topview.Nodes[ind].GroupIndex=-1;
		randomize_color(&(Topview.Nodes[ind].Color),2);
		Topview.Nodes[ind].Node=v;
		Topview.Nodes[ind].x=a;
		Topview.Nodes[ind].y=b;
		Topview.Nodes[ind].z=c;
		Topview.Nodes[ind].distorted_x=a;
		Topview.Nodes[ind].distorted_y=b;
		Topview.Nodes[ind].zoom_factor=1;
		Topview.Nodes[ind].degree=agdegree(g,v,1,1);
		Topview.Nodes[ind].node_alpha=log((float)Topview.Nodes[ind].degree+0.3);
		if(d_attr1)
		{
			str=agget(v,d_attr1);
			if(str)
			{
				Topview.Nodes[ind].Label=strdup(str);

			}

		}
		if(d_attr2)
		{
			str=agget(v,d_attr2);
			if(str)
			{
				Topview.Nodes[ind].Label2=strdup(str);
				//gtk_list_store_set_value(Topview.DataList,&iter,2,str);
			}
		}
		for (e = agfstout(g,v) ; e ; e = agnxtout (g,e))
		{
			Topview.Edges[ind2].Hnode=aghead(e);
			Topview.Edges[ind2].Tnode=agtail(e);
			Topview.Edges[ind2].Edge=e;
			
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

				Topview.Edges[ind2].x1=a;
				Topview.Edges[ind2].y1=b;
				Topview.Edges[ind2].z1=b;
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

				Topview.Edges[ind2].x2=a;
				Topview.Edges[ind2].y2=b;
				Topview.Edges[ind2].z2=c;
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
			Topview.Edges[ind2].Node1=  &Topview.Nodes[((custom_object_data*)AGDATA(Topview.Edges[ind2].Tnode))->TVRef];
			Topview.Edges[ind2].Node2=  &Topview.Nodes[((custom_object_data*)AGDATA(Topview.Edges[ind2].Hnode))->TVRef];
			ind2++;

		}
		ind++;
	}
	Topview.Nodecount=ind;
	Topview.Edgecount=ind2;
	fisheye_distortion_fac=5; //simdilik burda dursun , need to be hooked to a widget
	set_boundries();
	set_update_required(&Topview);
	set_boundries();
//	load_host_buttons(g);
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
	if(view.zoom > NODE_ZOOM_LIMIT)
	{
		glPointSize(15/view.zoom*-1);
		//draw nodes
		set_topview_options();
		if(view.zoom < NODE_CIRCLE_LIMIT)
			glBegin(GL_POINTS);
		for (ind=0;ind < Topview.Nodecount;ind ++)
		{

			if((Topview.Nodes[ind].x > view.clipX1) && (Topview.Nodes[ind].x < view.clipX2) &&(Topview.Nodes[ind].y > view.clipY1)&&(Topview.Nodes[ind].y < view.clipY2))
			{
				if(1)
				{
					v=&Topview.Nodes[ind];
					if(!node_visible(v->Node))
						break;

					select_topview_node(v);
					//UPDATE topview data from cgraph
					if (v->update_required)
						update_topview_node_from_cgraph(v);
					if( ((custom_object_data*)AGDATA(v->Node))->Selected==1)
					{
						glColor4f(view.selectColor.R,view.selectColor.G,view.selectColor.B,view.selectColor.A);
						ddx=dx;ddy=dy;
					}
					else		//get the color from node
					{
						glColor4f(v->Color.R,v->Color.G,v->Color.B,v->node_alpha);
		//					glColor4f(1,0,0,v->node_alpha);
		//				glColor4f (log((double)v->degree+0.5),v->Color.G,v->Color.B,);
						ddx=0;ddy=0;
					}

					
					if(view.zoom < NODE_CIRCLE_LIMIT)
						glVertex3f(v->distorted_x-ddx,v->distorted_y-ddy,-0.01); 
					else
						drawCircle(v->distorted_x-ddx,v->distorted_y-ddy,v->node_alpha*v->zoom_factor);
				}
			}
		}
		if(view.zoom< NODE_CIRCLE_LIMIT)
			glEnd();
		for (ind=0;ind < Topview.Nodecount;ind ++)
		{

			v=&Topview.Nodes[ind];
			if(!node_visible(v->Node))
				break;

			draw_topview_label(v);

		}
	}
	//draw edges
//	glLineWidth(5/view.zoom*-1);
	glBegin(GL_LINES);
	set_topview_options();
	for (ind=0;ind < Topview.Edgecount;ind ++)
	{
		if(
			((Topview.Edges[ind].x1 > view.clipX1) && (Topview.Edges[ind].x1 < view.clipX2) &&(Topview.Edges[ind].y1 > view.clipY1)&&(Topview.Edges[ind].y1 < view.clipY2))
							||
			((Topview.Edges[ind].x2 > view.clipX1) && (Topview.Edges[ind].x2 < view.clipX2) &&(Topview.Edges[ind].y2 > view.clipY1)&&(Topview.Edges[ind].y2 < view.clipY2))

			)
		{
			e=&Topview.Edges[ind];
			select_topview_edge(e);
			if( ((custom_object_data*)AGDATA(e->Node1->Node))->Selected==1)	//tail is selected
			{		 ddx=dx;ddy=dy;}
			else
			{		ddx=0;ddy=0;}
			if( ((custom_object_data*)AGDATA(e->Node2->Node))->Selected==1)	//head
			{		 dddx=dx;dddy=dy;}
			else
			{		dddx=0;dddy=0;}


			if(get_color_from_edge(e))
			{
				glVertex3f(e->Node1->distorted_x-ddx,e->Node1->distorted_y-ddy,-0.01);
				glVertex3f(e->Node2->distorted_x-dddx,e->Node2->distorted_y-dddy,-0.01);
			}
		}
	}
	glEnd();
	if((view.Selection.Active>0) && (!SignalBlock))
	{
			view.Selection.Active=0;
			drawTopViewGraph(g);
			SignalBlock=1;
			expose_event (drawing_area,NULL,NULL);
			SignalBlock=0;
	}

}


int select_topview_node(topview_node *n)
{
	if(!view.Selection.Active)
		return 0;
	if(is_point_in_rectangle(n->x,n->y,view.Selection.X,view.Selection.Y,view.Selection.W,view.Selection.H))
	{
			
		switch (view.Selection.Type)
		{
		case 0:

			if ( ((custom_object_data*)AGDATA(n->Node))->Selected==0)
			{
				((custom_object_data*)AGDATA(n->Node))->Selected=1;
				select_object (view.g[view.activeGraph],n->Node);
			}
			else
			{
				((custom_object_data*)AGDATA(n->Node))->Selected=1;
				deselect_object (view.g[view.activeGraph],n->Node);
			}
			break;

		case 1:
		case 2:
			if(view.Selection.Anti==0)
			{
				select_object (view.g[view.activeGraph],n->Node);
				view.Selection.AlreadySelected=1;
			}
			else
			{
				deselect_object (view.g[view.activeGraph],n->Node);
				view.Selection.AlreadySelected=1;
			}
			break;

		}
	}
}
int select_topview_edge(topview_edge *e)
{
	int r=0;
	if(!view.Selection.Active)
		return 0;
	r=(lineintersects(e->x1,e->y1,e->x2,e->y2));
	if(r >= 0)
	{
			
		switch (view.Selection.Type)
		{
		case 0:
			if ( ((custom_object_data*)AGDATA(e->Edge))->Selected==0)
			{
				((custom_object_data*)AGDATA(e->Edge))->Selected=1;
				select_object (view.g[view.activeGraph],e->Edge);
			}
			else
			{
				((custom_object_data*)AGDATA(e->Edge))->Selected=1;
				deselect_object (view.g[view.activeGraph],e->Edge);
			}
			break;

/*		case 1:
		case 2:
			if(view.Selection.Anti==0)
			{
				select_object (view.g[view.activeGraph],n->Node);
				view.Selection.AlreadySelected=1;
			}
			else
			{
				deselect_object (view.g[view.activeGraph],n->Node);
				view.Selection.AlreadySelected=1;
			}
			break;*/

		}
	}


/*	else if (view.Selection.Type==0)
	{
			deselect_object (view.g[view.activeGraph],n->Node);
			view.Selection.AlreadySelected=1;
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

		Node->Color.R=view.penColor.R;
		Node->Color.G=view.penColor.G;
		Node->Color.B=view.penColor.B;
		Node->Color.A=view.penColor.A;
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
		Edge->Color.R=view.penColor.R;
		Edge->Color.G=view.penColor.G;
		Edge->Color.B=view.penColor.B;
		Edge->Color.A=view.penColor.A;
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

int draw_topview_label(topview_node* v)
{

	float fs=0;
	float ddx=0;
	float ddy=0;
	if(!v->Label)
		return 0;
	if((view.zoom*-1/v->degree/v->zoom_factor) > 2)
		return 0;
	if((v->distorted_x > view.clipX1) && (v->distorted_x < view.clipX2) &&(v->distorted_y > view.clipY1)&&(v->distorted_y < view.clipY2))
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
		fontColorA (log((double)v->degree+1),view.penColor.G,view.penColor.B,view.penColor.A/log((float)v->degree)*-0.6*view.zoom);
//		printf("%f \n",view.penColor.A/log((float)v->degree)*-0.02*view.zoom);
		fontDrawString (v->distorted_x-ddx,v->distorted_y-ddy,v->Label,fs*5);
		return 1;
	}
	else
		return 0;
}


int randomize_color(RGBColor* c,int brightness)
{
	float R,B,G;
	float add;
	R=(float)(rand() % 255) / 255.0;
	G=(float)(rand() % 255) / 255.0;
	B=(float)(rand() % 255) / 255.0;
	add=(brightness-(R+G+B))/3;
	R = R;
	G = G;
	B = B;
	c->R=R;
	c->G=G;
	c->B=B;
//	printf(" %f %f %f \n",R,G,B);
}


void drawCircle(float x,float y,float radius)
{
	int i;
	if(radius <0.3)
		radius=0.4;
	glBegin(GL_POLYGON);
   for (i=0; i < 360; i=i+10)
   {
      float degInRad = i*DEG2RAD;
      glVertex2f(x+cos(degInRad)*radius,y+sin(degInRad)*radius);
   }
 
   glEnd();
}

void set_topview_options()
{
	
	if ((view.mouse.mouse_mode==10) && (view.mouse.mouse_down==1))	//selected, if there is move move it, experimental
	{
		dx=view.GLx-view.GLx2;
		dy=view.GLy-view.GLy2;
	}
	else
	{
		dx=0;
		dy=0;
	}

}
void set_boundries()
{
	GLfloat bdxLeft,bdyTop,bdzTop;	//border top coordinates
	GLfloat bdxRight,bdyBottom,bdzBottom; //border bottom coordinates

	view.bdxLeft=Topview.limits[0];
	view.bdyTop=Topview.limits[3];
	view.bdxRight=Topview.limits[2];
	view.bdyBottom=Topview.limits[1];
	
	view.bdzTop=0;
	view.bdzBottom=0;
//	view.panx=view.bdxLeft;
//	view.pany=view.bdyBottom;
//	view.prevpanx=view.bdxLeft;
//	view.prevpany=view.bdyBottom;

	printf ("graph boundries (%f,%f)/(%f,%f) \n" , view.bdxLeft,view.bdyTop,view.bdxRight,view.bdyBottom);
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
		glColor4f(view.selectColor.R,view.selectColor.G,view.selectColor.B,view.selectColor.A);
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
		if (hostactive[e->Node1->GroupIndex]==1)
		{
			gtk_color_button_get_color(gtkhostcolor[e->Node1->GroupIndex],&color);
			glColor4f(color.red/65535.0,color.green/65535.0,color.blue/65535.0,1);
			return return_value;
		}else
		{
			if (hostactive[e->Node2->GroupIndex]==1)
			{
				gtk_color_button_get_color(gtkhostcolor[e->Node2->GroupIndex],&color);
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
	for (ind=0;ind < Topview.Nodecount;ind ++)
	{
		v=&Topview.Nodes[ind];
		if( ((custom_object_data*)AGDATA(v->Node))->Selected==1)
		{
			v->x=v->x-dx;
			v->y=v->y-dy;
		}
	}
}
int draw_navigation_menu()
{

    int vPort[4];
    /* get current viewport */
    glGetIntegerv (GL_VIEWPORT, vPort);

	glMatrixMode (GL_PROJECTION);
	glPushMatrix ();
	glLoadIdentity ();

  
	glOrtho (0, vPort[2], 0, vPort[3], -1, 1);
	glMatrixMode (GL_MODELVIEW);
	glPushMatrix ();
	glLoadIdentity ();
		drawCircle(100,100,1);

	glMatrixMode (GL_PROJECTION);
	glPopMatrix ();
	glMatrixMode (GL_MODELVIEW);
	glPopMatrix ();
}

int load_host_buttons(Agraph_t *g)
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
	printf ("all regex\n");
	for (i=0;i < btncount ; i++)
	{

		prepare_nodes_for_groups(i);
		
	}
}

int prepare_nodes_for_groups(int groupindex)
{
	GdkColor color;
	int i=0;
	int count=0;
	tv_node tvn;
	gtk_color_button_get_color(gtkhostcolor[0],&color);

	for (i;i < Topview.Nodecount ; i++)
	{
		tvn.index=i;
		if(validate_group_node(&tvn,hostregex[groupindex]))
		{
			count ++;
			gtk_color_button_get_color(gtkhostcolor[groupindex],&color);
			Topview.Nodes[i].GroupIndex=groupindex;
			Topview.Nodes[i].GroupColor.R=color.red/65535.0;
			Topview.Nodes[i].GroupColor.G=color.green/65535.0;
			Topview.Nodes[i].GroupColor.B=color.blue/65535.0;
		}
	}
	printf ("%i matches for group:%i (%s) RGB(%f,%f,%f)\n",count,groupindex,hostregex[groupindex],color.red/65535.0,color.green/65535.0,color.blue/65535.0);
}


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

void host_button_clicked_Slot(GtkWidget *widget,gpointer     user_data)
{
	//negative active
	int i;
	if(hostactive[(int)user_data]==0)
		hostactive[(int)user_data]=1;
	else
		hostactive[(int)user_data]=0;
	printf("-----------\n");
	printf ("host%i:%i\n",(int)user_data,hostactive[(int)user_data]);
	printf("-----------\n");
	for (i=0;i < gtkhostbtncount ; i++)
	{
		printf ("host%i:%i\n",i,hostactive[i]);
	}
	expose_event (drawing_area,NULL,NULL);
}
void
on_host_alpha_change (GtkWidget *widget,gpointer     user_data)
{
	expose_event (drawing_area,NULL,NULL);
}

void local_zoom(topview* t)
{
	int i;
	double delx,dely,w,h,tempx,tempy;
	w=view.mg.width;
	h=view.mg.height;
	for (i=1; i< t->Nodecount; i++) 
	{
		if(is_point_in_rectangle(t->Nodes[i].x,t->Nodes[i].y,view.mg.x-view.mg.width,view.mg.y-view.mg.height,
			view.mg.width*2,view.mg.height*2))
		{
			delx=t->Nodes[i].x-view.mg.x;
			dely=t->Nodes[i].y-view.mg.y;

			t->Nodes[i].distorted_x=view.mg.x+delx*view.mg.kts;
			t->Nodes[i].distorted_y=view.mg.y+dely*view.mg.kts;
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
		if(point_within_ellips_with_coords(x_focus,y_focus,view.fmg.R,view.fmg.R,t->Nodes[i].x,t->Nodes[i].y))
		{
			range = MAX(range,dist(t->Nodes[i].x,t->Nodes[i].y, x_focus, y_focus));
		}
	}

	for (i=1; i< t->Nodecount; i++) 
	{

		if(point_within_ellips_with_coords(x_focus,y_focus,view.fmg.R,view.fmg.R,t->Nodes[i].x,t->Nodes[i].y))
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
	return (fisheye_distortion_fac+1)*x/(fisheye_distortion_fac*x+1);
}


