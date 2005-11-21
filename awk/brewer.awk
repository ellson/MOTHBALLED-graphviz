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
# Convert Brewer data to same RGB format used in color_names.
# See brewer_colors for input format.
#
BEGIN { 
  FS = ","
}
/^[^#]/{
  if ($1 != "") {
    name = $1 $2;
    gsub ("\"","",name);
  }
  printf ("/%s/%s %s %s %s\n", name, $5, $7, $8, $9); 
}
