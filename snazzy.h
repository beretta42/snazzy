#ifndef SNAZZY_H
#define SNAZZY_H

#include <stdint.h> // fixme: s/b our .h

#define MAX(a,b) (a > b ? a : b)
#define MIN(a,b) (a < b ? a : b)
#define abs(a) (a < 0 ? -a : a)

typedef struct widget_s widget;

struct widget_s {
    int16_t x;
    int16_t y;
    int16_t w;
    int16_t h;
    int16_t type;
    int16_t next;
    int16_t child;
    int16_t flags;
    int16_t text;
    int16_t x1;
    int16_t y1;
    int16_t d;
    int16_t app;
};

typedef void (* appcall_ptr)(widget *w, int ev);



/* widget types */
#define TY_VBOX      0
#define TY_HBOX      1
#define TY_LABEL     2
#define TY_BUTTON    3
#define TY_POPLIST   4
#define TY_POPITEM   5
#define TY_HSLIDE    6
#define TY_PANEL     7
#define TY_MENU      8
#define TY_MENUITEM  9
#define TY_MAX      10

/* widget flags */
#define FL_CLICKABLE   1        /* this widget can do mouse events */
#define FL_NODRAW      2        /* don't draw this widget */
#define FL_NOCHILD     4        /* don't draw this widget's children */

/* event types 
  These event are send to from the engine to widgets
*/
#define EV_DRAW   0       // draw 
#define EV_DOWN   1       // pen/mouse down
#define EV_UP     2       // pen/mouse up
#define EV_CLICK  3       // clicked
#define EV_DOUBLE 4       // double clicked
#define EV_MOVE   5       // mouse moved
#define EV_IN     6       // mouse moved into a widget
#define EV_OUT    7       // mouse moved out of a widget

/* uevent types 
  These are events passed from the graphcial i/o system 
  to the engine
*/
#define UEV_DOWN 0
#define UEV_UP   1
#define UEV_MOVE 2

/* these are high level event send from engine to app */
#define AEV_SELECT 0

/* ??? fixme: s/b named different? */
void ll_puts(int x, int y, char *t);
void ll_draw_back(int x, int y, int w, int h);
void draw_all(widget *w);
void draw_children(widget *w);
widget *collide_all(widget *head, int x, int y);
void draw_coll(widget *w);
void bound(widget *w);
void do_event(widget *w, int ev);
char *gpt(int o);
widget *gp(int o);
void send_uevent(int e, int x, int y);
void draw_all(widget *w);
void draw_back(widget *w);
void bounce(widget *w);
void do_appcall(widget *w, int ev);
void push_focus(widget *w);
void pull_focus(void);
int load(char *name);
void fixup(widget *w);
unsigned int  ll_getticks(void);
int reg_appcall(char *id, appcall_ptr m);
int ll_init(void);
void ll_deinit(void);
void ll_loop(void);
widget *find_widget(char *id);
widget *child_by_index(widget *w, int i);

/* widget event handlers */
void do_label(widget *w, int ev);
void do_button(widget *w, int ev);
void do_poplist(widget *w, int ev);
void do_popitem(widget *w, int ev);
void do_hslide(widget *w, int ev);
void do_panel(widget *w, int ev);
void do_menu(widget *w, int ev);
void do_menuitem(widget *w, int ev);

int get_hslide(widget *w);
void select_panel(widget *w, int sel);

extern widget *mwidget;
extern widget *focus;
extern widget *down;
extern widget *lmove;
extern int bx1;
extern int by1;
extern int bx2;
extern int by2;
extern int bw;
extern int bh;
extern int mx;
extern int my;
extern int drawf;
extern unsigned char databuffer[8192];

#endif /* SNAZZY_H */
