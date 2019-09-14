#include <stdio.h>
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
    do_menu(w, ev);
    switch (ev) {
    case EV_DRAW:
	ll_char_draw(w->x1+1, w->y1+1, poplist_up);
	break;
    case EV_DOWN:
	ll_char_draw(w->x1+2, w->y1+2, poplist_up);
	break;
    }
}

/*
  Pop Item Widget 

*/
void do_popitem(widget *w, int ev){
    if (ev == EV_UP)
	focus->text = w->text;
    do_menuitem(w, ev);
}

