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

/* Krishnam Raju Pericherla */

#include <gtk/gtk.h>
#include "common.h"
#include "g.h"
#include "gcommon.h"
#include "mem.h"

int Gxfd;
GtkWidget *Groot;
GdkDisplay *Gdisplay;
int Gpopdownflag;
int Gscreenn;
int Gdepth;
Glazyq_t Glazyq;

PIXpoint_t *Gppp;
int Gppn, Gppi;

char *Gbufp = NULL;
int Gbufn = 0, Gbufi = 0;

Gfont_t *Gfontp;
GdkFont *deffont;
int Gfontn;

int argn;

int Ginitgraphics(void)
{

    gtk_init(0, NULL);

    Gpopdownflag = FALSE;
    Gdisplay = gdk_display_get_default();
    deffont = gdk_font_load("fixed");

    Gpopdownflag = FALSE;
    Glazyq.flag = 0;
    Gbufp = Marrayalloc((long) BUFINCR * BUFSIZE);
    Gbufn = BUFINCR;
    Gppp = Marrayalloc((long) PPINCR * PPSIZE);
    Gppn = BUFINCR;
    Gfontp = Marrayalloc((long) FONTSIZE);
    Gfontn = 1;
    Gfontp[0].name = strdup("default");
    if (!Gdefaultfont)
	Gfontp[0].font = deffont;
    else if (Gdefaultfont[0] != '\000')
	Gfontp[0].font = gdk_font_load(Gdefaultfont);
    else
	Gfontp[0].font = NULL;
    return 0;
}

int Gtermgraphics(void)
{
    int fi;

    for (fi = 0; fi < Gfontn; fi++)
	free(Gfontp[fi].name);

    Marrayfree(Gfontp), Gfontp = NULL, Gfontn = 0;
    Marrayfree(Gppp), Gppp = NULL, Gppn = 0;
    Marrayfree(Gbufp), Gbufp = NULL, Gbufn = 0;

    return 0;
}

void Gflushlazyq(void)
{

}

void Glazymanage(GtkWidget w)
{

}

int Gsync(void)
{
    if (Glazyq.flag)
	Gflushlazyq();
    gdk_display_sync(Gdisplay);
    return 0;
}

int Gresetbstate(int wi)
{
    Gcw_t *cw;
    int bn;

    cw = Gwidgets[wi].u.c;
    bn = cw->bstate[0] + cw->bstate[1] + cw->bstate[2];
    cw->bstate[0] = cw->bstate[1] = cw->bstate[2] = 0;
    cw->buttonsdown -= bn;
    Gbuttonsdown -= bn;
    return 0;
}

int Gprocessevents(int waitflag, Geventmode_t mode)
{
    int rtn;

    if (Glazyq.flag)
	Gflushlazyq();
    rtn = 0;
    switch (waitflag) {
    case TRUE:
	gtk_main_iteration();
	if (mode == G_ONEEVENT)
	    return 1;
	rtn = 1;
    case FALSE:
	while (gtk_events_pending()) {
	    gtk_main_iteration();
	    if (mode == G_ONEEVENT)
		return 1;
	    rtn = 1;
	}
	break;
    }
    return rtn;
}
