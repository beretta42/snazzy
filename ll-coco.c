/*

Low level graphics routines for coco 2

*/
#include "stdint.h"
#include "file.h"
#include "coco.h"
#include "snazzy.h"
#include "ll.h"
#include "graf.h"
#include "string.h"

int mx, my;

int ll_init(void) {
    /* 256x192 bw mode green/light green */
    *(unsigned char *)0xff22 = 0xf0;
    *(unsigned char *)0xffc0 = 0x00;
    *(unsigned char *)0xffc3 = 0x00;
    *(unsigned char *)0xffc5 = 0x00;
    /* screen ram address to 0x6000 */
    graf_setbuf((char *)0x6000);
    graf_cset(0);
    graf_clear();
    mouse_show();
    graf_cset(1);
    ei();
    return 0;
}


void ll_deinit(void) {

}

void ll_setclip(int x, int y, int w, int h) {
    graf_setclip(x, y, w, h);
}

void ll_setpixel(int x, int y) {
    graf_setpixel(x, y);
}

void ll_bar(int x, int y, int w, int h){
    graf_bar(x, y, w, h);
}

void ll_cset(int color) {
    graf_cset(color);
}

void ll_char_draw(int x, int y, unsigned char *p) {
    graf_char_draw(x, y, p);
}


void ll_vline(int x, int y, int h) {
    graf_bar(x, y, 1, h);
}

void ll_hline(int x, int y, int w){
    graf_bar(x, y, w, 1);
}

void ll_box(int x, int y, int w, int h){
    ll_hline(x, y, w);
    ll_hline(x, y+h-1, w);
    ll_vline(x, y, h);
    ll_vline(x+w-1, y, h);
}

extern unsigned char font[];

void ll_puts(int x, int y, char *t) {
    ll_putn(x, y, t, szy_strlen(t));
}

void ll_putn(int x, int y, char *t, int len) {
    while (len--) {
	graf_char_draw(x,y, font + (*t-32)*6);
	x += 4;
	t++;
    }
}


void ll_draw_back(int x, int y, int w, int h) {
    graf_cset(0);
    graf_bar(x,y,w,h);
    graf_cset(1);
}

char *pos = (char *)0x6000;
int foo = 5;
int bar = 5;


void testfoo(void) {
    graf_cset(0);
    graf_bar(foo-1, bar-1, 9, 9);
    graf_cset(1);
    graf_char_draw(foo, bar, font+(30*6));
}



void ll_loop() {
    while(1) {
	int mx = mouse_x;
	int my = mouse_y;
	/* send movement events */
	if (mwidget && mouse_mf) {
	    mouse_hide();
	    mouse_mf = 0;
	    send_uevent(UEV_MOVE, mx, my);
	    mouse_show();
	}
	/* send button events */
	if (mouse_f) {
	    mouse_hide();
	    if (mouse_b)
		send_uevent(UEV_DOWN, mx, my);
	    else
		send_uevent(UEV_UP, mx, my);
	    mouse_show();
	    mouse_f = 0;
	}
	/* send keyboard events */
	if (key) {
	    mouse_hide();
	    getkey();
#ifdef DEBUG_CLIP
	    if (key == 'u')
		graf_setbuf(pos += 512);
	    if (key == 'd')
		graf_setbuf(pos -= 512);
	    if (key == 'e') {
		bar +=1;
		testfoo();
	    }
	    if (key == 'w') {
		bar -= 1;
		testfoo();
	    }
	    if (key == 'a') {
		foo -= 1;
		testfoo();
	    }
	    if (key == 's') {
		foo += 1;
		testfoo();
	    }
#endif /* DEBUG_CLIP */
	    send_uevent(UEV_KEY, mx, my);
	    key = 0;
	    mouse_show();
	}
    }
}

int load(char *name) {
    int ret;
    int fd;

    fd = file_open(name,O_RDONLY);
    ret = file_read(fd, databuffer, 2000);
    file_close(fd);
    graf_clear();
    focus = (widget *)(databuffer + 2);
    draw_all(focus);
}


unsigned int ll_getticks(void) {
    return timer;
}
