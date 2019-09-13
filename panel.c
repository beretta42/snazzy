#include "snazzy.h"
#include "ll.h"



void do_panel(widget *w, int ev) {
    switch (ev) {
    case EV_DRAW:
		ll_box(w->x,w->y,w->w, w->h);
    }
}


void select_panel(widget *w, int sel) {
    if (w->d != sel) {
	child_by_index(w, w->d)->flags |= FL_NODRAW;
	child_by_index(w, sel)->flags &= ~FL_NODRAW;
	draw_back(w);
	draw_all(w);
	w->d = sel;
    }
}
