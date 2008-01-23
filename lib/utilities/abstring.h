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

//string utilities
//supports memory allocation
//this library works with \0 terminated strings

#include "viewport.h"
int ABLength(char* s);
char* ABSet(char* s);
char* ABJoin(char* s1,char* s2);
int ABRemove(char** s1,char s2);
//char* ABTrim(char* s);
//char* ABRemove(char* s,char* param)



//char* ABSubString(char* s,int a,int b);
//char* ABUpperCase(char* s);
//char* ABLowerCase(char* s);

