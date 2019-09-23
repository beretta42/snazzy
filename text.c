#include "snazzy.h"
#include "ll.h"

static char *buf;
static int pos = 0;           // curent cursor position
static int len = 0;           // length of buffer
static int scr = 0;           // scrolling position
static int max = 0;           // max chars this widget can display
static int bmax = 0;


/* call this from app when AEV_TEXT is sent */
void set_text(widget *w, char *appbuf, int applen) {
    buf = appbuf;
    bmax = applen;
    scr = w->y1;
    max = (w->w - 4) / 4;
    len = szy_strlen(buf);
}

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
    if (pos == len) return;
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
	if (!kwidget) do_appcall(w, AEV_TEXT);
	draw_back(w);
	ll_box(w->x, w->y, w->w, w->h);
	ll_putn(w->x+2, w->y+2, buf+scr, MIN(len-scr,max));
	if (w == kwidget) {
	    ll_vline(w->x+2 + ((pos-scr) * 4), w->y+2, 8);
	}
	break;
    case EV_CLICK:
	if (!kwidget) do_appcall(w, AEV_TEXT);
	pos = ((mx - w->x + 2 - 3) / 4) + scr;
	pos = MIN(pos,len);
	kwidget = w;
	do_event(w, EV_DRAW);
	break;
    case EV_CANCEL:
	kwidget = NULL;
	do_event(w, EV_DRAW);
	w->y1 = scr;
	do_appcall(w, AEV_SELECT);
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
