
#include "snazzy.h"
#include "ll.h"


#define NULL ((void *)0)

static unsigned char icon_hslide[] = {
    0b01111000,
    0b11111100,
    0b11111100,
    0b11111100,
    0b11111100,
    0b01111000,
};


/* 

horizontal slider

*/
void do_hslide(widget *w, int ev) {
    int v,x;
    switch (ev) {
    case EV_MOVE:
	w->d = mx - 3;
    case EV_DRAW:
	draw_back(w);
	// constrain x
	w->d = MIN(w->d,w->x+w->w-6);
	w->d = MAX(w->d,w->x);
	ll_hline(w->x+3, w->y+5, w->w-6);
	ll_char_draw(w->d, w->y+2, icon_hslide);
	break;
    case EV_DOWN:
	x = mx - 3 - w->d;
	if (abs(x) > 3) {
	    v = (w->w - 6) / 10;
	    w->d += x < 0 ? -v : v;
	} else {
	    w->d = mx - 3;
	}
	mwidget = w;
	do_hslide(w, EV_DRAW);
	break;
    case EV_UP:
    case EV_CANCEL:
	do_appcall(w, AEV_SELECT);
	mwidget = NULL;
	break;
    }
}

int get_hslide(widget *w) {
    return (w->d-w->x) * w->x1 / (w->w-6);
}


