#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* 

   Snazzy - a GUI 


*/


/**************** 
   Widgets
*****************/

#include "snazzy.h"
#include "ll.h"   // fixme: remove

uint8_t databuffer[8192];
appcall_ptr app_tab[256];
int aindex = 1;
int bx1;
int by1;
int bx2;
int by2;
int bw;
int bh;
widget *focus;
widget *mwidget;
widget *lmove = NULL;
widget *down;
widget *wstack[16];
int wstack_ptr = 0;
int mx;
int my;
int drawf;
widget *clicked;
int time;
int dtime = 500;



static int swizzle(uint8_t *s) {
    return s[0] * 0x100 + s[1];
}

widget *find_widget(char *id) {
    uint8_t *s = gpt(*((uint16_t *)databuffer));
    while (*s) {
	if (!strcmp(id,s)) {
	    return gp(swizzle(s + strlen(s) + 1));
	}
	s += strlen(s) + 3;
    }
    return NULL;
}

int reg_appcall(char *id, appcall_ptr m) {
    widget *w;
    w = find_widget(id);
    if (w == NULL) return -1;
    w->app = aindex;
    app_tab[aindex++] = m;
    return 0;
}

void do_appcall(widget *w, int ev) {
    if (w->app)
	app_tab[w->app](w,ev);
}

// fixme: these need stack checking
void push_focus(widget *w) {
    wstack[wstack_ptr++] = focus;
    focus = w;
}

void pull_focus(void) {
    if (wstack_ptr < 1) {
	fprintf(stderr,"focus stack exhausted\n");
	exit(1);
    }
    focus->flags |= FL_NOCHILD;
    bound(focus);
    ll_draw_back(bx1-1, by1-1, bw+2, bh+2);
    focus = wstack[--wstack_ptr];
    draw_coll(focus);
    lmove = NULL;
    down = NULL;
    mwidget = NULL;
    clicked = NULL;
}

void draw_back(widget *w) {
    ll_draw_back(w->x, w->y, w->w, w->h);
}


void bounce(widget *w) {
    draw_back(w);
    ll_puts(w->x1+1, w->y1+1, gpt(w->text));
}

/* a dummy no-op for widgets that don't need a runtime
   event handler */
static void do_noop(widget *w, int ev) {
}


typedef void (*DOPTR)(widget *w, int ev);
void do_event(widget *w, int ev) {
    static DOPTR tab[TY_MAX] = {
	do_noop,    /*  vbox and hbox down have a runtine */
	do_noop,
	do_label,
	do_button,
	do_poplist,
	do_popitem,
	do_hslide,
	do_panel,
	do_menu,
	do_menuitem,
	do_window,
	};
    if (w == NULL) return;
    // fixme: check for out of bounds here
    tab[w->type](w, ev);
}

widget *gp(int o) {
    if (o == 0) return (widget *)NULL;
    return (widget *)(databuffer + o);
}

char *gpt(int o) {
    return (char *)gp(o);
}
    
void bound(widget *w) {
    widget *n = gp(w->child);
    bx1 = n->x;
    by1 = n->y;
    bx2 = n->x + n->w;
    by2 = n->y + n->h;
    for (; n; n = gp(n->next)) {
	bx1 = MIN(bx1, n->x);
	by1 = MIN(by1, n->y);
	bx2 = MAX(bx2, n->x + n->w);
	by2 = MAX(by2, n->y + n->h);
    }
    bw = bx2 - bx1;
    bh = by2 - by1;
}


void draw_coll(widget *w) {
    widget *n;
    if (w->flags & FL_NODRAW) return;
    if ( w->x > bx2 ||
    	 w->y > by2 ||
    	 w->x + w->w < bx1 ||
    	 w->y + w->h < by1 )
    	return;
    drawf = 1;
    do_event(w, EV_DRAW);
    if (w->flags & FL_NOCHILD) return;
    for (n = gp(w->child); n; n = gp(n->next)) {
	draw_coll(n);
    }
}

void draw_children(widget *w) {
    widget *n;
    for (n = gp(w->child); n; n = gp(n->next)) {
	draw_all(n);
    }
}

void draw_all(widget *w) {
    widget *n;
    if (w->flags & FL_NODRAW) return;
    drawf = 1;
    do_event(w, EV_DRAW);
    if (w->flags & FL_NOCHILD) return;
    for (n = gp(w->child); n; n = gp(n->next)) {
	draw_all(n);
    }
}

static int collide(widget *w, int x, int y) {
    if ( x < w->x ||
	 x > w->x + w->w ||
	 y < w->y ||
	 y > w->y + w->h )
	return 0 ;
    return 1;
}

widget *collide_all(widget *head, int x, int y) {
    widget *n;
    widget *t;
    if (head == NULL) return NULL;
    for (n = gp(head->child); n; n = gp(n->next)) {
	if (collide(n, x, y)) {
	    if (n->flags & FL_CLICKABLE) {
		return n;
	    }
	    else if ((n->flags & FL_NODRAW)==0) {
		t = collide_all(n, x, y);
		if (t) return t;
	    }
	}
    }
    if (collide(head,x,y)) return head;
    return NULL;
}

widget *child_by_index(widget *w, int i) {
    widget *n = gp(w->child);
    while (i-- && n) n = gp(n->next);
    return n;
}

/* called to send event to widget system 
     w = root widget
     e = input event
     x,y = coordinates of input event (if any)
*/
void send_uevent(int e, int x, int y) {
    widget *n = collide_all(focus, x, y);
    mx = x; my = y;
    switch (e) {
	// we only get MOVE events if the ll.c allows
	// but if we do track and send IN and OUT events to widgets
    case UEV_MOVE:
	if (n != lmove) {
	    do_event(n, EV_IN);
	    do_event(lmove, EV_OUT);
	    lmove = n;
	}
	else
	    do_event(n, EV_MOVE);
	break;
    case UEV_DOWN:
	do_event(n, EV_DOWN);
	down = n;
	// if we've click down off the form, then pull form stack
	if (!n)
	    do_event(focus, EV_CANCEL);
	    //pull_focus();
	break;
    case UEV_UP:
	/* send widget UP event, if it was the last widget pushed down,
	   then send a CLICK event,  if this was the second click
	   in a short time, also send DOUBLE
	*/
	do_event(n, EV_UP);
	if (n != down) {
	    do_event(down, EV_CANCEL);
	}
	else {
	    do_event(n, EV_CLICK);
	    if (n == clicked && (ll_getticks() - time) < dtime) {
		do_event(n, EV_DOUBLE);
		clicked = NULL;
	    }
	    else {
		clicked = n;
		time = ll_getticks();
	    }
	}
	break;
    }
}


// fixme: don't call me if target is BIG ENDIAN
void fixup(widget *w) {
    widget *n;
    uint8_t *s = (uint8_t *)w;
    int x;
    uint8_t t;
    for (x = 0; x < sizeof(widget); x +=2 ) {
	t = s[x];
	s[x] = s[x+1];
	s[x+1] = t;
    }
    for( n = gp(w->child); n; n = gp(n->next)) {
	fixup(n);
    }
}

