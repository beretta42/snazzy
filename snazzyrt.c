#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

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
    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0,255,0));
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
    ll_hline(x, y, w-1);
    ll_hline(x, y+h-1, w-1);
    ll_vline(x, y, h-1);
    ll_vline(x+w-1, y, h-1);
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


/**************** 
   Widgets
*****************/

#include "snazzy.h"

void noop(widget *w) {
}

void set_noop(widget *w, widget *c){
}


void draw_widget(widget *w){
    if( w->flags & S_HIDDEN )
	return;
    if (w->vmt)	w->vmt->draw(w);
    widget *p = w->child;
    while (p){
	draw_widget(p);
	p = p->sib;
    }
}


widget *root;
widget *dialog;

void draw_root(widget *w){
    ll_clear();
}

struct vmt_s root_vmt = {
    draw_root,
    noop,
    noop,
    noop,
    noop,
    noop,
};

int s_init(int w, int h) {
    dialog = root = &myroot;
    ll_clear();
}


/* redraw all widgets in an area */
void draw_area(int x, int y, int w, int h) {
    widget *p = root->child;
    ll_bar(x,y,w,h);
    ll_cset(0);
    while (p) {
	if ( x >= p->x + p->w     ||
	     x + w <  p->x        ||
	     y >= p->y + p->h     ||
	     y + h < p->y
	     );
	else {
	    draw_widget(p);
	}
	p = p->sib;
    }
}

/* hide all widgets in a widget */
void hide_widget(widget *w) {
    ll_cset(1);
    ll_bar(w->x, w->y, w->w, w->h);
    w->flags += S_HIDDEN;
}


/* is point (x,y) with in widget W, or any of its */
widget *collide(widget *w, int x, int y) {
    widget *p = dialog->child;
    while (p){
	if (p->flags & S_MOUSE &&
	    x >= p->x          &&
	    x < p->x + p->w    &&
	    y >= p->y          &&
	    y < p->y + p->h
	    )
	    return p;
	p = p->sib;
    }
    return NULL;
}



void snazzy_go(void) {
    int ret;
    
    draw_widget(&myroot);

    SDL_UpdateWindowSurface(win);

    /* event loop */
    SDL_Event e;
    widget *cd = NULL;
    char *s;
    widget *f;
    while(1) {
	ret = SDL_PollEvent(&e);
	if (ret == 0)
	    continue;
	if (e.type == SDL_MOUSEMOTION && e.motion.state &&
	    cd ){
	    cd->vmt->move(cd);
	}
	if (e.type == SDL_QUIT)
	    break;
	if (e.type == SDL_MOUSEBUTTONDOWN){
	    widget *w;
	    w = collide(dialog, e.button.x/2,e.button.y/2);
	    if (w) {
		w->vmt->down(w);
		cd = w;
	    }
	    SDL_UpdateWindowSurface(win);
	}
	if (e.type == SDL_MOUSEBUTTONUP){
	    widget *w;
	    w = collide(dialog, e.button.x/2,e.button.y/2);
	    if (w) {
		if (w == cd){
		    cd->vmt->up(cd);
		    cd->vmt->clicked(cd);
		}
		else {
		    cd->vmt->up(cd);
		}
	    }
	    else {
		if (cd) cd->vmt->up(cd);
	    }
	}
	SDL_UpdateWindowSurface(win);
    }
    
    ll_deinit();
    
    return;
}
