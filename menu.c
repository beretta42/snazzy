/* A Menu Widget */

#include <stdio.h>
#include <string.h>
#include "snazzy.h"
#include "ll.h"

static unsigned char ico_norm[] = {
    0b00000000,
    0b11111110,
    0b01000100,
    0b00101000,
    0b00010000,
    0b00000000,
};

typedef struct radio_data * data;

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
    widget *a = w->data.menu.popup;
    a->x = w->x + c->x;
    a->y = w->y + c->h;
}

static void compile(widget *w) {
    printf("\t&menu_vmt,\n");
    printf("\t.data.menu = {\n");
    printf("\t\t%d,\n", w->data.menu.state);
    if (w->data.menu.popup)
	printf("\t\t&%s,\n", w->data.menu.popup->ct->name);
    else
	printf("\t\tNULL,\n");
    printf("\t},\n");

}

static void up(widget *w){
    widget *pop = w->data.menu.popup;
    pop->flags |= S_HIDDEN;
    ll_cset(1);
    draw_area(pop->x, pop->y, pop->w, pop->h);
    dialog = root;
}

static void down(widget *w){
}

static void clicked(widget *w){
    widget *pop = w->data.menu.popup;
    pop->flags &= ~S_HIDDEN;
    ll_cset(1);
    ll_bar(pop->x, pop->y, pop->w, pop->h);
    ll_cset(0);
    draw_widget(pop);
    dialog = pop;
    w->data.menu.state = 1;
}

static void move(widget *w){
}

struct vmt_s menu_vmt = {
    draw,
    down,
    up,
    clicked,
    move,
    noop,
};



void new_menu(widget *w, widget *g) {
    g->flags |= S_HIDDEN;
    g->flags |= S_OVER;
    w->vmt = &menu_vmt;
    w->flags |= S_MOUSE | S_VERT;
    w->ct->layout = layout;
    w->ct->set = set;
    w->ct->compile = compile;
    w->data.menu.popup = g;
}


