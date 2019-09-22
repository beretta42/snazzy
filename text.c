#include <stdio.h>
#include "snazzy.h"
#include "ll.h"

#define BUFMAX 256
static char buf[BUFMAX + 1];
static int pos = 0;           // curent cursor position
static int len = 0;           // length of buffer
static int scr = 0;           // scrolling position
static int max = 0;           // max chars this widget can display
static int bmax = BUFMAX;

static void fix(void) {
    if (pos-scr > max) {
	scr++;
    }
    if (pos-scr < 0) {
	scr--;
    }
}

static void insert(char k) {
    int i;
    if (len == bmax) return; //fixme: bell here?
    for (i = len; i >= pos; i--) {
	buf[i + 1] = buf[i];
    }
    buf[pos++] = k;
    len++;
    fix();
}

static void del() {
    int i;
    if (pos == bmax) return;
    if (!len) return;
    for (i = pos; i <= len; i++) {
	buf[i] = buf[i + 1];
    }
    len--;
    fix();
}

static void left() {
    if (pos) pos--;
    fix();
}

static void right() {
    if (pos < len) pos++;
    fix();
}

static void bs() {
    if (pos) {
	pos--;
	del();
    }
}

void do_text(widget *w, int ev) {
    switch (ev) {
    case EV_DRAW:
	draw_back(w);
	ll_box(w->x, w->y, w->w, w->h);
	ll_putn(w->x+2, w->y+2, buf+scr, MIN(len-scr,max));
	if (w == kwidget) {
	    ll_vline(w->x+2 + ((pos-scr) * 4), w->y+2, 8);
	}
	break;
    case EV_CLICK:
	/* get buffer from app here */
	pos = ((mx- w->x + 2 - 3) / 4) + scr;
	pos = MIN(pos,len);
	kwidget = w;
	max = (w->w - 4) / 4;
	do_event(w, EV_DRAW);
	break;
    case EV_CANCEL:
	fprintf(stderr,"text canceled\n");
	kwidget = NULL;
	do_event(w, EV_DRAW);
	/* save to app here */
	break;
    case EV_KEY:
	switch (key) {
	case KEY_DEL: del(); break;
	case KEY_BS:  bs(); break;
	case KEY_LEFT: left(); break;
	case KEY_RIGHT: right(); break;
	case KEY_RET: do_event(w, EV_CANCEL); break;
	default: insert(key);
	}
	do_event(w, EV_DRAW);
	break;
    }
}
