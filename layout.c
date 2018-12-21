/*

Layout / Moving

*/

#include "string.h"
#include "snazzy.h"


static int sp = 0;
static widget widgets[256];

widget * alloc_widget(char *name){
    widget *new = &widgets[sp++];
    memset(new, 0, sizeof(widget));
    new->name = name;
    return new;
}

void pack_widget(widget *p,widget *c){
    /* setup child */
    c->sib = NULL;
    c->parent = p;
    /* find last */
    widget *ptr = p->child;
    widget *last = NULL;
    while (ptr){
	last = ptr;
	ptr = ptr->sib;
    }
    if (last == NULL){
	p->child = c;
    }
    else{
	last->sib = c;
    }
}



void set_widget(widget *w, int x, int y) {
    int a;
    w->x = x;
    w->y = y;
    widget *p = w->child;

    if (w->flags & S_VERT){
	a = y;
	while (p) {
	    set_widget(p, x, a);
	    if (p->flags & S_OVER) {
	    }
	    else {
		a += p->h;
	    }
	    w->vmt->set(w,p);
	    p = p->sib;
	}
    }
    else {
	a = x;
	while (p) {
	    set_widget(p, a, y);
	    if (p->flags & S_OVER) {
	    }
	    else {
		a += p->w;
	    }
	    w->vmt->set(w,p);
	    p = p->sib;
	}
    }
}


void layout_widget(widget *w) {
    widget *p = w->child;
    int max = 0;
    while (p) {
	layout_widget(p);
	if (p->flags & S_OVER){
	    /* this is an overlay, don't take in account it's dims */
	}
	else {
	    if (w->flags & S_VERT){
		w->h += p->h;
		if (p->w > max) max = p->w;
	    }
	    else {
		w->w += p->w;
		if (p->h > max) max = p->h;
	    }
	}
	p = p->sib;
    }
    if (w->flags & S_VERT)
	w->w = max;
    else
	w->h = max;
    w->vmt->layout(w);
}






