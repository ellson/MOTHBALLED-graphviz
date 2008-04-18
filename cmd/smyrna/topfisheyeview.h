#ifndef TOPFISHEYEVIEW_H
#define TOPFISHEYEVIEW_H
#include "smyrnadefs.h"

void fisheye_polar(double x_focus, double y_focus, topview * t);
void fisheye_spherical(double x_focus, double y_focus,double z_focus, topview * t);
void prepare_topological_fisheye(topview * t);
void drawtopologicalfisheye(topview * t);


#endif 
