#include "coco.h"
#include "snazzy.h"
#include "ll.h"
#include "file.h"

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

void main(void) {
    int i;
    szy_init();
    load("TEST.FRM");
    reg_appcall("example", do_example);
    reg_appcall("quit", do_quit);
    reg_appcall("text1", do_text1);
    panel = find_widget("ren");
    ll_loop();
    while(1);
}
