#
# /**********************************************************
# *      This software is part of the graphviz package      *
# *                http://www.graphviz.org/                 *
# *                                                         *
# *            Copyright (c) 1994-2005 AT&T Corp.           *
# *                and is licensed under the                *
# *            Common Public License, Version 1.0           *
# *                      by AT&T Corp.                      *
# *                                                         *
# *        Information and Software Systems Research        *
# *              AT&T Research, Florham Park NJ             *
# **********************************************************/

BEGIN	{ s = ARGV[1]; gsub (".*/", "", s); gsub("\\.","_",s); printf("char *%s[] = {\n",s); }
/^#/	{ print $0; next; }
		{ gsub("\\\\","\\\\",$0); printf("\"%s\",\n",$0); }
END		{ printf("(char*)0 };\n"); }
