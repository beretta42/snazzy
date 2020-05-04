/* 

   A bitmap widget

*/

#include "ll.h"
#include "snazzy.h"


void do_bitmap(widget *w, int ev) {
    if (ev == EV_DRAW) {
	ll_cset(1);
	ll_box(w->x, w->y, w->w, w->h);
	ll_blit(w->x1, w->y1, gpt(w->text));
    }
}
