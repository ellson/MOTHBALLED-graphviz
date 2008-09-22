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
# 
# Convert SVG-1.1 color data to same RGBA format used in color_names.
# See svgcolor_names for input format.
#
# All colors assumed opaque, so A = 255 in all colors
BEGIN {
	FS = "[ ,()]*";
}
/^[ 	]*$/    { next; }
/^#/    { next; }
{
	printf ("/svg/%s %s %s %s 255\n", $1, $5, $6, $7);
}
