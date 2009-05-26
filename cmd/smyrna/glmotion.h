#ifndef GLMOTION_H
#define GLMOTION_H
#include "viewport.h"
void glmotion_main(ViewInfo * v,GdkEventMotion * event,GtkWidget * widget);
void glmotion_zoom(ViewInfo * v);
void glmotion_pan(ViewInfo * v);
void glmotion_rotate(ViewInfo * v);
void glmotion_adjust_pan(ViewInfo* v,float panx,float pany);
void graph_zoom(float real_zoom);
void glmotion_zoom_inc(int zoomin);
#endif