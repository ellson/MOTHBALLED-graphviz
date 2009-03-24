#include "beacon.h"
#include "viewport.h"
#include "selection.h"
#include "gltemplate.h"
#include "toolboxcallbacks.h"

int pick_node(topview_node * n)
{
    static int closest_dif = 3;
	static char buf[512];
	float a, b;
    a = ABS(n->distorted_x - view->GLx);
    b = ABS(n->distorted_y - view->GLy);
    a = (float) pow((a * a + b * b), (float) 0.5);
    if (a < closest_dif) {
	if (!is_node_picked(n)) {
	    if (add_to_pick_list(n)) {
		sprintf(buf,"Clicked node name:%s\n",agnameof(n->Node));
		write_to_console(buf);
		return 1;
	    }
	    return 0;
	} else {
	    if (remove_from_pick_list(n)) {
//		printf("node has been unpicked ,name:%s\n",
//		       agnameof(n->Node));
		return 1;
	    }
	    return 0;
	}
    }
    return 0;

}

int is_node_picked(topview_node * n)
{
    int ind = 0;
    int found = 0;
    for (; ind < view->Topview->picked_node_count; ind++) {
	if ((view->Topview->picked_nodes[ind] == n) && (!found))
	    return 1;
    }
    return 0;
}

int remove_from_pick_list(topview_node * n)
{
    int ind = 0;
    int found = 0;
    for (; ind < view->Topview->picked_node_count; ind++) {
	if ((view->Topview->picked_nodes[ind] == n) && (!found))
	    found = 1;
	if ((found) && (ind < (view->Topview->picked_node_count - 1))) {
	    view->Topview->picked_nodes[ind] =
		view->Topview->picked_nodes[ind + 1];
	}
    }
    if (found) {
	view->Topview->picked_node_count--;
	view->Topview->picked_nodes =
	    realloc(view->Topview->picked_nodes,
		    sizeof(topview_node *) *
		    view->Topview->picked_node_count);
	return 1;
    }
    return 0;
}

int add_to_pick_list(topview_node * n)
{
    view->Topview->picked_node_count++;
    view->Topview->picked_nodes =
	realloc(view->Topview->picked_nodes,
		sizeof(topview_node *) * view->Topview->picked_node_count);
    view->Topview->picked_nodes[view->Topview->picked_node_count - 1] = n;
    return 1;

}




int draw_node_hint_boxes()
{
    int ind;
    /* int fs = 12; */
    for (ind = 0; ind < view->Topview->picked_node_count; ind++) 
	{
		draw_node_hintbox(view->Topview->picked_nodes[ind]->distorted_x,
			  view->Topview->picked_nodes[ind]->distorted_y,
					view->Topview->picked_nodes[ind]->distorted_z+(float)0.001,
					(GLfloat) view->FontSizeConst,
				     agnameof(view->Topview->picked_nodes[ind]->Node));
/*	fontSize(fs);
	fontColorA(0, 0, 1, 1);
	glprintf((int)
		       (view->Topview->picked_nodes[ind]->distorted_x -
			fs / 3 + 1 - fs),
		       (int) (view->Topview->picked_nodes[ind]->
			      distorted_y + fs + 1),
		       agnameof(view->Topview->picked_nodes[ind]->Node),
		       fs *
		       strlen(agnameof
			      (view->Topview->picked_nodes[ind]->Node)) /
		       2);*/
    }
    return 1;
}
