/*

Low level graphics routines for coco 2

*/
#include "snazzy.h"
#include "ll.h"

int mx, my;
int pen = 0;



int ll_init(void) {
    ll_cset(1);
    return 0;
}

void ll_cset(int color){
    pen = color;
}

void ll_deinit(void) {

}


void ll_clear(void){
    if (pen) {
	;
    }
    else {
	;
    }
}

void ll_setpixel(int x, int y) {
    if (pen)
	;
	//SDL_FillRect(screen, &r, SDL_MapRGB(screen->format, 0, 255, 0));
    else
	;
	//SDL_FillRect(screen, &r, SDL_MapRGB(screen->format, 0, 0, 0));
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
    ll_putn(x, y, t, szy_strlen(t));
}

void ll_putn(int x, int y, char *t, int len) {
    while (len--) {
	ll_char_draw(x,y, font + (*t-32)*6);
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
    ll_cset(1);
    ll_char_draw(x,y, icon_mouse);
}

void ll_unput_mouse(int x, int y) {
}


void ll_loop() {
    /* send SDL events to widget system */
    int ret;
    ll_put_mouse(0,0);
    while(1) {
	ll_put_mouse(mx,my);
    }
}


int load(char *name) {
    int fd;
    int ret;
    int t;
    //fd = open(name,O_RDONLY);
    //ret = read(fd,databuffer, 8192);
    //close(fd);
    focus = (widget *)(databuffer + 2);
    // fixme don't do this stuff if big endian!!
    t = databuffer[0];
    databuffer[0] = databuffer[1];
    databuffer[1] = t;
    fixup(focus);
}


unsigned int ll_getticks(void) {
    return 0;
}
