/* A Radio Button */

#include <string.h>
#include "snazzy.h"
#include "ll.h"

static unsigned char ico_up[] = {
    0b01111000,
    0b11001100,
    0b10000100,
    0b10001100,
    0b11011100,
    0b01111000,
};

static unsigned char ico_down[] = {
    0b01111000,
    0b11101100,
    0b11000100,
    0b10000100,
    0b11001100,
    0b01111000,
};

static unsigned char ico_set[] = {
    0b01111000,
    0b11010100,
    0b10111100,
    0b11110100,
    0b10101100,
    0b01111000,
};

typedef struct widget_data * data;

struct widget_data {
    unsigned char state;
    widget *sel;            /* currently selected */
    widget *group;
};


static void draw(widget *w) {
    ll_cset(1);
    ll_bar( w->x, w->y, 8, 6);
    ll_cset(0);
    ll_char_draw(w->x+1, w->y+1, ico_up);
    if (w->data[0])
	ll_char_draw(w->x+1, w->y+1, ico_set);
}

static void layout(widget *w) {
    w->w += 10;
    w->h += 4;
}

static void set(widget *w, widget *c){
    c->x += 10;
}

static void up(widget *w){
    draw(w);
}

static void down(widget *w){
    ll_cset(1);
    ll_bar(w->x,w->y, 8, 6);
    ll_cset(0);
    ll_char_draw(w->x+1, w->y+1, ico_down);
}

static void clicked(widget *w){
    data d = (data)&w->data;
    d = (data)&(d->group->data);
    d->sel->data[0] = 0;
    draw(d->sel);
    d->sel = w;
    w->data[0] = -1;
    draw(w);
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


void new_radio(widget *w, widget *g) {
    struct widget_data *d = (struct widget_data *)&w->data;
    w->vmt = &widget_vmt;
    w->flags |= S_MOUSE;
    /* attaching to another group leader? */
    if (g) {
	d->group = g;
    }
    /* No! then we're on our own, set our groups data */
    else {
	d->group = w;
	d->sel = w;
       	d->state = 1;
    }
}
