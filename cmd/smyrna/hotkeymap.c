
#include "hotkeymap.h"
int get_mode(ViewInfo* v)
{
/*#define MM_PAN					0
#define MM_ZOOM					1
#define MM_ROTATE				2
#define MM_SINGLE_SELECT		3
#define MM_RECTANGULAR_SELECT	4
#define MM_RECTANGULAR_X_SELECT	5
#define MM_MOVE					10
#define MM_MAGNIFIER			20
#define MM_FISHEYE_MAGNIFIER	21*/


/*#define F_BUTTON1   B_LSHIFT
#define F_BUTTON2   B_RSHIFT
#define F_BUTTON3   B_LCTRL
#define F_BUTTON4   B_LCTRL

#define MOUSE_BUTTON_1	LEFT_MOUSE_BUTTON
#define MOUSE_BUTTON_2  RIGHT_MOUSE_BUTTON
#define MOUSE_BUTTON_3	MIDDLE_MOUSE_BUTTON*/


    if ((view->mouse.t==MOUSE_BUTTON_1)&&(view->keymap.down) && (view->keymap.keyVal ==F_BUTTON1) && (view->active_camera==-1))
	return MM_FISHEYE_MAGNIFIER;
    if ((view->mouse.t==MOUSE_BUTTON_1)&&(view->keymap.down) && (view->keymap.keyVal == F_BUTTON1) && (view->active_camera>-1))
	return MM_ROTATE;
    if ((view->mouse.t==MOUSE_BUTTON_1)&&(view->keymap.down) && (view->keymap.keyVal == F_BUTTON3)) 
	return MM_MOVE;
    if ((view->mouse.t==MOUSE_BUTTON_1)&&(view->mouse.down) ) 
	return MM_PAN;
    if ((view->mouse.t==MOUSE_BUTTON_2)&&(view->mouse.down) ) 
	return MM_RECTANGULAR_SELECT;

    return 0;
}
