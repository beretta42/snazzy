#include <stdio.h> //fixme for debug
#include "snazzy.h"
#include "ll.h"

widget *menu;
widget *foo;
void do_menu(widget *w, int ev) {
    switch(ev) {
    case EV_DRAW:
	draw_back(w);
	ll_puts(w->x1+9, w->y1+1, gpt(w->text));
	break;
    case EV_DOWN:
	foo = NULL;
	mwidget = w;
	draw_back(w);
	ll_puts(w->x1+10, w->y1+2, gpt(w->text));
	w->flags ^= FL_NOCHILD;
	bound(w);
	ll_draw_back(bx1-1, by1-1, bx2-bx1+2, by2-by1+2);
	menu = w;
	draw_children(w);
	ll_box(bx1-1, by1-1, bx2-bx1+2, by2-by1+2);
	push_focus(w);
	break;
    case EV_UP:
	w->flags ^= FL_NOCHILD;
	bound(w);
	ll_draw_back(bx1-1, by1-1, bx2-bx1+2, by2-by1+2);
	pull_focus();
	draw_coll(focus);
	do_menu(w, EV_DRAW);
	mwidget = NULL;
	break;
    case EV_CLICK:
	if (w->flags & FL_NOCHILD) {
	    do_menu(w, EV_DOWN);
	}
	else {
	    down = NULL;
	    do_menu(w, EV_UP);
	}
	break;
    }
}

void do_menuitem(widget *w, int ev) {
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
	menu->d = w->d;
	do_menu(menu, EV_UP);
	foo = NULL;
	do_appcall(menu, AEV_SELECT);
	break;
    case EV_MOVE:
	if (foo != w) {
	    if (foo) do_menuitem(foo,EV_DRAW);
	    do_menuitem(w, EV_DOWN);
	}
	foo = w;
	break;
    }
}
