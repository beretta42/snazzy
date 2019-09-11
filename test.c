#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "snazzy.h"

/*
  Main / Testing
*/

widget *panel;

void do_quit(widget *w, int ev) {
    fprintf(stderr,"quit clicked\n", ev);
    ll_deinit();
    exit(0);
}

void do_save(widget *w, int ev) {
    fprintf(stderr,"save button clicked\n");
}

void do_example(widget *w, int ev) {
    fprintf(stderr,"example select: %d\n", w->d);
    select_panel(panel, w->d);
}

void do_volume(widget *w, int ev) {
    fprintf(stderr,"new volume: %d\n", get_hslide(w));
}

void do_foo(widget *w, int ev) {
    fprintf(stderr,"foo!\n");
}

void do_main(widget *w, int ev) {
    fprintf(stderr,"menu select: %d\n", w->d);
}

int main(int argc, char *argv[]) {
    int ret;
    
    ll_init();
    
    load("test.frm");
    reg_appcall("quit", do_quit);
    reg_appcall("save", do_save);
    reg_appcall("example", do_example);
    reg_appcall("volume", do_volume);
    reg_appcall("foo", do_foo);
    reg_appcall("main", do_main);
    panel = find_widget("ren");
    draw_all(focus);
    ll_loop();
    ll_deinit();
    
    return 0;
}
