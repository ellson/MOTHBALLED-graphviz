/*************************************************************************
 * Copyright (c) 2014 AT&T Intellectual Property 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: See CVS logs. Details at http://www.graphviz.org/
 *************************************************************************/
#ifndef LAB_H
#define LAB_H

struct rgb_struct {
  double r, g, b;/* 0 to 255 */
};
typedef struct rgb_struct color_rgb;

struct xyz_struct {
  double x, y, z;
};
typedef struct xyz_struct color_xyz;

struct lab_struct {
  signed char l, a, b;/* l: 0 to 100, a,b: -128 tp 128 */
};
typedef struct lab_struct color_lab;

color_xyz RGB2XYZ(color_rgb color);
color_rgb XYZ2RGB(color_xyz color);
color_lab RGB2LAB(color_rgb color);
void LAB2RGB_real_01(real *color);  /* convert an array[3] of LAB colors to RGB between 0 to 1, in place */
color_rgb LAB2RGB(color_lab color);
color_rgb color_rgb_init(double r, double g, double b);
color_xyz color_xyz_init(double x, double y, double z);
color_lab color_lab_init(double l, double a, double b);
QuadTree lab_gamut_quadtree(char *gamut_file, const char *lightness, int max_qtree_level); /* construct a quadtree of the LAB gamut points */
double *lab_gamut_from_file(char *gamut_file, const char *lightness, int *n);  /* give a list of n points  in the file defining the LAB color gamut */
double *lab_gamut(const char *lightness, int *n);  /* give a list of n points  in the file defining the LAB color gamut */
void color_blend_rgb2lab(char *color_list, const int maxpoints, double **colors); /* give a color list of the form "#ff0000,#00ff00,...", get a list of around maxpoints
										     colors in an array colors0 of size [maxpoints*3] of the form {{l,a,b},...}.
										     If *colors0 is NULL, it will be allocated. */
 
color_rgb color_blend_rgb(char *color_list, const real ratio, int *flag);/* blend a color list to get one color at ratio*100 percent of the list */
void color_blend_rgbstring(char *color_list, const real ratio, char **color0, int *flag);/* blend a color list to get one color at ratio*100 percent of the list */

#endif
