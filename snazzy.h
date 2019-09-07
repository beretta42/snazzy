#ifndef SNAZZY_H
#define SNAZZY_H

typedef struct widget_s widget;

struct widget_s {
    int x;
    int y;
    int w;
    int h;
    int type;
    // void (*doev)(widget *w, int ev);
    //    void (*uev)(widget *w, int ev);
    widget *next;
    widget *child;
    char *text;
    int x1;
    int y1;
    int flags;
    int d;
};

/* widget types */
#define TY_VBOX    0
#define TY_HBOX    1
#define TY_LABEL   2
#define TY_BUTTON  3
#define TY_POPLIST 4
#define TY_POPITEM 5
#define TY_HSLIDE  6
#define TY_MAX     7

/* widget flags */
#define FL_CLICKABLE  1        /* this widget can do mouse events */
#define FL_NODRAW     2        /* don't recurse thru children while drawing */ 

/* event types 
  These event are send to from the engine to widgets
*/
#define EV_DRAW   0       // draw 
#define EV_DOWN   1       // pen/mouse down
#define EV_UP     2       // pen/mouse up
#define EV_CLICK  3       // clicked
#define EV_DOUBLE 4       // double clicked
#define EV_MOVE   5       // mouse moved

/* uevent types 
  These are events passed from the graphcial i/o system 
  to the engine
*/
#define UEV_DOWN 0
#define UEV_UP   1
#define UEV_MOVE 2

#endif /* SNAZZY_H */
