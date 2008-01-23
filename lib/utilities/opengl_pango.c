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

#ifdef WIN32
#include <pango/pangocairo.h>
#else
#define PANGO_ENABLE_BACKEND
#include <pango/pangocairo.h>
#include <pango/pango-utils.h>
#endif
const char* testPango()
{
		return pango_version_string ();
}

void drawText (char* text,char* fontName,int fontSize,float x,float y)
{
	PangoContext*      p_context;
	GList*	pangoItems;
	PangoAttrList *attrs;
	p_context=pango_context_new();
		
    pangoItems=pango_itemize(p_context,"hello world",0,10,attrs,NULL);		
	g_object_unref(p_context);

}






