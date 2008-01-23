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

/*

XDOT DRAWING FUNCTIONS, maybe need to move them somewhere else
		for now keep them at the bottom
*/
#include "draw.h"
#include "topview.h"
//delta values
static float dx=0.0;
static float dy=0.0;


GLubyte rasters [24]={
	0xc0,0x00,0xc0,0x00,0xc0,0x00,0xc0,0x00,0xc0,0x00,0xff,0x00,0xff,0x00,
	0xc0,0x00,0xc0,0x00,0xc0,0x00,0xff,0xc0,0xff,0xc0};


void DrawBezier(GLfloat* xp,GLfloat* yp,GLfloat* zp, int filled,int param)
{
	/*copied from NEHE */
	//Written by: David Nikdel ( ogapo@ithink.net ) 
	//
	
	// Control points (substitute these values with your own if you like)
	double Ax = xp[0]; double Ay = yp[0]; double Az = zp[0];
	double Bx = xp[1]; double By = yp[1]; double Bz = zp[1];
	double Cx = xp[2]; double Cy = yp[2]; double Cz = zp[2];
	double Dx = xp[3]; double Dy = yp[3]; double Dz = zp[3];
	double X;
	double Y;
	double Z;
	int i = 0;//loop index
	// Variable
	double a = 1.0;
	double b = 1.0 - a;
	// Tell OGL to start drawing a line strip
	glLineWidth(view.LineWidth);
	if (!filled)
	{

		if (param==0)
			glColor4f(view.penColor.R,view.penColor.G,view.penColor.B,view.penColor.A);
		if (param==1)	//selected
			glColor4f(view.selectColor.R,view.selectColor.G,view.selectColor.B,view.selectColor.A);

		glBegin(GL_LINE_STRIP);
	}
	else
	{
		if (param==0)
			glColor4f(view.fillColor.R,view.fillColor.G,view.fillColor.B,view.penColor.A);
		if (param==1)	//selected
			glColor4f(view.selectColor.R,view.selectColor.G,view.selectColor.B,view.selectColor.A);
		glBegin(GL_POLYGON);
	}
	/* We will not actually draw a curve, but we will divide the curve into small
	points and draw a line between each point. If the points are close enough, it
	will appear as a curved line. 20 points are plenty, and since the variable goes
	from 1.0 to 0.0 we must change it by 1/20 = 0.05 each time */
	for(i = 0; i <= 20; i++)
	{
	  // Get a point on the curve
		X = Ax*a*a*a + Bx*3*a*a*b + Cx*3*a*b*b + Dx*b*b*b;
		Y = Ay*a*a*a + By*3*a*a*b + Cy*3*a*b*b + Dy*b*b*b;
		Z = Az*a*a*a + Bz*3*a*a*b + Cz*3*a*b*b + Dz*b*b*b;
	  // Draw the line from point to point (assuming OGL is set up properly)
		glVertex3d(X, Y, Z);
	  // Change the variable
		a -= 0.05;
		b = 1.0 - a;
	}
// Tell OGL to stop drawing the line strip
	glEnd();
}

void DrawBeziers(xdot_op* op,int param)
{
	//SEND ALL CONTROL POINTS IN 3D ARRAYS

	GLfloat tempX[4];
	GLfloat tempY[4];
	GLfloat tempZ[4];
	int temp=0;
	int filled;
	int i=0;
	float spline_kts=0;
	SelectBeziers(op);
	relocate_spline(op,param);
	if(op->kind == xd_filled_bezier)
		filled=1;
	else
		filled=0;

	for (i=0;i < op->u.bezier.cnt ; i= i + 1)
	{
		if (temp==4)
		{
			DrawBezier(tempX,tempY,tempZ,filled,param);
			tempX[0]=op->u.bezier.pts[i-1].x;
			tempY[0]=op->u.bezier.pts[i-1].y;
			tempZ[0]=op->u.bezier.pts[i-1].z;
			temp=1;
			tempX[temp]=op->u.bezier.pts[i].x;			
			tempY[temp]=op->u.bezier.pts[i].y;			
			tempZ[temp]=op->u.bezier.pts[i].z;			
			temp=temp+1;
		}
		else
		{
			tempX[temp]=op->u.bezier.pts[i].x;
			tempY[temp]=op->u.bezier.pts[i].y;
			tempZ[temp]=op->u.bezier.pts[i].z;
			temp=temp+1;
		}
	}
	DrawBezier(tempX,tempY,tempZ,filled,param);
}

//function to load .raw files
void load_raw_texture ( char *file_name, int width, int height, int depth, GLenum colour_type, GLenum filter_type )
{ 
	//Line 3 creates a pointer to an (as yet unallocated) array of gl unsigned bytes - 
	//this will store our image until we've passed it to opengl.  Line 4 stores the file pointer, don't worry about that. 
	GLubyte *raw_bitmap ;
	FILE    *file; 

	//The if statement from line 6-10 opens the file and reports an error if it doesn't exist.   

	if ((file = fopen(file_name, "rb"))==NULL)
	{
		printf("File Not Found : %s\n",file_name);
		exit (1);
	} 

	//Line 11 allocates the correct number of bytes for the size and depth of the image.  Remember, our image depth will usually be 3 -- one 'channel' each for red, green and blue values.   
	//Lines 13-18 check if the memory was allocated correctly and quit the program if there was a problem. 

	raw_bitmap = (GLubyte *) malloc ( width * height * depth * ( sizeof ( GLubyte )) );

	if ( raw_bitmap == NULL )
	{
		printf ( "Cannot allocate memory for texture\n" );
		fclose ( file );
		exit   ( 1 );
	} 

	//Line 19 reads the required number of bytes from the file and places them into our glubyte array.  Line 20 closes the close as it isn't required anymore.   
	fread  ( raw_bitmap , width * height * depth, 1 , file );
	fclose ( file); 

	//Lines 22-25 set the texture's mapping type and environment settings.  GL_TEXTURE_MAG_FILTER and GL_TEXTURE_MIN_FILTER are enumerands that let us change the way in which opengl magnifies and minifies the texture.  If we passed GL_LINEAR to the function, our texture would be interpolated using bilinear filtering (in other words, it'd appear smoothed).  If we passed GL_NEAREST then no smoothing would occur.  By passing GL_MODULATE to the texture environment function, we tell opengl to blend the texture with the base colour of the object.  Had we specified GL_DECAL or GL_REPLACE then the base colour (and therefore our lighting effect) would be replaced purely with the colours of the texture. 
	glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter_type );
	glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter_type );

	glTexEnvf ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE ); 

	//Line 27 passes the image to opengl using the function gluBuild2DMipmaps.
	//Read a graphics book for a description of mipmaps.
	//This function will also resize the texture map if it doesn't conform 
	//to the opengl restriction of height and width values being a power of 2.   
	gluBuild2DMipmaps ( GL_TEXTURE_2D, colour_type, width, height, colour_type, GL_UNSIGNED_BYTE, raw_bitmap ); 

	//We don't need our texture in the temporary array anymore as it has been passed to opengl.
	//We use the standard C library call 'free' to return the allocated memory.  
	free ( raw_bitmap );
}

//loads texture from bitmap files, bmp
//hopefully
int load_bitmap(char *filename) 
{ 
#ifdef _WIN32
	unsigned char *l_texture;
	int i, j=0;
	FILE *l_file;
	BITMAPFILEHEADER fileheader; 
	BITMAPINFOHEADER infoheader;
	RGBTRIPLE rgb;
	l_file = fopen(filename, "r");
	if(l_file==NULL)
	{
		printf("file could not be loaded!.\n");
		return (-1);
	}
	fread(&fileheader, sizeof(fileheader), 1, l_file);
	fseek(l_file, sizeof(fileheader), SEEK_SET);
	fread(&infoheader, sizeof(infoheader), 1, l_file);
	l_texture = (GLubyte *) malloc(infoheader.biWidth * infoheader.biHeight * 4);
	memset(l_texture, 0, infoheader.biWidth * infoheader.biHeight * 4);
	for (i=0; i < infoheader.biWidth*infoheader.biHeight; i++)
	{ 
		fread(&rgb, sizeof(rgb), 1, l_file); 
		l_texture[j+0] = rgb.rgbtRed; // Red component
		l_texture[j+1] = rgb.rgbtRed; // Green component
		l_texture[j+2] = rgb.rgbtBlue; // Blue component
		l_texture[j+3] = 255; // Alpha value
		j += 4; // Go to the next position
	}
	glBindTexture(GL_TEXTURE_2D, view.font_texture_count);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_REPEAT );
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, infoheader.biWidth, infoheader.biHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, l_texture);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 4, infoheader.biWidth, infoheader.biHeight, GL_RGBA, GL_UNSIGNED_BYTE, l_texture);
	free(l_texture);
	return (view.font_texture_count);
#else
	return 0;
#endif
}

//Draws a ellpise made out of points.
//void DrawEllipse(xdot_point* xpoint,GLfloat xradius, GLfloat yradius,int filled)
void DrawEllipse(xdot_op* op,int param)
//void DrawEllipse(GLfloat x,GLfloat y,GLfloat xradius, GLfloat yradius,int filled)
{
	//if xradius and yradius are same values, output is circle
	GLfloat x,y,xradius,yradius;
	int i=0;
	int filled;
	set_options(op,param);
	x=op->u.ellipse.x-dx;
	y=op->u.ellipse.y-dy;
	xradius=(GLfloat)op->u.ellipse.w;
	yradius=(GLfloat)op->u.ellipse.h;
	SelectEllipse(op);
	if(op->kind == xd_filled_ellipse)
	{
		if (param==0)	
			glColor4f(view.fillColor.R,view.fillColor.G,view.fillColor.B,view.fillColor.A);
		if (param==1)	//selected
			glColor4f(view.selectColor.R,view.selectColor.G,view.selectColor.B,view.selectColor.A);

		filled=1;
	}
	else
	{
		if (param==0)
			glColor4f(view.penColor.R,view.penColor.G,view.penColor.B,view.penColor.A);
		if (param==1)	//selected
			glColor4f(view.selectColor.R,view.selectColor.G,view.selectColor.B,view.selectColor.A);

		filled=0;
	}

	glLineWidth(view.LineWidth);
	if (!filled)
		glBegin(GL_LINE_LOOP);
	else
		glBegin(GL_POLYGON);
   for (i=0; i < 360; i=i+1)
   {
      //convert degrees into radians
      float degInRad = i*DEG2RAD;
	  glVertex2f(x+cos(degInRad)*xradius,y+sin(degInRad)*yradius);
   }
   glEnd();
}

void DrawPolygon(xdot_op* op,int param)
//void DrawPolygon(xdot_point* xpoint,int count, int filled)
{
	int i=0;
	int filled;
	int select=0;
	SelectPolygon(op);
	set_options(op,param);

	if(op->kind == xd_filled_polygon)
	{
		if(param==0)
			glColor4f(view.fillColor.R,view.fillColor.G,view.fillColor.B,view.fillColor.A);
		if (param==1)	//selected
			glColor4f(view.selectColor.R,view.selectColor.G,view.selectColor.B,view.selectColor.A);

		filled=1;
	}
	else
	{	
		filled=0;
		if(param==0)
			glColor4f(view.penColor.R,view.penColor.G,view.penColor.B,view.penColor.A);
		if (param==1)	//selected
			glColor4f(view.selectColor.R,view.selectColor.G,view.selectColor.B,view.selectColor.A);

	}
	glLineWidth(view.LineWidth);
	if (!filled)
		glBegin(GL_LINE_STRIP);
	else
		glBegin(GL_POLYGON);
	for (i=0;i < op->u.polygon.cnt ; i=i+1)
	{
		glVertex3f(op->u.polygon.pts[i].x-dx,op->u.polygon.pts[i].y-dy,op->u.polygon.pts[i].z);
	}	
	glVertex3f(op->u.polygon.pts[0].x-dx,op->u.polygon.pts[0].y-dy,op->u.polygon.pts[0].z);	//close the polygon
	glEnd();
}

void DrawPolyline(xdot_op* op,int param)
{
	int i=0;
	int select=0;
	if(param==0)	
		glColor4f(view.penColor.R,view.penColor.G,view.penColor.B,view.penColor.A);
	if (param==1)	//selected
		glColor4f(view.selectColor.R,view.selectColor.G,view.selectColor.B,view.selectColor.A);
	SelectPolyline(op);
	set_options(op,param);
	glLineWidth(view.LineWidth);
	glBegin(GL_LINE_STRIP);
	for (i=0;i < op->u.polyline.cnt ; i=i+1)
	{
		glVertex3f(op->u.polyline.pts[i].x-dx,op->u.polyline.pts[i].y-dy,op->u.polyline.pts[i].z);
	}	
	glEnd();
}

void DrawBitmap(GLfloat bmpX,GLfloat bmpY,GLfloat bmpW,GLfloat bmpH)
{
  if(view.texture)
	  glEnable( GL_TEXTURE_2D );
  else
	  glDisable( GL_TEXTURE_2D );
/*	glRasterPos2d(bmpX,bmpY);
	  glBitmap( bmpW,bmpH,0.0,0.0,0.0,0.0, 1); */
    glBegin( GL_QUADS );
	glTexCoord2d(bmpX,bmpY); glVertex2d(bmpX,bmpY);
	glTexCoord2d(bmpX+bmpW,bmpY); glVertex2d(bmpX+bmpW,bmpY);
	glTexCoord2d(bmpX+bmpW,bmpY+bmpH); glVertex2d(bmpX+bmpW,bmpY+bmpH);
	glTexCoord2d(bmpX,bmpY+bmpH); glVertex2d(bmpX,bmpY+bmpH);
	glEnd(); 
	glDisable( GL_TEXTURE_2D );
}

void SetFillColor(xdot_op* op,int param)
{
	RGBColor c;
	c=GetRGBColor(op->u.color);
	view.fillColor.R=c.R;
	view.fillColor.G=c.G;
	view.fillColor.B=c.B;
	view.fillColor.A=c.A;
}
void SetPenColor(xdot_op* op,int param)
{
	RGBColor c;
	c=GetRGBColor(op->u.color);
	view.penColor.R=c.R;
	view.penColor.G=c.G;
	view.penColor.B=c.B;
	view.penColor.A=c.A;
}

void SetStyle(xdot_op* op,int param)
{


}

void SetFont(xdot_op* op,int param)
{
//	view.FontName=ABSet(op->u.font.name);
	view.FontSize=op->u.font.size;
}

void InsertImage(xdot_op* op,int param)
{
	SelectImage(op);

}
void EmbedText(xdot_op* op,int param)		
{
	GLfloat x;
	SelectText(op);
	set_options(op,param);
	if (op->u.text.align == 1)
		x=op->u.text.x-op->u.text.width/2;
	if (op->u.text.align == 0)
		x=op->u.text.x;
	if (op->u.text.align == -1)
		x=op->u.text.x+op->u.text.width;
	fontSize (view.FontSize);
	if(param==0)
		fontColor (view.penColor.R,view.penColor.G,view.penColor.B);
	if (param==1)	//selected
		fontColor (view.selectColor.R,view.selectColor.G,view.selectColor.B);

	fontDrawString (x-dx,op->u.text.y-dy,op->u.text.text,op->u.text.width);
}


void draw_selection_box()
{
	if(view.mouse.mouse_down==1) 	//rectangle selection
	{
		glColor4f(view.Selection.SelectionColor.R,view.Selection.SelectionColor.G,view.Selection.SelectionColor.B,view.Selection.SelectionColor.A);
		if(view.mouse.mouse_mode==5)
		{
			glEnable(GL_LINE_STIPPLE);
			glLineStipple(1,15);
		}
		glBegin(GL_LINE_STRIP);
		glVertex3f(view.GLx,view.GLy,0.001);
		glVertex3f(view.GLx,view.GLy2,0.001);
		glVertex3f(view.GLx2,view.GLy2,0.001);
		glVertex3f(view.GLx2,view.GLy,0.001);
		glVertex3f(view.GLx,view.GLy,0.001);
		glEnd();
		if(view.mouse.mouse_mode==5)
			glDisable(GL_LINE_STIPPLE);

	}
}

void set_options(xdot_op* op,int param)
{
	
	if ((param==1) && (view.mouse.mouse_mode==10) && (view.mouse.mouse_down==1))	//selected, if there is move move it, experimental
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
void relocate_spline(xdot_op* op,int param)
{
	Agedge_t* e;
	Agnode_t* tn;	//tail node
	Agnode_t* hn;	//head node
	int i=0;
	if( AGTYPE(((xdot*)(op->parentxdot))->obj )==AGEDGE)
	{
		e=((xdot*)(op->parentxdot))->obj;
		tn=agtail(e);
		hn=aghead(e);
		if ( (((custom_object_data*)AGDATA(hn))->Selected==1)&&(((custom_object_data*)AGDATA(tn))->Selected==0))
		{
			set_options(op,1);
			for (i=1;i < op->u.bezier.cnt-1 ; i= i + 1)
			{
				if((dx != 0) || (dy != 0))
				{
					op->u.bezier.pts[i].x=op->u.bezier.pts[i].x-(dx*(float)i/(float)(op->u.bezier.cnt));
					op->u.bezier.pts[i].y=op->u.bezier.pts[i].y-(dy*(float)i/(float)(op->u.bezier.cnt));
				}
			}
			if((dx != 0) || (dy != 0))
			{
				op->u.bezier.pts[op->u.bezier.cnt-1].x=op->u.bezier.pts[op->u.bezier.cnt-1].x-dx;
				op->u.bezier.pts[op->u.bezier.cnt-1].y=op->u.bezier.pts[op->u.bezier.cnt-1].y-dy;
			}
		}
		if ( (((custom_object_data*)AGDATA(hn))->Selected==0)&&(((custom_object_data*)AGDATA(tn))->Selected==1))
		{
			set_options(op,1);
			for ( i=op->u.bezier.cnt-1; i>0 ; i= i -1 )
			{
				if((dx != 0) || (dy != 0))
				{
					op->u.bezier.pts[i].x=op->u.bezier.pts[i].x-(dx*(float)(op->u.bezier.cnt-i)/(float)(op->u.bezier.cnt));
					op->u.bezier.pts[i].y=op->u.bezier.pts[i].y-(dy*(float)(op->u.bezier.cnt-i)/(float)(op->u.bezier.cnt));
				}
			}
			if((dx != 0) || (dy != 0))
			{
				op->u.bezier.pts[0].x=op->u.bezier.pts[0].x-dx;
				op->u.bezier.pts[0].y=op->u.bezier.pts[0].y-dy;
			}
		}

		if ( (((custom_object_data*)AGDATA(hn))->Selected==1)&&(((custom_object_data*)AGDATA(tn))->Selected==1))
		{
			set_options(op,1);
			for (i=0;i < op->u.bezier.cnt ; i= i + 1)
			{
				if((dx != 0) || (dy != 0))
				{
					op->u.bezier.pts[i].x=op->u.bezier.pts[i].x-dx;
					op->u.bezier.pts[i].y=op->u.bezier.pts[i].y-dy;
				}
			}
		}
	}



}

void draw_letter(GLfloat x,GLfloat y,char c)
{
	glRasterPos2i((int)x,(int)y);
	glBitmap(10,12,0.0,0.0,0.0,0.0,rasters);
}
int font_display_list()
{
	int a;
	a=glGenLists(6);
	//draw 5 Fs
	glNewList(a,GL_COMPILE);
	draw_letter(0,0,0);
	draw_letter(0,0,0);
	draw_letter(0,0,0);
	draw_letter(0,0,0);
	draw_letter(0,0,0);
	draw_letter(0,0,0);
	glEndList();
	return a;
}
void draw_cached_letter(GLfloat x,GLfloat y,int letter)
{
/*	glPushMatrix();
	glTranslatef (view.panx+x,view.pany+y,view.zoom);
	glCallList (view.fontbase+letter-1);
	glPopMatrix(); */

}
void draw_magnifier()
{
	
	if((view.mouse.mouse_mode==MM_MAGNIFIER) && (view.mouse.mouse_down))
	{

		GLfloat mg_x,mg_y,mg_z,mg_width,mg_height;
		float a;
		int winX,winY,winW,winH;
		//converting screen pixel distaances to GL distances
		view.mg.GLwidth=GetOGLDistance(view.mg.width)/2.0;
		view.mg.GLheight=GetOGLDistance(view.mg.height)/2.0;
		GetOGLPosRef(view.mouse.mouse_X,view.mouse.mouse_Y,&mg_x,&mg_y,&mg_z);//retrieving mouse coords as GL coordinates
		view.mg.x=mg_x;
		view.mg.y=mg_y;
		glLineWidth(4);
		local_zoom(&Topview);
		//drawing the magnifier borders
		glBegin(GL_LINE_STRIP);
		glColor4f(0.3,0.1,0.8,1);
		glVertex3f(view.mg.x-view.mg.GLwidth,view.mg.y-view.mg.GLheight,0.0);
		glVertex3f(view.mg.x+view.mg.GLwidth,view.mg.y-view.mg.GLheight,0.0);
		glVertex3f(view.mg.x+view.mg.GLwidth,view.mg.y+view.mg.GLheight,0.0);
		glVertex3f(view.mg.x-view.mg.GLwidth,view.mg.y+view.mg.GLheight,0.0);
		glVertex3f(view.mg.x-view.mg.GLwidth,view.mg.y-view.mg.GLheight,0.0);
		glEnd();

		glLineWidth(1);
	}

}



void draw_fisheye_magnifier()
{
	if((view.mouse.mouse_mode==21)  && (view.mouse.mouse_down))
	{
		float a;
		GLfloat mg_x,mg_y,mg_z,mg_width,mg_height;
		a=GetOGLDistance(250);
		view.fmg.R=a;
		GetOGLPosRef(view.mouse.mouse_X,view.mouse.mouse_Y,&mg_x,&mg_y,&mg_z);
		glColor4f(0.3,0.1,0.8,1);
		if ((view.fmg.x != mg_x) || (view.fmg.y != mg_y))
		{
			fisheye_polar(mg_x, mg_y,&Topview);
			draw_circle(mg_x,mg_y,a);
		}
		view.fmg.x=mg_x;
		view.fmg.y=mg_y;

	}
}


void draw_circle(float originX,float originY,float radius)
{
/* draw a circle from a bunch of short lines */
	float vectorX1,vectorY1,vectorX,vectorY,angle;
	vectorY1=originY+radius;
	vectorX1=originX;
	glLineWidth(4);
	glBegin(GL_LINE_STRIP);			
	for(angle=0.0f;angle<=(2.1f*3.14159);angle+=0.1f)
	{		
		vectorX=originX+(radius*(float)sin((double)angle));
		vectorY=originY+(radius*(float)cos((double)angle));		
		glVertex2d(vectorX1,vectorY1);
		vectorY1=vectorY;
	vectorX1=vectorX;			
	}
	glEnd();
	glLineWidth(1);

}

int point_within_ellips_with_coords(float ex,float ey,float ea,float eb,float px,float py)
{

	float dx,dy;
	float a;
	dx = px - ex;
	dy = py - ey;
	a=(dx*dx)/(ea*ea) + (dy*dy)/(eb*eb);
	return (a <= 1);
}


