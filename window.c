#include <stdio.h>  // fixme: remove after debug
#include <string.h> // fixme: use internal
#include "snazzy.h"
#include "ll.h"



void do_window(widget *w, int ev) {
    if (ev == EV_DRAW) {
	draw_back(w);
	ll_hline( w->x+1, w->y+1, w->w-2);
	ll_hline( w->x+1, w->y+3, w->w-2);
	ll_hline( w->x+1, w->y+5, w->w-2);
	ll_hline( w->x+1, w->y+7, w->w-2);
	ll_cset(0);
	ll_draw_back(w->x1-1, w->y1, strlen(gpt(w->text))*4+1,10);
	do_label(w, EV_DRAW);
    }
}
