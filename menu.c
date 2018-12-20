/* A Menu Widget */

#include <string.h>
#include "snazzy.h"
#include "ll.h"

#define USE_DATA  struct widget_data *d = (struct widget_data *)&w->data

static unsigned char ico_norm[] = {
    0b00000000,
    0b11111110,
    0b01000100,
    0b00101000,
    0b00010000,
    0b00000000,
};

typedef struct widget_data * data;

struct widget_data {
    unsigned char state;
    widget *popup;
};


static void draw(widget *w) {
    int x, y;
    x = w->x + w->child->w + 1;
    y = w->y + 1;
    ll_cset(1);
    ll_bar( w->x, w->y, 8, 6);
    ll_cset(0);
    ll_char_draw(x, y, ico_norm);
}

static void layout(widget *w) {
    w->w += 10;
    w->h += 4;
}

static void set(widget *w, widget *c){
    USE_DATA;
    widget *a = d->popup;
    a->x = w->x + c->x;
    a->y = w->y + c->h;
}

static void up(widget *w){
    USE_DATA;
    widget *pop = d->popup;
    pop->flags |= S_HIDDEN;
    ll_cset(1);
    draw_area(pop->x, pop->y, pop->w, pop->h);
    dialog = &root;
}

static void down(widget *w){
}

static void clicked(widget *w){
    USE_DATA;
    widget *pop = d->popup;
    if (d->state) {
	up(w);
	d->state = 0;
    }
    else {
	pop->flags &= ~S_HIDDEN;
	ll_cset(1);
	ll_bar(pop->x, pop->y, pop->w, pop->h);
	ll_cset(0);
	draw_widget(pop);
	dialog = pop;
	d->state = 1;
    }
}

static void move(widget *w){
}

static struct vmt_s widget_vmt = {
    draw,
    layout,
    set,
    down,
    up,
    clicked,
    move,
};



void new_menu(widget *w, widget *g) {
    USE_DATA;
    memset(w,0,sizeof(widget));
    g->flags |= S_HIDDEN;
    g->flags |= S_OVER;
    w->vmt = &widget_vmt;
    w->flags |= S_MOUSE | S_VERT;
    d->popup = g;
}


