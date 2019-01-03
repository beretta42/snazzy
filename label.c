/* a label widget */

#include <stdio.h>
#include <string.h>
#include "snazzy.h"
#include "tgi.h"

/* fixme: no, no, no */
#include "/home/beretta/C/platotermCoCo/src/coco2/font.c"


void draw(widget *w) {
    const char *s = w->data.label.text;
    int x = w->x + 1;
    int y = w->y + 1;
    int c;
    while (c = *s++) {
	ll_char_draw(x, y, font + (c-32)*6);
	x += 4;
    }
}

static void set(widget *w, widget *c){
}

static void layout(widget *w) {
    struct label_data *d = (struct label_data *)&w->data;
    w->h = 6;
    w->w = strlen(d->text) * 4 + 2;
}


static void compile(widget *w) {
    printf("\t&label_vmt,\n");
    printf("\t(struct label_data){\n");
    printf("\t\t%s_text,\n", w->ct->name);
    printf("\t},\n");
}

struct vmt_s label_vmt = {
    draw,
    noop,
    noop,
    noop,
    noop,
    noop,
};


void new_label(widget *w, char *s) {
    w->data.label.text = s;
    /* compile here */
    printf("#define %s_text  \"%s\"\n", w->ct->name, s);
    w->vmt = &label_vmt;
    w->ct->layout = layout;
    w->ct->set = set;
    w->ct->compile = compile;
}



