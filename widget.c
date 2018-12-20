/* A Very Generic Skeleton Widget */

#include "snazzy.h"


static struct widget_data {
    unsigned char state;
};


static void draw(widget *w) {
}

static void layout(widget *w) {
}

static void set(widget *w, widget *c){
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

void new_widget(widget *w) {
    memset(w,0,sizeof(widget));
    w->vmt = &widget_vmt;
}
