#include "snazzy.h"
#include "ll.h"



static unsigned char poplist_up[] = {
    0b00000000,
    0b11111110,
    0b01000100,
    0b00101000,
    0b00010000,
    0b00000000,
};

widget *poplist;
void do_poplist(widget *w, int ev) {
    switch (ev) {
    case EV_DRAW:
	draw_back(w);
	ll_char_draw(w->x1+1, w->y1+1, poplist_up);
	ll_puts(w->x1+9, w->y1+1, gpt(w->text));
	break;
    case EV_DOWN:
	draw_back(w);
	ll_char_draw(w->x1+2, w->y1+2, poplist_up);
	ll_puts(w->x1+10, w->y1+2, gpt(w->text));
	break;
    case EV_UP:
	do_poplist(w, EV_DRAW);
	break;
    case EV_CLICK:
	w->flags ^= FL_NOCHILD;
	bound(w);
	ll_draw_back(bx1-1, by1-1, bx2-bx1+2, by2-by1+2);
	if (w->flags & FL_NOCHILD) {
	    pull_focus();
	    draw_coll(focus);
	}
	else {
	    poplist = w;
	    draw_all(w);
	    ll_box(bx1-1, by1-1, bx2-bx1+2, by2-by1+2);
	    push_focus(w);
	}
	break;
    }
}


/*
  Pop Item Widget 

*/

void do_popitem(widget *w, int ev){
    switch (ev) {
    case EV_DRAW:
	draw_back(w);
	do_label(w, ev);
	break;
    case EV_DOWN:
	draw_back(w);
	ll_puts(w->x1+1, w->y1+1, gpt(w->text));
	break;
    case EV_UP:
	do_popitem(w, EV_DRAW);
	break;
    case EV_CLICK:
        poplist->text = w->text;
	poplist->d = w->d;
	do_poplist(poplist, ev);
	do_appcall(poplist, AEV_SELECT);
	break;
    }
}
