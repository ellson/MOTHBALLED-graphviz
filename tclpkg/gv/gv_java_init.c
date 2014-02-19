/* $Id$ $Revision$ */
/* vim:set shiftwidth=4 ts=8: */

/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: See CVS logs. Details at http://www.graphviz.org/
 *************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "gvc.h"
#include "gvplugin.h"
#include "gvcjob.h"
#include "gvcint.h"

typedef struct {
    char* data;
    int sz;       /* buffer size */
    int len;      /* length of array */
} BA;

static size_t gv_string_writer(GVJ_t *job, const char *s, size_t len)
{
    BA* bap = (BA*)(job->output_file);
/* fprintf (stderr, "newlen %ld data %p sz %d len %d\n", len, bap->data,bap->sz, bap->len); */
    int newlen = bap->len + len;
    if (newlen > bap->sz) {
	bap->sz *= 2;
	if (newlen > bap->sz)
	    bap->sz = 2*newlen;
        bap->data = (char*)realloc(bap->data, bap->sz);
    }
    memcpy (bap->data+bap->len, s, len); 
    bap->len = newlen;
    return len;
}

void gv_string_writer_init(GVC_t *gvc)
{
    gvc->write_fn = gv_string_writer;
}

static size_t gv_channel_writer(GVJ_t *job, const char *s, size_t len)
{
    return len;
}

void gv_channel_writer_init(GVC_t *gvc)
{
    gvc->write_fn = gv_channel_writer;
}

void gv_writer_reset (GVC_t *gvc) {gvc->write_fn = NULL;}
