#include <stdio.h> //fixme for debug
#include "snazzy.h"
#include "ll.h"

void do_menu(widget *w, int ev) {
    switch(ev) {
    case EV_DRAW:
	draw_back(w);
	ll_puts(w->x1+9, w->y1+1, gpt(w->text));
	break;
    case EV_DOWN:
	if (w->flags & FL_WIDGET) break;
	w->flags |= FL_WIDGET;
	mwidget = w;
	draw_back(w);
	ll_puts(w->x1+10, w->y1+2, gpt(w->text));
	w->flags &= ~FL_NOCHILD;
	bound(w);
	ll_draw_back(bx1-1, by1-1, bw+2, bh+2);
	draw_children(w);
	ll_box(bx1-1, by1-1, bw+2, bh+2);
	push_focus(w);
	break;
    case EV_CANCEL:
	do_event(w, EV_DRAW);
	if (! (w->flags & FL_WIDGET)) break;
	w->flags &= ~FL_WIDGET;
	w->flags &= ~FL_WIDGET2;
	pull_focus();
	break;
    case EV_CLICK:
	if (w->flags & FL_WIDGET2) {
	    do_event(w, EV_CANCEL);
	} else
	    w->flags ^= FL_WIDGET2;
	break;
    }
}

void do_menuitem(widget *w, int ev) {
    widget *m;
    switch (ev) {
    case EV_DRAW:
	draw_back(w);
	do_label(w, ev);
	break;
    case EV_DOWN:
	bounce(w);
	break;
    case EV_UP:
	m = focus;
	focus->d = w->d;
	do_event(focus, EV_CANCEL);
	do_appcall(m, AEV_SELECT);
	break;
    case EV_IN:
	do_menuitem(w, EV_DOWN);
	break;
    case EV_OUT:
	do_menuitem(w, EV_DRAW);
	break;
    }
}
