#include "snazzy.h"
#include "ll.h"



void do_panel(widget *w, int ev) {
    switch (ev) {
    case EV_DRAW:
		ll_box(w->x,w->y,w->w, w->h);
    }
}


void select_panel(widget *w, int sel) {
    widget *n;
    for (n = gp(w->child); n; n = gp(n->next)) {
	if (sel-- == 0) {
	    draw_back(w);
	    do_event(w, EV_DRAW);
	    n->flags &= ~FL_NODRAW;
	    draw_all(n);
	}
	else
	    n->flags |= FL_NODRAW;
    }
}
