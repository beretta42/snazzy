#ifndef SNAZZY_H
#define SNAZZY_H

typedef struct widget_s widget;

void draw_widget(widget *w);
void hide_widget(widget *w);
void draw_area(int x, int y, int w, int h);
void set_widget(widget *w, int x, int y);
void layout_widget(widget *w);
void pack_widget(widget *p, widget *c);

extern widget root;
extern widget *dialog;


struct vmt_s {
    void (*draw)(widget *w);
    void (*layout)(widget *w);
    void (*set)(widget *w, widget *c);
    void (*down)(widget *w);
    void (*up)(widget *w);
    void (*clicked)(widget *w);
    void (*move)(widget *w);
};
void noop(widget *w);
void set_noop(widget *w, widget *c);


struct widget_s {
    widget *parent;
    widget *sib;
    widget *child;
    struct vmt_s *vmt;
    int flags;
#define S_VERT   1      /* set if vertical layout, clear if horizontal */
#define S_MOUSE  2      /* do we handle mouse events? */
#define S_HIDDEN 4      /* don't draw me */
#define S_OVER   8      /* don't take in account my size when laying out*/
    int x;
    int y;
    int w;
    int h;
    char data[64];      /* fixme: make much smaller for 16 bit machines */
};

void new_radio(widget *w, widget *g);
void new_menu(widget *w, widget *g);
void new_area(widget *w);

#endif /* SNAZZY_H */
