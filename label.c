/* a label widget */

#include <string.h>
#include "snazzy.h"
#include "tgi.h"

/* fixme: no, no, no */
#include "/home/beretta/C/platotermCoCo/src/coco2/font.c"

struct label_data {
    char *text;
};

//void draw_label(widget *w);
//void layout_label(widget *w);
//void set_label(widget *w, widget *c);

static void draw_label(widget *w) {
    struct label_data *d = (struct label_data *)&w->data;
    char *s = d->text;
    int x = w->x + 1;
    int y = w->y + 1;
    int c;
    while (c = *s++) {
	ll_char_draw(x, y, font + (c-32)*6);
	x += 4;
    }
}

static void set_label(widget *w, widget *c){
}

static void layout_label(widget *w) {
    struct label_data *d = (struct label_data *)&w->data;
    w->h = 6;
    w->w = strlen(d->text) * 4 + 2;
}


struct vmt_s label_vmt = {
    draw_label,
    layout_label,
    set_label,
    noop,
    noop,
    noop,
    noop,
};


void new_label(widget *w, char *s) {
    struct label_data *d = (struct label_data *)&w->data;
    d->text = s;
    w->vmt = &label_vmt;
}



