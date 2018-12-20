#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/* 
   Lowlevel graphics routines

*/

#include <SDL2/SDL.h>
#include "/home/beretta/C/platotermCoCo/src/coco2/font.c"
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


struct label_data {
    char *text;
};

void draw_label(widget *w);
void layout_label(widget *w);
void set_label(widget *w, widget *c);

struct vmt_s label_vmt = {
    draw_label,
    layout_label,
    set_label,
    noop,
    noop,
    noop,
    noop,
};

void draw_label(widget *w) {
    struct label_data *d = (struct label_data *)&w->data;
    char *s = d->text;
    int x = w->x + 1;
    int y = w->y + 1;
    int c;
    while (c = *s++) {
	ll_char_draw(x, y, font + (c-32)*6);
	x += 4;
    }
}

void set_label(widget *w, widget *c){
}

void new_label(widget *w, char *s) {
    struct label_data *d = (struct label_data *)&w->data;
    memset(w,0, sizeof(widget));
    d->text = s;
    w->vmt = &label_vmt;
}

void layout_label(widget *w) {
    struct label_data *d = (struct label_data *)&w->data;
    w->h = 6;
    w->w = strlen(d->text) * 4 + 2;
}



unsigned char check_ico_up[] = {
    0b11111100,
    0b10000100,
    0b10000100,
    0b10000100,
    0b10000100,
    0b11111100,
};
    
unsigned char check_ico_set[] = {
    0b00000110,
    0b00001100,
    0b11011000,
    0b01110000,
    0b00110000,
    0b00000000,
};

unsigned char check_ico_down[] = {
    0b11111100,
    0b10101100,
    0b11000100,
    0b10001100,
    0b11010100,
    0b11111100,
};

struct check_data {
    unsigned char state;
};

void layout_check(widget *w){
    w->w += 10;
    w->h += 4;
}

void draw_check(widget *w) {
    ll_cset(1);
    ll_bar( w->x, w->y, 8, 6);
    ll_cset(0);
    ll_char_draw(w->x+1, w->y+1, check_ico_up);
    if (w->data[0])
	ll_char_draw(w->x+1, w->y+1, check_ico_set);
}

void down_check(widget *w){
    ll_cset(1);
    ll_bar( w->x, w->y, 8, 6);
    ll_cset(0);
    ll_char_draw(w->x+1, w->y+1, check_ico_down);
}

void clicked_check(widget *w){
    struct check_data *d = (struct check_data *)&w->data;
    d->state = ~d->state;
    draw_check(w);
}

void set_check(widget *w, widget *c){
    c->x += 10;
}

struct vmt_s check_vmt = {
    draw_check,
    layout_check,
    set_check,
    down_check,
    draw_check,
    clicked_check,
    noop,
};

void new_check(widget *w) {
    memset(w,0,sizeof(widget));
    w->vmt = &check_vmt;
    w->flags |= S_MOUSE;
}




struct button_data {
    void (*clicked)(widget *w);
};

void draw_button(widget *w) {
    /* background */
    ll_cset(1);
    ll_bar(w->x,w->y,w->w,w->h);
    /* box */
    ll_cset(0);
    //ll_box(w->x+2, w->y+2, w->w-4, w->h-4);
    ll_hline(w->x+2, w->y+2, w->w-4);
    ll_hline(w->x+2, w->y + w->h-2, w->w-4+1);
    ll_vline(w->x+2, w->y+2, w->h-4);
    ll_vline(w->x+ w->w -2, w->y+2, w->h-4);
    /* shadow */
    ll_hline(w->x+1, w->y + w->h -1, w->w-2+1);
    ll_vline(w->x+w->w-1, w->y+1, w->h-2);
}

void layout_button(widget *w) {
    w->w += 8;
    w->h += 8;
}

void set_button(widget *w,widget *c){
    c->x = w->x + (w->w/2 - c->w/2);
    c->y = w->y + (w->h/2 - c->h/2);
}

void up_button(widget *w){
    draw_widget(w);
}

void down_button(widget *w) {
    /* remove shadow from bottom right */
    ll_cset(1);
    ll_hline(w->x+1, w->y + w->h -1, w->w-2+1);
    ll_vline(w->x+w->w-1, w->y+1, w->h-2);
    /* put shadow top left */
    ll_cset(0);
    ll_hline(w->x+1, w->y+1, w->w-1);
    ll_vline(w->x+1, w->y+1, w->h-1);
}

void clicked_button(widget *w) {
    struct button_data *d = (struct button_data *)&w->data;
    if (d->clicked) d->clicked(w);
}

struct vmt_s button_vmt = {
    draw_button,
    layout_button,
    set_button,
    down_button,
    up_button,
    clicked_button,
    noop,
};

void new_button(widget *w, void (*cb)(widget *w)) {
    struct button_data *d = (struct button_data *)&w->data;
    memset(w,0, sizeof(widget));
    w->vmt = &button_vmt;
    w->flags |= S_MOUSE;
    d->clicked = cb;
}

void draw_widget(widget *w){
    if( w->flags & S_HIDDEN )
	return;
    w->vmt->draw(w);
    widget *p = w->child;
    while (p){
	draw_widget(p);
	p = p->sib;
    }
}


widget root;
widget *dialog = &root;

void draw_root(widget *w){
    ll_clear();
}

struct vmt_s root_vmt = {
    draw_root,
    noop,
    set_noop,
    noop,
    noop,
    noop,
    noop,
};

int s_init(int w, int h) {
    memset(&root, 0, sizeof(widget));
    root.w = w;
    root.h = h;
    root.vmt = &root_vmt;
}


/* redraw all widgets in an area */
void draw_area(int x, int y, int w, int h) {
    widget *p = root.child;
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


/*
  Main / Testing
*/



widget xlab1;
widget xlab2;
widget xlab3;
widget xbut1;
widget xbut2;
widget xbut3;
widget xlab4;
widget xcheck1;
widget xlab5;
widget xcheck2;
widget xlab6;
widget xrad1;
widget xlab7;
widget xrad2;
widget xlab8;
widget xrad3;
widget xlab9;
widget xlab10;
widget xlab11;
widget xlab12;
widget xlab13;
widget xmenu1;
widget xitem1;
widget xitem2;
widget xitem3;
widget xarea1;

char *get_name(widget *w){
    char *s;
    s = "unknown";
    if (w == &xlab1)
	s = "xlab1";
    if (w == &xlab2)
	s = "xlab2";
    if (w == &xbut1)
	s = "xbut1";
    if (w == &xbut2)
	s = "xbut2";
    if (w == &root)
	s = "root";
    if (w == &xbut3)
	s = "xbut3";
    if (w == &xlab4)
	s = "xlab4";
    if (w == &xlab3)
	s = "xlab3";
    return s;
}

void list_widget(widget *w){
    static int t = 0;
    int x;
    char *s;
    for( x=0; x<t; x++)
	printf("  ");
    s = get_name(w);
    printf("%s %p: %p %p %d %d %d %d\n",
	   s, w, w->child, w->sib, w->x, w->y, w->w, w->h);
    widget *p = w->child;
    t++;
    while (p) {
	list_widget(p);
	p = p->sib;
    }
    t--;
}

void do_ok(widget *w) {
    printf("ok clicked!\n");
}

void do_accept(widget *w) {
    printf("accept clicked!\n");
}

void do_cancel(widget *w) {
    printf("cancel clicked\n");
    ll_deinit();
    exit(0);
}


int main(int argc, char *argv[]) {
    int ret;
    
    ll_init();
    s_init(256,192);
    ll_cset(0);

    new_area(&xarea1);
    xarea1.flags |= S_VERT;
    new_label(&xlab10, "Open");
    new_label(&xlab11, "Close");
    new_label(&xlab12, "Save");
    pack_widget(&xarea1, &xlab10);
    pack_widget(&xarea1, &xlab11);
    pack_widget(&xarea1, &xlab12);
    new_label(&xlab13, "File");
    new_menu(&xmenu1, &xarea1);
    pack_widget(&xmenu1, &xlab13);
    pack_widget(&root, &xmenu1);
    pack_widget(&root, &xarea1);
    
    new_check(&xcheck1);
    new_label(&xlab5, "checky");
    pack_widget(&xcheck1, &xlab5);
    pack_widget(&root, &xcheck1);

    new_check(&xcheck2);
    new_label(&xlab6, "no check me!");
    pack_widget(&xcheck2, &xlab6);
    pack_widget(&root, &xcheck2);

    new_radio(&xrad1,NULL);
    new_label(&xlab7, "Radio");
    pack_widget(&xrad1, &xlab7);
    pack_widget(&root, &xrad1);

    new_radio(&xrad2, &xrad1);
    new_label(&xlab8, "buttons");
    pack_widget(&xrad2, &xlab8);
    pack_widget(&root, &xrad2);

    new_radio(&xrad3, &xrad1);
    new_label(&xlab9, "are awesome.");
    pack_widget(&xrad3, &xlab9);
    pack_widget(&root, &xrad3);

    new_label(&xlab1, "Ok");
    new_label(&xlab2, "Cancel");
    new_label(&xlab3, "Accept");
    new_label(&xlab4, "Select One");
    new_button(&xbut1, do_ok);
    new_button(&xbut2, do_cancel);
    new_button(&xbut3, do_accept);
    pack_widget(&root,&xlab4);
    pack_widget(&root,&xbut1);
    pack_widget(&root,&xbut2);
    pack_widget(&root,&xbut3);
    pack_widget(&xbut1,&xlab1);
    pack_widget(&xbut2,&xlab2);
    pack_widget(&xbut3,&xlab3);


    root.flags = S_VERT;
    layout_widget(&root);
    set_widget(&root, 2, 2);
    list_widget(&root);
    draw_widget(&root);

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
    
    return 0;
}
