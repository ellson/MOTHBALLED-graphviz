#ifndef GLEXPOSE_H
#define GLEXPOSE_H

#include "viewport.h"
int glupdatecamera(ViewInfo* v);
int glexpose_main(ViewInfo* v);
void glexpose_grid(ViewInfo* v);
int glexpose_drawgraph(ViewInfo* view);

#endif
