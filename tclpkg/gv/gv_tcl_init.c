/* $Id$ $Revision$ */
/* vim:set shiftwidth=4 ts=8: */

/**********************************************************
*      This software is part of the graphviz package      *
*                http://www.graphviz.org/                 *
*                                                         *
*            Copyright (c) 1994-2008 AT&T Corp.           *
*                and is licensed under the                *
*            Common Public License, Version 1.0           *
*                      by AT&T Corp.                      *
*                                                         *
*        Information and Software Systems Research        *
*              AT&T Research, Florham Park NJ             *
**********************************************************/

#include <tcl.h>
#include "gvc.h"
#include "gvplugin.h"
#include "gvcjob.h"
#include "gvcint.h"

static size_t gv_string_writer(GVJ_t *job, const char *s, int len)
{
    Tcl_AppendToObj((Tcl_Obj*)(job->output_file), s, len);
    return len;
}

static size_t gv_channel_writer(GVJ_t *job, const char *s, int len)
{
    return Tcl_Write((Tcl_Channel)(job->output_file), s, len);
}

void gv_string_writer_init(GVC_t *gvc)
{
    gvc->write_fn = gv_string_writer;
}

void gv_channel_writer_init(GVC_t *gvc)
{
    gvc->write_fn = gv_channel_writer;
}
