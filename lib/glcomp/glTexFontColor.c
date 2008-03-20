/*
    Copyright (c) 1999 Nate Miller
    
    Notice: Usage of any code in this file is subject to the rules
    described in the LICENSE.TXT file included in this directory.
    Reading, compiling, or otherwise using this code constitutes
    automatic acceptance of the rules in said text file.

    File        -- glTexFontColor.c
    Date        -- 6/10/99
    Author      -- Nate 'm|d' Miller
    Contact     -- vandals1@home.com
    Web         -- http://members.home.com/vandals1
*/
#include "glTexFont.h"

/* color functions must be defined in texFont.h */

extern texFont_t font;

/*
=============
fontColor, fontColorA, fontColorp, fontColorAp
   
Sets the currect color for the text. 
=============
*/
void fontColor (float r, float g, float b)
{
    font.fgColor[0] = r;
    font.fgColor[1] = g;
    font.fgColor[2] = b;
    font.fgColor[3] = 1.0;
}
void fontColorA (float r, float g, float b, float a)
{
    font.fgColor[0] = r;
    font.fgColor[1] = g;
    font.fgColor[2] = b;
    font.fgColor[3] = a;
}
void fontColorp (float *clr)
{
    font.fgColor[0] = clr[0];
    font.fgColor[1] = clr[1];
    font.fgColor[2] = clr[2];
    font.fgColor[3] = 1.0;
}
void fontColorAp (float *clr)
{
    font.fgColor[0] = clr[0];
    font.fgColor[1] = clr[1];
    font.fgColor[2] = clr[2];
    font.fgColor[3] = clr[3];
}
/*
=============
fontShadow, fontShadowA, fontShadowp, fontShadowAp
   
Sets the currect shadow color for the text. 
=============
*/
void fontShadowColor (float r, float g, float b)
{
    font.bgColor[0] = r;
    font.bgColor[1] = g;
    font.bgColor[2] = b;
    font.bgColor[3] = 1.0;
}
void fontShadowColorA (float r, float g, float b, float a)
{
    font.bgColor[0] = r;
    font.bgColor[1] = g;
    font.bgColor[2] = b;
    font.bgColor[3] = a;
}
void fontShadowColorp (float *clr)
{
    font.bgColor[0] = clr[0];
    font.bgColor[1] = clr[1];
    font.bgColor[2] = clr[2];
    font.bgColor[3] = 1.0;
}
void fontShadowColorAp (float *clr)
{
    font.bgColor[0] = clr[0];
    font.bgColor[1] = clr[1];
    font.bgColor[2] = clr[2];
    font.bgColor[3] = clr[3];
}
/*
=============
fontGradientColor, fontGradientColorA, fontGradientColorp, fontGradientColorAp
   
Sets the currect gradient color for the text. 
=============
*/
void fontGradientColor (float r, float g, float b)
{
    font.gdColor[0] = r;
    font.gdColor[1] = g;
    font.gdColor[2] = b;
    font.gdColor[3] = 1.0;
}
void fontGradientColorA (float r, float g, float b, float a)
{
    font.gdColor[0] = r;
    font.gdColor[1] = g;
    font.gdColor[2] = b;
    font.gdColor[3] = a;
}
void fontGradientColorp (float *clr)
{
    font.gdColor[0] = clr[0];
    font.gdColor[1] = clr[1];
    font.gdColor[2] = clr[2];
    font.gdColor[3] = 1.0;
}
void fontGradientColorAp (float *clr)
{
    font.gdColor[0] = clr[0];
    font.gdColor[1] = clr[1];
    font.gdColor[2] = clr[2];
    font.gdColor[3] = clr[3];
}
