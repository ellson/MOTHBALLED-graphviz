#include "glcompbutton.h"
#include "glcomptexture.h"
#include "glTexFont.h"

glCompButton *glCompButtonNew(GLfloat x, GLfloat y, GLfloat w, GLfloat h,
			      char *caption, char *glyphfile,
			      int glyphwidth, int glyphheight,glCompOrientation orientation)
{
    glCompButton *p;
    p = malloc(sizeof(glCompButton));
    p->color.R = GLCOMPSET_BUTTON_COLOR_R;
    p->color.G = GLCOMPSET_BUTTON_COLOR_G;
    p->color.B = GLCOMPSET_BUTTON_COLOR_B;
    p->color.A = GLCOMPSET_BUTTON_COLOR_ALPHA;
    p->bevel = GLCOMPSET_PANEL_BEVEL + GLCOMPSET_BEVEL_DIFF;
    p->thickness = GLCOMPSET_BUTTON_THICKNESS;
    p->caption = strdup(caption);
    p->pos.x = x;
    p->pos.y = y;
    p->width = w;
    p->height = h;
    p->enabled = 1;
    p->visible = 1;
    p->status = 0;		//0 not pressed 1 pressed;
    p->groupid = -1;
    p->fontsize = GLCOMPSET_FONT_SIZE;
    p->fontcolor.R = GLCOMPSET_BUTTON_FONT_COLOR_R;
    p->fontcolor.G = GLCOMPSET_BUTTON_FONT_COLOR_G;
    p->fontcolor.B = GLCOMPSET_BUTTON_FONT_COLOR_B;
    p->fontcolor.A = GLCOMPSET_BUTTON_FONT_COLOR_ALPHA;
    //load glyph
    if (glyphfile) {

	p->glyph =
	    glCompCreateTextureFromRaw(glyphfile, glyphwidth, glyphheight,
				       0);
	p->glyphwidth = glyphwidth;
	p->glyphheight = glyphheight;
	p->hasglyph = 1;

    } else {
	p->glyphwidth = 0;
	p->glyphheight = 0;
	p->hasglyph = 0;
    }
	p->orientation=orientation;
	p->callbackfunc = '\0';
    p->panel = '\0';
    p->customptr = '\0';
    p->font = font_init();

    return p;
}


int glCompSetAddButton(glCompSet * s, glCompButton * p)
{
    s->buttoncount++;
    s->buttons =
	realloc(s->buttons, sizeof(glCompButton *) * s->buttoncount);
    s->buttons[s->buttoncount - 1] = p;
    p->parentset = s;
    if (p->font->texId==-1)	//no font has been set yet
	copy_font((p->font),(s->font));
    return 1;
}

int glCompSetRemoveButton(glCompSet * s, glCompButton * p)
{
    int ind = 0;
    int found = 0;
    for (; ind < s->buttoncount ; ind++) {
	if ((s->buttons[ind] == p) && found == 0)
	    found = 1;
	if ((found == 1) &&(ind <= (s->buttoncount-1)))
	    s->buttons[ind] = s->buttons[ind + 1];
    }
    if (found) {
	free(p->font);
	free(p->caption);
	free(p);
	s->buttoncount--;
	s->buttons =
	    realloc(s->buttons, sizeof(glCompButton *) * s->buttoncount);

	return 1;
    }
    return 0;
}

int glCompDrawButton(glCompButton * p)
{
    float color_fac;
    float thickness = p->thickness;
    float fontx, fonty;
    if (!p->visible)
	return 0;
    if (p->panel) {
	p->pos.x = p->panel->pos.x + p->pos.x;
	p->pos.y = p->panel->pos.y + p->pos.y;
    }
    if (p->status == 1) {
	color_fac = GLCOMPSET_BUTTON_BEVEL_BRIGHTNESS;
	glColor4f(p->color.R / (GLfloat) 1.2, p->color.G / (GLfloat) 1.2,
		  p->color.B / (GLfloat) 1.2, p->color.A);
	p->thickness = p->thickness / (GLfloat) 1.2;

    }

    else {
	color_fac = 1 / GLCOMPSET_BUTTON_BEVEL_BRIGHTNESS;
	glColor4f(p->color.R, p->color.G, p->color.B, p->color.A);
	p->thickness = p->thickness * (GLfloat) 1.2;
    }
    if (!p->hasglyph) {
	glBegin(GL_POLYGON);
	glVertex3f(p->pos.x + p->thickness, p->pos.y + p->thickness,
		   p->bevel);
	glVertex3f(p->pos.x + p->width - p->thickness,
		   p->pos.y + p->thickness, p->bevel);
	glVertex3f(p->pos.x + p->width - p->thickness,
		   p->pos.y + p->height - p->thickness, p->bevel);
	glVertex3f(p->pos.x + p->thickness,
		   p->pos.y + p->height - p->thickness, p->bevel);
	glVertex3f(p->pos.x + p->thickness, p->pos.y + p->thickness,
		   p->bevel);
	glEnd();
	//buttom thickness
	glColor4f(p->color.R * color_fac, p->color.G * color_fac,
		  p->color.B * color_fac, p->color.A);
	glBegin(GL_POLYGON);
	glVertex3f(p->pos.x + p->thickness, p->pos.y + p->thickness,
		   p->bevel);
	glVertex3f(p->pos.x + p->width - p->thickness,
		   p->pos.y + p->thickness, p->bevel);
	glVertex3f(p->pos.x + p->width, p->pos.y, p->bevel);
	glVertex3f(p->pos.x, p->pos.y, p->bevel);
	glVertex3f(p->pos.x + p->thickness, p->pos.y + p->thickness,
		   p->bevel);
	glEnd();
	//left thickness
	glBegin(GL_POLYGON);
	glVertex3f(p->pos.x + p->width, p->pos.y + p->height, p->bevel);
	glVertex3f(p->pos.x + p->width - p->thickness,
		   p->pos.y + p->height - p->thickness, p->bevel);
	glVertex3f(p->pos.x + p->width - p->thickness,
		   p->pos.y + p->thickness, p->bevel);
	glVertex3f(p->pos.x + p->width, p->pos.y, p->bevel);
	glVertex3f(p->pos.x + p->width, p->pos.y + p->height, p->bevel);
	glEnd();

	glColor4f(p->color.R / color_fac, p->color.G / color_fac,
		  p->color.B / color_fac, p->color.A);
	glBegin(GL_POLYGON);
	glVertex3f(p->pos.x + p->thickness, p->pos.y + p->thickness,
		   p->bevel);
	glVertex3f(p->pos.x + p->thickness,
		   p->pos.y + p->height - p->thickness, p->bevel);
	glVertex3f(p->pos.x, p->pos.y + p->height, p->bevel);
	glVertex3f(p->pos.x, p->pos.y, p->bevel);
	glVertex3f(p->pos.x + p->thickness, p->pos.y + p->thickness,
		   p->bevel);
	glEnd();
	//left thickness
	glBegin(GL_POLYGON);
	glVertex3f(p->pos.x + p->thickness,
		   p->pos.y + p->height - p->thickness, p->bevel);
	glVertex3f(p->pos.x, p->pos.y + p->height, p->bevel);
	glVertex3f(p->pos.x + p->width, p->pos.y + p->height, p->bevel);
	glVertex3f(p->pos.x + p->width - p->thickness,
		   p->pos.y + p->height - p->thickness, p->bevel);
	glVertex3f(p->pos.x + p->thickness,
		   p->pos.y + p->height - p->thickness, p->bevel);
	glEnd();
	//draw caption
	fontx =
	    (p->width - p->thickness * (GLfloat) 2 -
	     p->fontsize * strlen(p->caption) *
	     GLCOMPSET_FONT_SIZE_FACTOR) / (GLfloat) 2.0 + p->pos.x +
	    p->thickness;
	fonty =
	    (p->height - p->thickness * (GLfloat) 2 -
	     p->fontsize) / (GLfloat) 2.0 + p->pos.y + p->thickness;
	fontSize(p->font,p->fontsize);
//              fontColorA (p->fontcolor.R,p->fontcolor.B,p->fontcolor.G,p->fontcolor.A);
	fontColorA(p->font,0, 0, 0, 1);
	fontDrawString(p->font,(GLfloat) fontx, (GLfloat) fonty,
		        (p->fontsize *(GLfloat) strlen(p->caption) *
			      GLCOMPSET_FONT_SIZE_FACTOR), p->caption);
    }
    //put glyph
    else {

	glEnable(GL_TEXTURE_2D);
	fontx =
	    (p->width - p->thickness * (GLfloat) 2 -
	     p->glyphwidth) / (GLfloat) 2.0 + p->pos.x + p->thickness;
	fonty =
	    (p->height - p->thickness * (GLfloat) 2 -
	     p->glyphheight) / (GLfloat) 2.0 + p->pos.y + p->thickness;
	glBindTexture(GL_TEXTURE_2D, p->glyph->id);
	glColor4f(1, 1, 1, 1);
	glBegin(GL_QUADS);
	glTexCoord2d(0.0f, 1.0f);
	glVertex3d(fontx, fonty, p->bevel + GLCOMPSET_BEVEL_DIFF);
	glTexCoord2d(1.0f, 1.0f);
	glVertex3d(fontx + p->glyph->w, fonty,
		   p->bevel + GLCOMPSET_BEVEL_DIFF);
	glTexCoord2d(1.0f, 0.0f);
	glVertex3d(fontx + p->glyph->w, fonty + p->glyph->h,
		   p->bevel + GLCOMPSET_BEVEL_DIFF);
	glTexCoord2d(0.0f, 0.0f);
	glVertex3d(fontx, fonty + p->glyph->h,
		   p->bevel + GLCOMPSET_BEVEL_DIFF);
//              glTexCoord2d(fontx,fonty); glVertex3d(fontx,fonty,p->bevel+GLCOMPSET_BEVEL_DIFF);

	glEnd();
	glDisable(GL_TEXTURE_2D);
	if (p->status == 1) {
	    glColor4f(p->color.R * color_fac, p->color.G * color_fac,
		      p->color.B * color_fac, p->color.A / 2);
	    glBegin(GL_POLYGON);
	    glVertex3d(fontx - p->thickness, fonty - p->thickness,
		       p->bevel + GLCOMPSET_BEVEL_DIFF * 2);
	    glVertex3d(fontx + p->glyph->w + p->thickness,
		       fonty - p->thickness,
		       p->bevel + GLCOMPSET_BEVEL_DIFF * 2);
	    glVertex3d(fontx + p->glyph->w + p->thickness,
		       fonty + p->glyph->h + p->thickness,
		       p->bevel + GLCOMPSET_BEVEL_DIFF * 2);
	    glVertex3d(fontx - p->thickness,
		       fonty + p->glyph->h + p->thickness,
		       p->bevel + GLCOMPSET_BEVEL_DIFF * 2);
	    glEnd();

	}

    }
    p->thickness = thickness;
    if (p->panel) {
	p->pos.x = p->pos.x - p->panel->pos.x;
	p->pos.y = p->pos.y - p->panel->pos.y;
    }

    return 1;


}
void glCompButtonClick(glCompButton * p)
{
    int ind;
    if (p->groupid > 0) {
	for (ind = 0; ind < ((glCompSet *) p->parentset)->buttoncount;
	     ind++) {
	    if (((glCompSet *) p->parentset)->buttons[ind]->groupid ==
		p->groupid)
		((glCompSet *) p->parentset)->buttons[ind]->status = 0;
	}
	p->status = 1;
    } else {
	if (p->groupid == -1) {
	    if (p->status == 0)
		p->status = 1;
	    else
		p->status = 0;
	} else
	    p->status = 0;
    }
    if (p->callbackfunc)
	p->callbackfunc(p);


}


