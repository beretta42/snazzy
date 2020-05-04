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
/* fixme: remove these next debuging sprites */
#include "floppy.h"

extern unsigned char stip1[];

int mx, my;

// fixme: test
unsigned char stip1[] = {
    0b01010101,
    0b10101010,
    0b01010101,
    0b10101010,
    0b01010101,
    0b10101010,
    0b01010101,
    0b10101010,
};
unsigned char stip2[] = {
    //--------
    0b00010001,
    0b01000100,
    0b00100010,
    0b10001000,
    0b00010001,
    0b01000100,
    0b00100010,
    0b10001000,
};
unsigned char stip3[] = {
    // --------
    0b00000000,
    0b11001100,
    0b00000000,
    0b00110011,
    0b00000000,
    0b11001100,
    0b00000000,
    0b00110011,
};
unsigned char stip4[] = {
    // --------
    0b11011101,
    0b00010001,
    0b01001011,
    0b01111000,
    0b00011110,
    0b11010010,
    0b10001000,
    0b10111011,
};
unsigned char stip5[]={ -1,-1,-1,-1,-1,-1,-1,-1 };

void ll_circ(int xc, int yc, int r);

//#define DEBUG
int ll_init(void) {
    /* 256x192 bw mode green/light green */
    *(unsigned char *)0xff22 = 0xf0;
    *(unsigned char *)0xffc0 = 0x00;
    *(unsigned char *)0xffc3 = 0x00;
    *(unsigned char *)0xffc5 = 0x00;
    /* screen ram address to 0x6000 */
    graf_setbuf((char *)0x6000);  // fixme: figure out where this should be
    graf_cset(0);
    graf_clear();
    graf_cset(1);
    ei();
    graf_setclip(0,0, 256, 192);

#ifdef DEBUG
    int x = 128;
    int y = 10;

    while(1) {
	if (key) {
	    getkey();
	    if (key == 'u') y -= 1;
	    if (key == 'd') y += 1;
	    if (key == 'y') y -= 8;
	    if (key == 'b') y += 8;
	    if (key == 'a') x -= 1;
	    if (key == 's') x += 1;
	    if (key == 'l') x += 8;
	    if (key == 'k') x -= 8;
	    if (key == 'g') x  = 0;
	    if (key == 'r') graf_clear();
	    if (key == 'x') break;
	    graf_clear();
	    //graf_stiple(0,0,256,192,stip5);
	    //graf_bar(0, 100, 1, 3);
	    //graf_bar(8, 100, 1, 3);
	    //graf_bar(16, 100, 1, 3);
	    //graf_bar(24, 100, 1, 3);
	    //graf_bar(32, 100, 1, 3);
	    //graf_bar(5, 5, 128, 64);
	    //graf_bar(5+60-1, 5, 1, 60 - 5);
	    //graf_blit(x, y, floppy_ico);
	    ll_roundrect(x, y, 60, 60, stip5, 16);
	}
    }
#endif
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

void circ(int x, int y, int xc, int yc) {
    graf_setpixel(-x+xc,-y+yc);
    graf_setpixel(-y+xc,-x+yc);
}


static int cd;
static int cx;
static int cy;

static int circ_step(void) {
    while (cy >= cx) {
	cx++;
	if (cd < 0)
	    cd += (4 * cx) + 6;
	else {
	    cd += 4 * (cx - cy) + 10;
	    cy--;
	    return 1;
	}
    }
    return 0;
}


static int circ_setup(int r) {
    r += 1;
    cd = 3 - (2 * r);
    cx = 0;
    cy = r;
}

void ll_circ(int xc, int yc, int r) {
    int d = 3 - (2 * r);
    int x = 0;
    int y = r;
    circ(x,y, xc, yc);
    while (y >= x) {
	x++;
	if (d < 0)
	    d += (4 * x) + 6;
	else {
	    d += 4 * (x - y) + 10;
	    y--;
	}
	circ(x, y, xc, yc);
    }
}

void ll_roundrect(int x, int y, int w, int h, unsigned char *s, int r) {
    int y1 = y;
    int y2 = y+h;
    int y3 = y+r;
    int y4 = y+h-r;
    int delta = 0;
    int old = 0;
    int diam = r * 2;
    int adjw = w - diam;

    if (r < 3 ) {
	graf_stiple(x, y, w, h, s);
	return;
    }
    circ_setup(r);
    while(circ_step()) {
	/* top and bottom edge round parts */
	graf_stiple(x + r - cx, y1++, cx * 2 + adjw, 1, s);
	graf_stiple(x + r - cx, --y2, cx * 2 + adjw, 1, s);
	delta = cx - old;
	y3 -= delta;
	/* middle rounded rounded parts */
	graf_stiple(x + r - cy, y3, cy * 2 + adjw, delta, s);
	graf_stiple(x + r - cy, y4, cy * 2 + adjw, delta, s);
	y4 += delta;
	old = cx;
    }
    /* the square stuff middle */
    graf_stiple(x, y + r, w, h - (r*2), s);

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
    //graf_stiple(x,y,w,h,stip2);
}

char *pos = (char *)0x6000;
int foo = 5;
int bar = 5;

/* for clipping testing, fixme: delete */
void testfoo(void) {
    graf_cset(0);
    graf_bar(foo-1, bar-1, 9, 9);
    graf_cset(1);
    graf_char_draw(foo, bar, font+(30*6));
}



void ll_loop() {
    draw_all(focus);
    mouse_show();

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
#ifdef DEBUG
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
#endif /* DEBUG */
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
    ret = file_read(fd, databuffer, 0x2000);
    file_close(fd);
    focus = (widget *)(databuffer + 2);
    mouse_hide();
    graf_clear();
    mouse_show();
}


unsigned int ll_getticks(void) {
    return timer;
}

void ll_blit(int x, int y, unsigned char *bm) {
    graf_blit(x, y, bm);
}
