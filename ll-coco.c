/*

Low level graphics routines for coco 2

*/
#include "coco.h"
#include "snazzy.h"
#include "ll.h"


void ll_hline_setup(int x, int y, int w);
void ll_hline_go(void);

int mx, my;

int ll_init(void) {
    /* 256x192 bw mode green/light green */
    *(unsigned char *)0xff22 = 0xf0;
    *(unsigned char *)0xffc0 = 0x00;
    *(unsigned char *)0xffc3 = 0x00;
    *(unsigned char *)0xffc5 = 0x00;
    /* screen ram address to 0x6000 */
    *(unsigned char *)0xffc6 = 0x00;
    *(unsigned char *)0xffc8 = 0x00;
    *(unsigned char *)0xffca = 0x00;
    *(unsigned char *)0xffcc = 0x00;
    *(unsigned char *)0xffcf = 0x00;
    *(unsigned char *)0xffd1 = 0x00;
    *(unsigned char *)0xffd2 = 0x00;
    ll_cset(0);
    ll_clear();
    mouse_show();
    ll_cset(1);
    ei();
    return 0;
}


void ll_deinit(void) {

}


void ll_hline(int x, int y, int w){
    ll_hline_setup(x, y, w);
    ll_hline_go();
}


void ll_bar(int x, int y, int w, int h){
    ll_hline_setup(x, y, w);
    while(h--){
	ll_hline_go();
    }
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

void ll_loop() {
    while(1) {
	int mx = mouse_x/2;
	int my = mouse_y*3/8;
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
	    getkey();
	    send_uevent(UEV_KEY, mx, my);
	    key = 0;
	}
    }
}

void *memcpy(void *dest, void *src, int n) {
    char *d = dest;
    char *s = src;
    while(n--)
	*d++ = *s++;
    return dest;
};

extern widget testfrm[];
int load(char *name) {
    int fd;
    int ret;
    int t;
    //fd = open(name,O_RDONLY);
    //ret = read(fd,databuffer, 8192);
    //close(fd);
    memcpy(databuffer, testfrm, 2000);
    focus = (widget *)(databuffer + 2);
}


unsigned int ll_getticks(void) {
    return timer;
}
