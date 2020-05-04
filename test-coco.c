#include "coco.h"
#include "snazzy.h"
#include "ll.h"
#include "file.h"


void tobasic(void);

extern widget *testfrm;
static widget *panel;
static char buf1[32] = "GUIs are fun";

void do_example(widget *w, int ev) {
    select_panel(panel, w->d);
}

void do_quit(widget *w, int ev) {
    exit(0);
}

void do_text1(widget *w, int ev) {
    if (ev == AEV_TEXT)
	set_text(w, buf1, 32);
    if (ev == AEV_SELECT) {
	//fprintf(stderr,"text1: %s\n", buf1);
    }
}

void do_save(widget *w, int ev) {
    if (ev == AEV_SELECT) {
	graf_clear();
	draw_all(focus);
    }
}

void display(void) {
    char *p = (char *)0x400;
    int x,y;

    for (y = 50; y<50+32; y += 2) {
	for (x = 50; x<50+32; x++) {
	    if (*p++ == 0x60) {
		graf_setpixel(x,y);
		graf_setpixel(x,y+1);
	    }
	}
    }
}

void do_thumb(widget *w, int ev) {
    display();

}

void do_basic(widget *w, int ev) {
    tobasic();
}

void main(void) {
    int i;
    szy_init();
    load("TEST.FRM");
    reg_appcall("example", do_example);
    reg_appcall("quit", do_quit);
    reg_appcall("text1", do_text1);
    reg_appcall("save", do_save);
    reg_appcall("thumb", do_thumb);
    reg_appcall("basic", do_basic);
    panel = find_widget("ren");
    ll_loop();
    while(1);
}
