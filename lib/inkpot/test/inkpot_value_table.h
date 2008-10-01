/* $Id$ $Revision$ */
/* vim:set shiftwidth=4 ts=8: */

/***********************************************************
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
