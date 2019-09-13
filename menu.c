#include <stdio.h> //fixme for debug
#include "snazzy.h"
#include "ll.h"

widget *menu;

static void do_foo(widget *w) {
    w->flags |= FL_NOCHILD;
    bound(w);
    ll_draw_back(bx1-1, by1-1, bw+2, bh+2);
    pull_focus();
    draw_coll(focus);
    menu = NULL;
}

void do_menu(widget *w, int ev) {
    switch(ev) {
    case EV_DRAW:
	draw_back(w);
	ll_puts(w->x1+9, w->y1+1, gpt(w->text));
	break;
    case EV_DOWN:
	mwidget = w;
	draw_back(w);
	ll_puts(w->x1+10, w->y1+2, gpt(w->text));
	w->flags &= ~FL_NOCHILD;
	bound(w);
	ll_draw_back(bx1-1, by1-1, bw+2, bh+2);
	menu = w;
	draw_children(w);
	ll_box(bx1-1, by1-1, bw+2, bh+2);
	push_focus(w);
	break;
    case EV_OUT:
	if (!collide_all(focus,mx,my))
	    do_foo(w);
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
	m = menu;
	menu->d = w->d;
	do_foo(menu);
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
