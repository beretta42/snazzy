/*  A button Class */


#include "snazzy.h"
#include "tgi.h"



static struct button_data {
    void (*clicked)(widget *w);
};

static void draw(widget *w) {
    /* background */
    ll_cset(1);
    ll_bar(w->x,w->y,w->w,w->h);
    /* box */
    ll_cset(0);
    ll_box(w->x+2, w->y+2, w->w-4, w->h-4);
    /* shadow */
    ll_hline(w->x+1, w->y + w->h -1, w->w-2+1);
    ll_vline(w->x+w->w-1, w->y+1, w->h-2);
}

static void layout(widget *w) {
    w->w += 8;
    w->h += 8;
}

static void set(widget *w,widget *c){
    c->x = w->x + (w->w/2 - c->w/2);
    c->y = w->y + (w->h/2 - c->h/2);
}

static void up(widget *w){
    draw_widget(w);
}

static void down(widget *w) {
    /* remove shadow from bottom right */
    ll_cset(1);
    ll_hline(w->x+1, w->y + w->h -1, w->w-2+1);
    ll_vline(w->x+w->w-1, w->y+1, w->h-2);
    /* put shadow top left */
    ll_cset(0);
    ll_hline(w->x+1, w->y+1, w->w-1);
    ll_vline(w->x+1, w->y+1, w->h-1);
}

void clicked(widget *w) {
    struct button_data *d = (struct button_data *)&w->data;
    if (d->clicked) d->clicked(w);
}

static struct vmt_s button_vmt = {
    draw,
    layout,
    set,
    down,
    up,
    clicked,
    noop,
};

void new_button(widget *w, void (*cb)(widget *w)) {
    struct button_data *d = (struct button_data *)&w->data;
    w->vmt = &button_vmt;
    w->flags |= S_MOUSE;
    d->clicked = cb;
}

