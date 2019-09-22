#include <stdio.h>
#include "snazzy.h"
#include "ll.h"

#define BUFMAX 256
static char buf[BUFMAX + 1]="one";
static int pos = 1;
static int len = 3;

static void insert(char k) {
    int i;
    if (len == BUFMAX) return; //fixme: bell here?
    for (i = len; i >= pos; i--) {
	buf[i + 1] = buf[i];
    }
    buf[pos++] = k;
    len++;
}

static void del() {
    int i;
    if (pos == BUFMAX) return;
    for (i = pos; i <= len; i++) {
	buf[i] = buf[i + 1];
    }
    len--;
}

static void left() {
    if (pos) pos--;
}

static void right() {
    if (pos < len) pos++;
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
	ll_puts(w->x+2, w->y+2, buf);
	ll_vline(w->x+2 + (pos * 4), w->y+2, 8);
	break;
    case EV_CLICK:
	fprintf(stderr,"text clicked\n");
	pos = (mx- w->x + 2 - 3) / 4;
	pos = MIN(pos,len);
	kwidget = w;
	do_event(w, EV_DRAW);
	break;
    case EV_CANCEL:
	fprintf(stderr,"text canceled\n");
	break;
    case EV_KEY:
	fprintf(stderr,"%d key pressed\n", key);
	switch (key) {
	case KEY_DEL: del(); break;
	case KEY_BS:  bs(); break;
	case KEY_LEFT: left(); break;
	case KEY_RIGHT: right(); break;
	default: insert(key);
	}
	do_event(w, EV_DRAW);
	break;
    }
}
