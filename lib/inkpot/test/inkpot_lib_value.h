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

unsigned long TAB_VALUES_64[SZT_VALUES_64] = {
    0x000000000000ffff,  /* black */
    0x00000000ffffffff,  /* bleu, blue */
    0x0000ffff0000ffff,  /* green, vert */
    0xffff00000000ffff,  /* red, rouge */
    0xffffcf000000ffff,  /* yellow (svg) */
    0xffffffff0000ffff,  /* yellow (x11), jaune */
    0xffffffffffffffff,  /* white */
}; 

unsigned long TAB_NONAME_VALUES_64[SZT_NONAME_VALUES_64] = {
    0x000000008000ffff,
    0x00000000a400ffff,
    0x00000000c800ffff,
    0x00000000ea00ffff,
}; 

unsigned char TAB_VALUES_24[SZT_VALUES_24] = {
      0,   0,   0,  /* black */
      0,   0, 255,  /* bleu, blue */
      0, 255,   0,  /* green, vert */
    255,   0,   0,  /* red, rouge */
    255, 192,   0,  /* yellow (svg) */
    255, 255, 255,  /* yellow (x11), jaune */
    255, 255, 255,  /* white */
}; 

unsigned char TAB_NONAME_VALUES_24[SZT_NONAME_VALUES_24] = {
      0,   0, 128,
      0,   0, 160,
      0,   0, 192,
      0,   0, 224,
}; 
