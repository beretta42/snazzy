/* Area Widget */

#include <string.h>
#include "snazzy.h"
#include "ll.h"

static struct widget_data {
};


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

static void up(widget *w){
}

static void down(widget *w){
}

static void clicked(widget *w){
}

static struct vmt_s widget_vmt = {
    draw,
    layout,
    set,
    down,
    up,
    clicked,
    noop,
};

void new_area(widget *w) {
    w->vmt = &widget_vmt;
}
