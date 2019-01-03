/* Area Widget */

#include <stdio.h>
#include <string.h>
#include "snazzy.h"
#include "ll.h"


static void draw(widget *w) {
    ll_box(w->x, w->y, w->w, w->h);
}

static void layout(widget *w) {
    w-> w += 4;
    w-> h += 4;
}

static void set(widget *w, widget *c){
    c->x += 2;
    c->y += 2;
}

static void compile(widget *w){
    printf("\t&area_vmt,\n");
}

static void up(widget *w){
}

static void down(widget *w){
}

static void clicked(widget *w){
}

struct vmt_s area_vmt = {
    draw,
    down,
    up,
    clicked,
    noop,
    noop,
};

void new_area(widget *w) {
    w->vmt = &area_vmt;
    w->ct->layout = layout;
    w->ct->set = set;
    w->ct->compile = compile;
}
