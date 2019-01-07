/*  A Menu item */


#include <stdio.h>
#include "snazzy.h"
#include "tgi.h"



static void draw(widget *w) {
}

static void layout(widget *w) {
    w->w += 2;
    w->h += 2;
}

static void set(widget *w,widget *c){
    c->x = w->x + 1;
    c->y = w->y + 1;
}

static void compile(widget *w) {
    printf("\t&menuitem_vmt,\n");
    printf("\t.data.button = {\n");
    printf("\t\t&%s_clicked,\n", w->ct->name);
    printf("\t},\n");
}

static void up(widget *w){
    ll_cset(1);
    ll_hline(w->x + 1, w->y + w->h - 2, w->w - 2);
}

static void down(widget *w) {
    ll_cset(0);
    ll_hline(w->x + 1, w->y + w->h - 2, w->w - 2);
}

static void clicked(widget *w) {
    if (w->data.button.clicked)	w->data.button.clicked(w);
    dialog = root;
    w->parent->flags |= S_HIDDEN;
    draw_area(w->parent->x, w->parent->y, w->parent->w, w->parent->h);
}

struct vmt_s menuitem_vmt = {
    draw,
    down,
    up,
    clicked,
    noop,
    noop,
};

void new_menuitem(widget *w, void (*cb)(widget *w)) {
    w->vmt = &menuitem_vmt;
    w->flags |= S_MOUSE;
    w->data.button.clicked = cb;
    w->ct->layout = layout;
    w->ct->set = set;
    w->ct->compile = compile;
}

