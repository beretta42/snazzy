#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/* 

   Snazzy - a GUI 


*/

/* 
   Lowlevel graphics routines

*/

#include <SDL2/SDL.h>
#include "ll.h"

SDL_Window *win = NULL;
SDL_Surface *screen = NULL;
int pen = 0;

int ll_init(void) {
    int ret;

    ret = SDL_Init(SDL_INIT_VIDEO);
    win = SDL_CreateWindow("A Fancy Title",
			   SDL_WINDOWPOS_UNDEFINED,
			   SDL_WINDOWPOS_UNDEFINED,
			   512, 384,
			   SDL_WINDOW_SHOWN);
    screen = SDL_GetWindowSurface(win);
    SDL_UpdateWindowSurface(win);
}

void ll_cset(int color){
    pen = color;
}

void ll_deinit(void) {
    SDL_DestroyWindow(win);
    SDL_Quit();
}


void ll_clear(void){
    if (pen) {
	SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0,255,0));
    }
    else {
	SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0,0,0));
    }
}

void ll_setpixel(int x, int y) {
    SDL_Rect r;
    r.x = x * 2;
    r.y = y * 2;
    r.w = 2;
    r.h = 2;
    if (pen)
	SDL_FillRect(screen, &r, SDL_MapRGB(screen->format, 0, 255, 0));
    else
	SDL_FillRect(screen, &r, SDL_MapRGB(screen->format, 0, 0, 0));
}


void ll_hline(int x, int y, int w){
    while(w--) {
	ll_setpixel(x++,y);
    }
}

void ll_vline(int x, int y, int h){
    while(h--) {
	ll_setpixel(x,y++);
    }
}

void ll_bar(int x, int y, int w, int h){
    while(h--){
	ll_hline(x, y++, w);
    }
}

void ll_box(int x, int y, int w, int h){
    ll_hline(x, y, w);
    ll_hline(x, y+h-1, w);
    ll_vline(x, y, h);
    ll_vline(x+w-1, y, h);

}

void ll_char_draw(int x, int y, unsigned char *p){
    int i,j;
    unsigned char c;
    int v;
    
    for(i = 0; i < 6; i++){
	c = *p++;
	v = x;
	for(j = 0; j < 8; j++){
	    if(c & 0x80)
		ll_setpixel(v,y);
	    v++;
	    c <<=1;
	}
	y++;
    }
}

extern unsigned char font[];

void ll_puts(int x, int y, char *t) {
    int n = strlen(t);
    while (n--) {
	ll_char_draw(x, y, font + (*t-32)*6);
	x += 4;
	t++;
    }
}

void ll_draw_back(int x, int y, int w, int h) {
    ll_cset(0);
    ll_bar(x,y,w,h);
    ll_cset(1);
}

/**************** 
   Widgets
*****************/

#include "snazzy.h"

#define MAX(a,b) (a > b ? a : b)
#define MIN(a,b) (a < b ? a : b)

void draw_all(widget *w);
void draw_coll(widget *w);
void bound(widget *w);

int bx1;
int by1;
int bx2;
int by2;
widget *focus;
widget *mwidget;
widget *wstack[16];
int wstack_ptr = 0;
int mx;
int my;

static unsigned char poplist_up[] = {
    0b00000000,
    0b11111110,
    0b01000100,
    0b00101000,
    0b00010000,
    0b00000000,
};

static unsigned char icon_hslide[] = {
    0b01111000,
    0b11111100,
    0b11111100,
    0b11111100,
    0b11111100,
    0b01111000,
};


void push_focus(widget *w) {
    wstack[wstack_ptr++] = focus;
    focus = w;
}

void pull_focus(void) {
    focus = wstack[--wstack_ptr];
}

void draw_back(widget *w) {
    ll_draw_back(w->x, w->y, w->w, w->h);
}


void do_vbox(widget *w, int ev) {
}

void do_hbox(widget *w, int ev) {
}

void do_label(widget *w, int ev) {
    switch(ev) {
    case EV_DRAW:
	ll_puts(w->x1, w->y1, w->text);
    }
}

void do_button(widget *w, int ev) {
    switch(ev) {
    case EV_DRAW:
	draw_back(w);
	ll_box(w->x, w->y, w->w, w->h);
	ll_puts(w->x1, w->y1, w->text);
	break;
    case EV_DOWN:
	draw_back(w);
	ll_box(w->x, w->y, w->w, w->h);
	ll_puts(w->x1+1, w->y1+1, w->text);
	break;
    case EV_UP:
	do_button(w, EV_DRAW);
	break;
    case EV_CLICK:
	fprintf(stderr,"button clicked\n");
	break;
    case EV_DOUBLE:
	fprintf(stderr,"button double clicked\n");
	break;
    }
}


/*

  poplist widget

*/

widget *poplist;
void do_poplist(widget *w, int ev) {
    switch (ev) {
    case EV_DRAW:
	draw_back(w);
	ll_char_draw(w->x1+1, w->y1+1, poplist_up);
	ll_puts(w->x1+9, w->y1+1, w->text);
	break;
    case EV_DOWN:
	draw_back(w);
	ll_char_draw(w->x1+2, w->y1+2, poplist_up);
	ll_puts(w->x1+10, w->y1+2, w->text);
	break;
    case EV_UP:
	do_poplist(w, EV_DRAW);
	break;
    case EV_CLICK:
	w->flags ^= FL_NODRAW;
	bound(w);
	ll_draw_back(bx1, by1, bx2-bx1, by2-by1);
	if (w->flags & FL_NODRAW) {
	    pull_focus();
	    draw_coll(focus);
	}
	else {
	    poplist = w;
	    draw_all(w);
	    ll_box(bx1, by1, bx2-bx1, by2-by1);
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
	ll_puts(w->x1+1, w->y1+1, w->text);
	break;
    case EV_UP:
	do_popitem(w, EV_DRAW);
	break;
    case EV_CLICK:
        poplist->text = w->text;
	poplist->d = w->d;
	do_poplist(poplist, ev);
	fprintf(stderr,"d = %d\n", w->d);
	break;
    }
}

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
	v = (w->d-w->x) * w->x1 / (w->w-6);
	fprintf(stderr,"hslide = %d\n", v);
	mwidget = NULL;
	break;
    }
}


    
void bound(widget *w) {
    widget *n = w->child;
    bx1 = n->x;
    by1 = n->y;
    bx2 = n->x + n->w;
    by2 = n->y + n->h;
    for (; n; n = n->next) {
	bx1 = MIN(bx1, n->x);
	by1 = MIN(by1, n->y);
	bx2 = MAX(bx2, n->x + n->w);
	by2 = MAX(by2, n->y + n->h);
    }
}


void draw_coll(widget *w) {
    widget *n;
    if ( w->x > bx2 ||
    	 w->y > by2 ||
    	 w->x + w->w < bx1 ||
    	 w->y + w->h < by1 )
    	return;
    if ((w->flags & FL_NODRAW) == 0) {
	for (n = w->child; n; n = n->next) {
	    draw_coll(n);
	    n->doev(n, EV_DRAW);
	}
    }
    w->doev(w, EV_DRAW);
}

void draw_all(widget *w) {
    widget *n;
    if ((w->flags & FL_NODRAW) == 0) {
	for (n = w->child; n; n = n->next) {
	    draw_all(n);
	    n->doev(n, EV_DRAW);
	}
    }
    w->doev(w, EV_DRAW);
}

int collide(widget *w, int x, int y) {
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
    for (n = head->child; n; n = n->next) {
	if (collide(n, x, y)) {
	    if (n->flags & FL_CLICKABLE) {
		return n;
	    }
	    else {
		t = collide_all(n, x, y);
		if (t) return t;
	    }
	}
    }
    if (collide(head,x,y)) return head;
    return NULL;
}


widget *clicked;
int time;
int dtime = 500;
widget *down;

/* called to send event to widget system 
     w = root widget
     e = input event
     x,y = coordinates of input event (if any)
*/
void send_uevent(int e, int x, int y) {
    widget *n = collide_all(focus, x, y);
    mx = x; my = y;
    if (n) {
	switch (e) {
	case UEV_MOVE:
	    n->doev(n, EV_MOVE);
	    break;
	case UEV_DOWN:
	    n->doev(n, EV_DOWN);
	    down = n;
	    break;
	case UEV_UP:
	    if (n != down) 
		down->doev(down, EV_UP);
	    else {
		n->doev(n, EV_UP);
		n->doev(n, EV_CLICK);
		if (n == clicked && (SDL_GetTicks() - time) < dtime) {
		    n->doev(n, EV_DOUBLE);
		    clicked = NULL;
		}
		else {
		    clicked = n;
		    time = SDL_GetTicks();
		}
	    }
	    break;
	}
    }
    else {
	if (down) down->doev(down, EV_UP);
	down == NULL;
	// fixme: I'm not sure this is great here
	if (wstack_ptr) {
	    focus->doev(focus, EV_CLICK);
	}
    }
}

/*
  Main / Testing
*/
#include "test.h"

void do_quit( ) {
    ll_deinit();
    exit(0);
}

int main(int argc, char *argv[]) {
    int ret;
    
    ll_init();
    ll_cset(1);

    focus = &noname0;
    draw_all(focus);
    SDL_UpdateWindowSurface(win);

    /* send SDL events to widget system */
    SDL_Event e;
    while(1) {
	ret = SDL_PollEvent(&e);
	if (ret == 0)
	    continue;
	if (e.type == SDL_MOUSEMOTION ){
	    if (mwidget) {
		send_uevent(UEV_MOVE, e.motion.x/2, e.motion.y/2);
	    }
	}
	if (e.type == SDL_QUIT) {
	    break;
	}
	if (e.type == SDL_MOUSEBUTTONDOWN){
	    send_uevent(UEV_DOWN, e.button.x/2, e.button.y/2);
	}
	if (e.type == SDL_MOUSEBUTTONUP){
	    send_uevent(UEV_UP, e.button.x/2, e.button.y/2);
	}
	if (e.type == SDL_TEXTINPUT) {
	    fprintf(stderr,"%d\n", e.text.text[0]);
	    if (e.text.text[0] == 'q') break;
	    if (e.text.text[0] == 'r') {
		ll_cset(0);
		ll_clear();
		draw_all(focus);
		continue;
	    }
	}
	SDL_UpdateWindowSurface(win);
    }
    
    ll_deinit();
    
    return 0;
}
