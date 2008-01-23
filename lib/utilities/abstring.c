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
#include "stdio.h"
#include "abstring.h"
#define MAXSTRINGLENGTH	1000
int ABLength(char* s)
{
	//length does not include the \0
	int id=0;
	while ((s[id]) && (id < MAXSTRINGLENGTH))
		id++;
	if (id==MAXSTRINGLENGTH)
		return -1;
	else
		return id;
}
char* ABSet(char* s)
{
	char* a;
	a=(char*)realloc(a, ABLength(s)+1);	
	strcpy(a,s);
}
char* ABJoin(char* s1,char* s2)
{
	char* a;
	int i=0;
	a=(char*)realloc(a, ABLength(s1)+ABLength(s2)+1);	

	a=strncat(a, s1,ABLength(s1));
	a=strncat(a, s2,ABLength(s2)+1);

}

int ABRemove(char** s1,char s2)
{
	
	int cursor,cursor2;
	char BUFFER[255];
	char* test=*s1;
	cursor=0;
	cursor2=0;
	for (cursor=0;(*s1)[cursor] != '\0' ; cursor ++)
	{
		if ((*s1)[cursor] != s2)
		{
			BUFFER[cursor2]=(*s1)[cursor];
			cursor2++;
		}
	}
	BUFFER[cursor2]='\0';
	*s1=realloc(*s1,strlen(BUFFER)+1);
	strcpy(*s1,BUFFER);

}
