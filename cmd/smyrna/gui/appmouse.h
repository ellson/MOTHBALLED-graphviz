#ifndef APPMOUSE_H
#define APPMOUSE_H
#include "smyrnadefs.h"

extern void appmouse_left_click_down(ViewInfo* v,int x,int y);
extern void appmouse_left_click_up(ViewInfo* v,int x,int y);
extern void appmouse_left_drag(ViewInfo* v,int x,int y);
extern void appmouse_right_click_down(ViewInfo* v,int x,int y);
extern void appmouse_right_click_up(ViewInfo* v,int x,int y);
extern void appmouse_right_drag(ViewInfo* v,int x,int y);

#endif
