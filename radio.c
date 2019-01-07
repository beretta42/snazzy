/* A Radio Button */

#include <stdio.h>
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


static void draw(widget *w) {
    ll_cset(1);
    ll_bar( w->x, w->y, 8, 6);
    ll_cset(0);
    ll_char_draw(w->x+1, w->y+1, ico_up);
    if (w->data.radio.state)
	ll_char_draw(w->x+1, w->y+1, ico_set);
}

static void layout(widget *w) {
    w->w += 10;
    w->h += 4;
}

static void set(widget *w, widget *c){
    c->x += 10;
}

static void compile(widget *w) {
    printf("\t&radio_vmt,\n");
    printf("\t.data.radio = {\n");
    printf("\t\t%d,\n", w->data.radio.state);

    if (w->data.radio.sel)
	printf("\t\t&%s,\n", w->data.radio.sel->ct->name);
    else
	printf("\t\tNULL,\n");

    if (w->data.radio.group)
	printf("\t\t&%s,\n", w->data.radio.group->ct->name);
    else
	printf("\t\tNULL,\n");

    if (w->data.radio.clicked)
	printf("\t\t%s_clicked,\n", w->ct->name);
    else
	printf("\t\tNULL,\n");
    
    printf("\t},\n");
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
    widget *grp = w->data.radio.group;
    widget *sel = grp->data.radio.sel;
    sel->data.radio.state = 0;
    if (sel->data.radio.clicked)
	sel->data.radio.clicked(sel, sel->data.radio.state);
    draw(sel);
    grp->data.radio.sel = w;
    w->data.radio.state = -1;
    if (w->data.radio.clicked)
	w->data.radio.clicked(w, w->data.radio.state);
    draw(w);
}


struct vmt_s radio_vmt = {
    draw,
    down,
    up,
    clicked,
    noop,
    noop,
};


void new_radio(widget *w,
	       widget *g,
	       void (*cb)(widget *w, unsigned char state)) {
    struct radio_data *d = (struct radio_data *)&w->data;
    w->vmt = &radio_vmt;
    w->flags |= S_MOUSE;
    w->ct->layout = layout;
    w->ct->set = set;
    w->ct->compile = compile;
    d->clicked = cb;
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
