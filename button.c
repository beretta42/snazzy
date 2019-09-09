
#include "snazzy.h"
#include "ll.h"


void do_button(widget *w, int ev) {
    switch(ev) {
    case EV_DRAW:
	draw_back(w);
	ll_box(w->x, w->y, w->w, w->h);
	ll_puts(w->x1, w->y1, gpt(w->text));
	break;
    case EV_DOWN:
	draw_back(w);
	ll_box(w->x, w->y, w->w, w->h);
	ll_puts(w->x1+1, w->y1+1, gpt(w->text));
	break;
    case EV_UP:
	do_button(w, EV_DRAW);
	break;
    case EV_CLICK:
	do_appcall(w,AEV_SELECT);
	break;
    case EV_DOUBLE:
	//fprintf(stderr,"button double clicked\n");
	break;
    }
}

