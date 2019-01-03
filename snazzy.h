#ifndef SNAZZY_H
#define SNAZZY_H

typedef struct widget_s widget;
typedef struct widget_ct_s widget_ct;
typedef union widget_data_u widget_data;

extern widget myroot;

#include "button.h"
#include "label.h"
#include "check.h"
#include "radio.h"
#include "menu.h"
#include "area.h"

void draw_widget(widget *w);
void hide_widget(widget *w);
void draw_area(int x, int y, int w, int h);
void set_widget(widget *w, int x, int y);
void layout_widget(widget *w);
void pack_widget(widget *p, widget *c);
widget *alloc_widget(char *name);
void snazzy_go(void);
int s_init(int w, int h);

extern widget *root;
extern widget *dialog;


union widget_data_u {
    struct label_data label;
    struct radio_data radio;
    struct menu_data menu;
    struct check_data check;
    struct button_data button;
};

struct vmt_s {
    void (*draw)(widget *w);
    void (*down)(widget *w);
    void (*up)(widget *w);
    void (*clicked)(widget *w);
    void (*move)(widget *w);
    void (*key)(widget *w);
};
void noop(widget *w);
void set_noop(widget *w, widget *c);

struct widget_ct_s {
    unsigned char type;           /* type of widget */
#define WT_LABEL  0
#define WT_BUTTON 1
#define WT_CHECK  2
    widget *rt;                   /* ptr to run-time */
    char *name;                   /* ptr to unique compiler label */
    void (*layout)(widget *w);    /* ptr to layout method */
    void (*set)(widget *w, widget *p);       /* ptr to set method */
    void (*compile)(widget *w);   /* ptr to compile method */
};

struct widget_s {
    widget *parent;
    widget *sib;
    widget *child;
    int flags;
    /* fixme: some of these flags maybe belong in widget_ct */
#define S_VERT   1      /* set if vertical layout, clear if horizontal */
#define S_MOUSE  2      /* do we handle mouse events? */
#define S_HIDDEN 4      /* don't draw me */
#define S_OVER   8      /* don't take in account my size when laying out*/
    int x;
    int y;
    int w;
    int h;
    struct vmt_s *vmt;
    widget_data data;   /* class data */
    struct widget_ct_s *ct;
};

void new_radio(widget *w, widget *g);
void new_menu(widget *w, widget *g);
void new_area(widget *w);
void new_button(widget *w, void (*cb)(widget *w));
void new_label(widget *w, char *s);
void new_check(widget *w);

#endif /* SNAZZY_H */
