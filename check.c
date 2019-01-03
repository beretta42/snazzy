/* a check box widget */

#include <stdio.h>
#include "snazzy.h"
#include "tgi.h"

static unsigned char ico_up[] = {
    0b11111100,
    0b10000100,
    0b10000100,
    0b10000100,
    0b10000100,
    0b11111100,
};
    
static unsigned char ico_set[] = {
    0b00000110,
    0b00001100,
    0b11011000,
    0b01110000,
    0b00110000,
    0b00000000,
};

static unsigned char ico_down[] = {
    0b11111100,
    0b10101100,
    0b11000100,
    0b10001100,
    0b11010100,
    0b11111100,
};

static void layout(widget *w){
    w->w += 10;
    w->h += 4;
}

static void draw(widget *w) {
    ll_cset(1);
    ll_bar( w->x, w->y, 8, 6);
    ll_cset(0);
    ll_char_draw(w->x+1, w->y+1, ico_up);
    if (w->data.check.state)
	ll_char_draw(w->x+1, w->y+1, ico_set);
}

static void down(widget *w){
    ll_cset(1);
    ll_bar( w->x, w->y, 8, 6);
    ll_cset(0);
    ll_char_draw(w->x+1, w->y+1, ico_down);
}

static void clicked(widget *w){
    struct check_data *d = (struct check_data *)&w->data;
    d->state = ~d->state;
    draw(w);
}

static void set(widget *w, widget *c){
    c->x += 10;
}

static void compile(widget *w) {
    printf("\t&check_vmt,\n");
    printf("\t.data.check = {\n");
    printf("\t\t%d,\n", w->data.check.state);
    printf("\t},\n");
}

struct vmt_s check_vmt = {
    draw,
    down,
    draw,
    clicked,
    noop,
    noop,
};

void new_check(widget *w) {
    w->vmt = &check_vmt;
    w->flags |= S_MOUSE;
    w->ct->layout = layout;
    w->ct->set = set;
    w->ct->compile = compile;
}
