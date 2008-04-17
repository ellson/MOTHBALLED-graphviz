#ifndef GLMOTION_H
#define GLMOTION_H
#include "viewport.h"
int glmotion_main(ViewInfo * v,GdkEventMotion * event,GtkWidget * widget);
int glmotion_zoom(ViewInfo * v);
int glmotion_pan(ViewInfo * v);
int glmotion_rotate(ViewInfo * v);
int glmotion_adjust_pan(ViewInfo* v,float panx,float pany);
#endif