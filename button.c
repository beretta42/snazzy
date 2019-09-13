
#include "snazzy.h"
#include "ll.h"


void do_button(widget *w, int ev) {
    switch(ev) {
    case EV_DRAW:
	draw_back(w);
	do_label(w, EV_DRAW);
	ll_box(w->x, w->y, w->w, w->h);
	break;
    case EV_DOWN:
	bounce(w);
	ll_box(w->x, w->y, w->w, w->h);
	break;
    case EV_UP:
	do_button(w, EV_DRAW);
	break;
    case EV_CLICK:
	do_appcall(w,AEV_SELECT);
	break;
    }
}

