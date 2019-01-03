/*  A button Class */


#include <stdio.h>
#include "snazzy.h"
#include "tgi.h"



static void draw(widget *w) {
    /* background */
    ll_cset(1);
    ll_bar(w->x,w->y,w->w,w->h);
    /* box */
    ll_cset(0);
    ll_box(w->x+2, w->y+2, w->w-4, w->h-4);
    /* shadow */
    ll_hline(w->x+1, w->y + w->h -2, w->w-3);
    ll_vline(w->x+w->w-2, w->y+1, w->h-2);
}

static void layout(widget *w) {
    w->w += 8;
    w->h += 8;
}

static void set(widget *w,widget *c){
    c->x = w->x + (w->w/2 - c->w/2);
    c->y = w->y + (w->h/2 - c->h/2);
}

static void compile(widget *w) {
    printf("\t&button_vmt,\n");
    printf("\t.data.button = {\n");
    printf("\t\t&%s_clicked,\n", w->ct->name);
    printf("\t},\n");
}

static void up(widget *w){
    draw_widget(w);
}

static void down(widget *w) {
    /* remove shadow from bottom right */
    ll_cset(1);
    ll_hline(w->x+1, w->y + w->h -2, w->w-3);
    ll_vline(w->x+w->w-2, w->y+1, w->h-2);
    /* put shadow top left */
    ll_cset(0);
    ll_hline(w->x+1, w->y+1, w->w-2);
    ll_vline(w->x+1, w->y+1, w->h-2);
}

static void clicked(widget *w) {
    if (w->data.button.clicked)	w->data.button.clicked(w);
}

struct vmt_s button_vmt = {
    draw,
    down,
    up,
    clicked,
    noop,
    noop,
};

void new_button(widget *w, void (*cb)(widget *w)) {
    w->vmt = &button_vmt;
    w->flags |= S_MOUSE;
    w->data.button.clicked = cb;
    w->ct->layout = layout;
    w->ct->set = set;
    w->ct->compile = compile;
}

