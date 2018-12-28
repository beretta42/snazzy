/* a check box widget */

#include "snazzy.h"
#include "tgi.h"

static unsigned char check_ico_up[] = {
    0b11111100,
    0b10000100,
    0b10000100,
    0b10000100,
    0b10000100,
    0b11111100,
};
    
static unsigned char check_ico_set[] = {
    0b00000110,
    0b00001100,
    0b11011000,
    0b01110000,
    0b00110000,
    0b00000000,
};

static unsigned char check_ico_down[] = {
    0b11111100,
    0b10101100,
    0b11000100,
    0b10001100,
    0b11010100,
    0b11111100,
};

struct check_data {
    unsigned char state;
};

static void layout_check(widget *w){
    w->w += 10;
    w->h += 4;
}

static void draw_check(widget *w) {
    ll_cset(1);
    ll_bar( w->x, w->y, 8, 6);
    ll_cset(0);
    ll_char_draw(w->x+1, w->y+1, check_ico_up);
    if (w->data[0])
	ll_char_draw(w->x+1, w->y+1, check_ico_set);
}

static void down_check(widget *w){
    ll_cset(1);
    ll_bar( w->x, w->y, 8, 6);
    ll_cset(0);
    ll_char_draw(w->x+1, w->y+1, check_ico_down);
}

static void clicked_check(widget *w){
    struct check_data *d = (struct check_data *)&w->data;
    d->state = ~d->state;
    draw_check(w);
}

static void set_check(widget *w, widget *c){
    c->x += 10;
}

static struct vmt_s check_vmt = {
    draw_check,
    layout_check,
    set_check,
    down_check,
    draw_check,
    clicked_check,
    noop,
};

void new_check(widget *w) {
    w->vmt = &check_vmt;
    w->flags |= S_MOUSE;
}
