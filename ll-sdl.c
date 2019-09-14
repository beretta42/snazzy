/*

Low level graphics routines for SDL2

*/

#include <unistd.h>
#include <fcntl.h>
#include <SDL2/SDL.h>
#include "ll.h"

SDL_Window *win = NULL;
SDL_Surface *screen = NULL;
SDL_Surface *mouse_back = NULL;
SDL_Rect mouse_rec;
int mx, my;
int pen = 0;


int ll_init(void) {
    int ret;

    ret = SDL_Init(SDL_INIT_VIDEO);
    win = SDL_CreateWindow("A Fancy Title",
			   10,
			   10,
			   512, 384,
			   SDL_WINDOW_SHOWN);
    screen = SDL_GetWindowSurface(win);
    ll_cset(1);
    SDL_UpdateWindowSurface(win);
    SDL_ShowCursor(SDL_DISABLE);
    mouse_back = SDL_CreateRGBSurface(0, 16, 12, 32, 0, 0, 0, 0);
    if (mouse_back == NULL) {
	fprintf(stderr,"cannot allocate mouse buffer\n");
	exit(1);
    }
    mouse_rec.x = 0;
    mouse_rec.y = 0;
    mouse_rec.w = 16;
    mouse_rec.h = 12;
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


#include "snazzy.h"

static unsigned char icon_mouse[] = {
    0b10000000,
    0b11100000,
    0b11111000,
    0b11110000,
    0b00011000,
    0b00001100,
};


void ll_put_mouse(int x, int y) {
    SDL_Rect s;
    s.x = x * 2;
    s.y = y * 2;
    s.w = 16;
    s.h = 12;
    SDL_BlitSurface(screen, &s, mouse_back, &mouse_rec);
    ll_cset(1);
    ll_char_draw(x,y, icon_mouse);
}

void ll_unput_mouse(int x, int y) {
    SDL_Rect d;
    d.x = x * 2;
    d.y = y * 2;
    SDL_BlitSurface(mouse_back, &mouse_rec, screen, &d);
}


void ll_loop() {
    /* send SDL events to widget system */
    int ret;
    SDL_Event e;
    SDL_UpdateWindowSurface(win);
    while(1) {
	ret = SDL_PollEvent(&e);
	if (ret == 0)
	    continue;
	ll_unput_mouse(mx, my);
	if (e.type == SDL_MOUSEMOTION ){
	    mx = e.motion.x/2;
	    my = e.motion.y/2;
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
	ll_put_mouse(mx,my);
	if (drawf) SDL_UpdateWindowSurface(win);
    }
}


int load(char *name) {
    int fd;
    int ret;
    int t;
    fd = open(name,O_RDONLY);
    if (fd < 0)
	return 1;
    ret = read(fd,databuffer, 8192);
    close(fd);
    focus = (widget *)(databuffer + 2);
    // fixme don't do this stuff if big endian!!
    t = databuffer[0];
    databuffer[0] = databuffer[1];
    databuffer[1] = t;
    fixup(focus);
}


unsigned int ll_getticks(void) {
    return SDL_GetTicks();
}
