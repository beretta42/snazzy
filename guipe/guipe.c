#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>

#define MAX(a,b) (a > b ? a : b);
#define VPAD 2
#define HPAD 2

/* input buffers */
char wordb[80];
char buf[512];
int len = 0;
int pos = 0;

typedef struct widget_s widget;

enum {
    VBOX,
    HBOX,
    LABEL,
    BUTTON,
    POPLIST,
    POPITEM,
    HSLIDE,
    PANEL,
    MENU,
    MENUITEM,
    WINDOW,
    TEXT,
};

/*This an internal compile-time widget structure,
  not the one used at run-time.
 */ 
struct widget_s {
    int type;
    int x;
    int y;
    int w;
    int h;
    char *text;
    char *ctext;
    int d;
    widget *parent;
    widget *next;
    widget *child;
    void (* vsize)(widget *w);
    void (* vset)(widget *w, int height);
    void (* vpos)(widget *w, int x, int y);
    void (* hsize)(widget *w);
    void (* hset)(widget *w, int width);
    int vno;
    int vused;
    int hno;
    int hused;
    int flags;
    int x1;
    int y1;
    int rt_flags;
    int off;    /* data file offset to widget */
    int toff;   /* data file offset to text string */
};

#define MAXW   1      /* maximize width to parent */
#define MAXH   2      /* maximize height to parent */
#define HALIGN 12     /* horizontal alignment mask */
#define HCENTER 0
#define HLEFT   4
#define HRIGHT  8
#define VALIGN 48     /* verical alignment mask */
#define VCENTER 0
#define VTOP  16
#define VBOTTOM 32


#define RT_CLICKABLE 1  /* this widget can handle */
#define RT_NODRAW    2  /* don't let engine redraw this widget's children */
#define RT_NOCHILD   4  /* don't draw this widget's children */

widget *tails[16];
int tailptr = 0;
widget *cur;
widget *head;
widget *tail;
widget *parent;
int fontw = 4;
int fonth = 6;
int ucounter = 0;


/* returns child widget indexed by i */
widget *child_by_index(widget *w, int i) {
    widget *n = w->child;
    while(i-- && n) n = n->next;
    return n;
}

/* returns an allocated, unique string for unnamed
   widgets */
char *unique(void) {
    static char buf[11];
    sprintf(buf, "noname%d", ucounter++);
    return strdup(buf);
}

int isws(char c) {
    switch (c) {
    case ' ':
    case '\t':
    case '\r':
    case '\n':
	return 1;
    default:
	return 0;
    }    
}

int isquote(char c) {
    if (c == '"') return 1;
    return 0;
}

int refill_buffer(void) {
    if (len == 0) {
	len = read(0, buf, 512);
	pos = 0;
	if (len == 0) return 1;
    }
    return 0;
}

int getin(void) {
    if (refill_buffer()) return 0;
    len--;
    return buf[pos++];
}

int peekin(void) {
    if (refill_buffer()) return 0;
    return buf[pos];
}

void getquote(char c) {
    char *d = wordb;
    char n;
    while (1) {
	n = getin();
	if (n == c) break; 
	*d++ = n;
    }
    *d = 0;
}


char *word() {
    char *d = wordb;
    int c;
    // skip whitespace
    while (isws(c = getin()));
    if (isquote(c))
	getquote(c);
    else {
	// append until whitespace
	do *d++ = c;
	while (!isws(c = getin()) && c);
	*d = 0;
    }
    return wordb;
}

int getnumber(void) {
    char *end;
    int ret;
    ret = strtol(word(), &end, 0);
    if (end == wordb || *end) {
	fprintf(stderr,"not a number: %s\n", wordb);
	exit(1);
    }
    return ret;
}

char *getstr(void) {
    return strdup(word());
}
    
void do_comment() {
    while (getin() != '\n');
}

// allocates new widget, links it into tree
void new_widget() {
    widget *new = calloc(1,sizeof(widget));
    if (new == NULL) {
	perror("malloc new widget");
	exit(1);
    }
    new->parent = parent;
    if (tail) {
	tail->next = new;
    } else {
	if(cur) cur->child = new;
    }
    tail = new;
    cur = new;
    if (head == NULL) head = new;
}

// apply justification to text
void apply_just(widget *w) {
    switch(w->flags & HALIGN) {
    case HCENTER:
	w->x1 = w->x + w->w/2 - strlen(w->text) * 4 / 2;
	break;
    case HLEFT:
	w->x1 = w->x;
	break;
    case HRIGHT:
	w->x1 = w->x + w->w - strlen(w->text) * 4;
	break;
    }
    switch(w->flags & VALIGN) {
    case VCENTER:
	w->y1 = w->y + w->h/2 - 6 / 2;
	break;
    case VTOP:
	w->y1 = w->y;
	break;
    case VBOTTOM:
	w->y1 = w->y + w->h - 6;
    }
}

/*
     Vertical Box Layout
*/

void vset_vbox(widget *w, int height) {
    widget *n;
    int m = w->vno;
    int h = w->vused;
    int e;

    if (m) { 
	e = (height - h) / m;
	for (n = w->child; n; n = n->next) {
	    if (n->h) continue;
	    n->vset(n, e);
	}
    }
    w->h = height;
}

void vsize_vbox(widget *w) {
    widget *n;
    int m = 0;
    int h = 0;
    // size all the minimum child widgets
    // count number of max sized widgets
    for (n = w->child; n; n = n->next) {
	n->vsize(n);
	if (n->h == 0) {
	    m++;
	    continue;
	}
	h += n->h + VPAD;
    }
    // pass handy values to vset
    w->vused = h;
    w->vno = m;
    if (w->h == 0 && m == 0 && !(w->flags & MAXH)) {
	w->h = h;
    }
}


void hsize_vbox(widget *w) {
    widget *n;
    int m = 0;
    int h = 0;
    for(n = w->child; n; n = n->next) {
	n->hsize(n);
	if (n->w == 0) {
	    m++;
	    continue;
	}
	h = MAX(h, n->w);
    }
    if (w->w == 0 && m == 0 && !(w->flags & MAXW)) {
	w->w = h;
    }
}

void vpos_vbox(widget *w, int x, int y) {
    widget *n;
    w->y = y;
    w->x = x;
    for (n = w->child; n; n = n->next) {
	n->vpos(n, x, y);
	y += n->h + VPAD;
    }
}

void hset_vbox(widget *w, int width) {
    widget *n;
    for (n = w->child; n; n = n->next) {
	if (n->w) continue;
	n->hset(n, width);
    }
    w->w = width;
}

void do_vbox() {
    new_widget();
    cur->type = VBOX;
    cur->vsize = vsize_vbox;
    cur->vset = vset_vbox;
    cur->vpos = vpos_vbox;
    cur->hsize = hsize_vbox;
    cur->hset = hset_vbox;
    cur->ctext = unique();
}


/* 
   Horitonal Box Layout
*/

void vset_hbox(widget *w, int height) {
    widget *n;
    for (n = w->child; n; n = n->next) {
	if (n->h) continue;
	n->vset(n, height);
    }
    w->h = height;
}

void vsize_hbox(widget *w) {
    widget *n;
    int m = 0;
    int h = 0;
    for (n = w->child; n; n = n->next) {
	n->vsize(n);
	if (n->h == 0) {
	    m++;
	    continue;
	}
	if (n->h > h ) h = n->h;
    }
    if (w->h == 0 && m == 0 && !(w->flags & MAXH)) {
	w->h = h;
    }
}

void hsize_hbox(widget *w) {
    widget *n;
    int m = 0;
    int h = 0;
    // size all the minimum child widgets
    // count number of max sized widgets
    for (n = w->child; n; n = n->next) {
	n->hsize(n);
	if (n->w == 0) {
	    m++;
	    continue;
	}
	h += n->w;
    }
    // pass handy value to hset
    w->hused = h;
    w->hno = m;
    if (w->w == 0 && m == 0 && !(w->flags & MAXW)) {
	w->w = h;
    }
}

void vpos_hbox(widget *w, int x, int y) {
    widget *n;
    w->y = y;
    w->x = x;
    for (n = w->child; n; n = n->next) {
	n->vpos(n, x, y);
	x += n->w;
    }
}

void hset_hbox(widget *w, int width) {
    widget *n;
    int m = w->hno;
    int h = w->hused;
    int e;

    if (m) {
	e = (width - h) / m;
	for (n = w->child; n; n = n->next) {
	    if (n->w) continue;
	    n->hset(n, e);
	}
    }
    w->w = width;
}

void do_hbox() {
    new_widget();
    cur->type = HBOX;
    cur->vsize = vsize_hbox;
    cur->vset = vset_hbox;
    cur->vpos = vpos_hbox;
    cur->hsize = hsize_hbox;
    cur->hset = hset_hbox;
    cur->ctext = unique();
}



/* 
   Label Widget 

*/

void vsize_label(widget *w) {
    if (w->flags & MAXH) return;
    if (w->h) return;
    w->h = 10;
}

void vset_label(widget *w, int height) {
    w->h = height;
}

void vpos_label(widget *w, int x, int y) {
    w->y = y;
    w->x = x;
    apply_just(w);
}

void hsize_label(widget *w) {
    if (w->flags & MAXW) return;
    w->w = strlen(w->text) * fontw;
}

void hset_label(widget *w, int width) {
    w->w = width;
}

void do_label() {
    new_widget();
    cur->type = LABEL;
    cur->vsize = vsize_label;
    cur->vset = vset_label;
    cur->vpos = vpos_label;
    cur->hsize = hsize_label;
    cur->hset = hset_label;
    cur->text = getstr();
    cur->ctext = unique();
    //cur->rt_flags = RT_CLICKABLE;
}

/*
   Button Widget 
*/

void vsize_button(widget *w) {
    if (w->flags & MAXH) return;
    w->h = fonth + 6;
}

void vset_button(widget *w, int height) {
    w->h = height;
}

void hsize_button(widget *w) {
    if (w->flags & MAXW) return;
    w->w = strlen(w->text) * fontw + 6;
}

void hset_button(widget *w, int width) {
    w->w = width;
}

void vpos_button(widget *w, int x, int y) {
    w->y = y;
    w->x = x;
    apply_just(w);
}


void do_button() {
    new_widget();
    cur->type = BUTTON;
    cur->vsize = vsize_button;
    cur->vset = vset_button;
    cur->vpos = vpos_button;
    cur->hsize = hsize_button;
    cur->hset = hset_button;
    cur->ctext = getstr();
    cur->text = getstr();
    cur->rt_flags = RT_CLICKABLE;
}


/* 
   Pop-Up List widget

*/

void vsize_pop(widget *w) {
    if (w->flags & MAXH) return;
    vsize_vbox(w);
    w->h = fonth + 6;
}

void vset_pop(widget *w, int height) {
    vset_vbox(w, height);
}

void vpos_pop(widget *w, int x, int y) {
    vpos_vbox(w, x, y+w->h);
    w->y = y;
    apply_just(w);
    // use this as a place to iterate index values into children
    widget *n;
    int i = 0;
    n = child_by_index(w, w->d);
    w->toff = n->toff;
    w->text = n->text;
    for (n = w->child; n; n = n->next) {
	n->d = i++;
	n->w = w->w;
    }
}

void hsize_pop(widget *w) {
    hsize_vbox(w);
    w->w += 10;
}

void hset_pop(widget *w, int width){
    hset_vbox(w, width);
    w->w = width;
}

void do_poplist() {
    new_widget();
    cur->type = POPLIST;
    cur->vsize = vsize_pop;
    cur->vset = vset_pop;
    cur->vpos = vpos_pop;
    cur->hsize = hsize_pop;
    cur->hset = hset_pop;
    cur->ctext = getstr();
    cur->rt_flags = RT_CLICKABLE | RT_NOCHILD;
    cur->flags |= HLEFT;
}


void vpos_popitem(widget *w, int x, int y) {
    vpos_label(w, x+2 , y+1);
    w->x1 += 2;
}

void do_popitem() {
    new_widget();
    cur->type = POPITEM;
    cur->vsize = vsize_label;
    cur->vset = vset_label;
    cur->vpos = vpos_popitem;
    cur->hsize = hsize_label;
    cur->hset = hset_label;
    cur->ctext = unique();
    cur->text = getstr();
    cur->rt_flags = RT_CLICKABLE;
    cur->flags |= HLEFT;
}


void vsize_hslide(widget *w) {
    w->h = 10;
}

void vset_hslide(widget *w, int height) {
    w->h = height;
}

void hsize_hslide(widget *w) {
}

void hset_hslide(widget *w, int width) {
    w->w = width;
}

void vpos_hslide(widget *w, int x, int y) {
    w->x = x;
    w->y = y;
}

void do_hslide() {
    new_widget();
    cur->type = HSLIDE;
    cur->vsize = vsize_hslide;
    cur->vset = vset_hslide;
    cur->vpos = vpos_hslide;
    cur->hsize = hsize_hslide;
    cur->hset = hset_hslide;
    cur->ctext = getstr();
    cur->rt_flags = RT_CLICKABLE;
}


void vsize_panel(widget *w) {
    widget *n;
    int max = w->h;
    for (n = w->child; n; n = n->next) {
	n->vsize(n);
	max = MAX(n->h,max);
    }
    if (w->h || (w->flags & MAXH)) return;
    w->h = max;
}

void vset_panel(widget *w, int height) {
    widget *n;
    w->h = height;
    for (n = w->child; n; n = n->next) {
	n->vset(n, height);
    }
}

void hsize_panel(widget *w) {
    widget *n;
    int max = w->w;
    for (n = w->child; n; n = n->next) {
	n->hsize(n);
	max = MAX(n->w,max);
    }
    if (w->w || (w->flags & MAXW)) return;
    w->w = max;
}

void hset_panel(widget *w, int width) {
    widget *n;
    w->w = width;
    for (n = w->child; n; n = n->next) {
	n->hset(n, width);
    }
}

void vpos_panel(widget *w, int x, int y) {
    widget *n;
    w->x = x;
    w->y = y;
    for (n = w->child; n; n = n->next) {
	n->vpos(n, x, y);
	n->rt_flags |= RT_NODRAW;
    }
    child_by_index(w,w->d)->rt_flags &= ~RT_NODRAW;
}

/* panel widget */
void do_panel() {
    new_widget();
    cur->type = PANEL;
    cur->vsize = vsize_panel;
    cur->vset = vset_panel;
    cur->vpos = vpos_panel;
    cur->hsize = hsize_panel;
    cur->hset = hset_panel;
    cur->ctext = getstr();
}

/* 
   menu widget */


void vpos_menu(widget *w, int x, int y) {
    int toff = w->toff;
    char *text = w->text;
    vpos_pop(w,x,y);
    w->toff = toff;
    w->text = text;
}

void do_menu() {
    do_poplist();
    cur->text = getstr();
    cur->type = MENU;
    cur->vpos = vpos_menu;
}

void do_menuitem() {
    do_popitem();
    cur->type = MENUITEM;
}

/*
  Window widget */

void vsize_window(widget *w) {
    w->h = 8;
}

void vset_window(widget *w, int height) {
    w->h = height;
}

void hsize_window(widget *w) {
}

void hset_window(widget *w, int width) {
    w->w = width;
}

void vpos_window(widget *w, int x, int y) {
    w->x = x;
    w->y = y;
    apply_just(w);
}

void do_window() {
    new_widget();
    cur->type = WINDOW;
    cur->vsize = vsize_window;
    cur->vset = vset_window;
    cur->vpos = vpos_window;
    cur->hsize = hsize_window;
    cur->hset = hset_window;
    cur->ctext = getstr();
    cur->text = getstr();
    cur->rt_flags = RT_CLICKABLE;
}


void vsize_text(widget *w) {
    w->h = 10;
}

void vset_text(widget *w, int height) {
    w->h = height;
}

void hsize_text(widget *w) {

}

void hset_text(widget *w, int width) {
    w->w = width;
}

void vpos_text(widget *w, int x, int y) {
    w->x = x;
    w->y = y;
}

    
void do_text() {
    new_widget();
    cur->type = TEXT;
    cur->vsize = vsize_text;
    cur->vset = vset_text;
    cur->vpos = vpos_text;
    cur->hsize = hsize_text;
    cur->hset = hset_text;
    cur->ctext = getstr();
    cur->text = getstr();
    cur->rt_flags = RT_CLICKABLE;
}


/*    ui tie-ins 


*/

void do_width() {
    cur->w = getnumber();
}

void do_height() {
    cur->h = getnumber();
}


void do_open() {
    parent = cur;
    tails[tailptr++] = tail;
    tail = NULL;
}

void do_close() {
    cur = cur->parent;
    parent = cur->parent;
    tail = tails[--tailptr];
}

void do_maxh() {
    cur->flags |= MAXH;
}

void do_maxw() {
    cur->flags |= MAXW;
}

void do_max() {
    cur->flags |= MAXH | MAXW;
}

void do_xloc() {
    cur->x = getnumber();
}

void do_yloc() {
    cur->y = getnumber();
}

void do_left() {
    cur->flags |= HLEFT;
}

void do_right() {
    cur->flags |= HRIGHT;
}
 
void do_top() {
    cur->flags |= VTOP;
}

void do_bottom() {
    cur->flags |= VBOTTOM;
}

void do_valmax() {
    cur->x1 = getnumber();
}

void do_value() {
    cur->d = getnumber();
}

typedef struct {
    char *txt;
    void (* call)(void);
} command_t;


command_t cmds[] = {
    { "vbox",        do_vbox },
    { "hbox",        do_hbox },
    { "width",       do_width },
    { "height",      do_height },
    { "label",       do_label },
    { "poplist",     do_poplist },
    { "popitem",     do_popitem },
    { "hslide",      do_hslide },
    { "panel",       do_panel },
    { "{",           do_open },
    { "}",           do_close },
    { "maxh",        do_maxh },
    { "maxw",        do_maxw },
    { "max",         do_max },
    { "xloc",        do_xloc },
    { "yloc",        do_yloc },
    { "left",        do_left },
    { "right",       do_right },
    { "top",         do_top },
    { "bottom",      do_bottom },
    { "button",      do_button },
    { "valmax",      do_valmax },
    { "value",       do_value },
    { "menu",        do_menu },
    { "menuitem",    do_menuitem },
    { "window",      do_window },
    { "text",        do_text },
    { NULL, NULL },
};



void print_widget(widget *w) {
    static const char nstr[] = "NULL";
    printf("%s %s p>%s\n",
	   w->ctext ? w->ctext : nstr,
	   w->text ? w->text : nstr,
	   w->parent ? w->parent->ctext : nstr
	   );
    printf("\t%d %d %d %d\n",
	   w->x, w->y, w->w, w->h);
}

void hprint_widget(widget *w) {
    static const char nstr[] = "NULL";
    char *t;
    printf("widget %s = {", w->ctext);
    // x,y,w,h
    printf("%d, %d, %d, %d, ",
	   w->x, w->y, w->w, w->h);
    // type
    printf("%d, ", w->type);
    // user ev
    //printf("do_%s, ", w->ctext); 
    // next
    if (w->next) {
	printf("&%s, ", w->next->ctext);
    }
    else {
	printf("NULL, ");
    }
    // child
    if (w->child) {
	printf("&%s, ", w->child->ctext);
    }
    else {
	printf("NULL, ");
    }
    // run-time flags
    printf("%d, ", w->rt_flags);
    // text
    if (w->text) {
	printf("\"%s\", ", w->text);
    }
    else {
	printf("NULL, ");
    }
    // alginment x,y
    printf("%d, %d, ", w->x1, w->y1);
    // run time integer data
    printf("%d, ", w->d);
    // tail
    printf("}; \n");
}

// print widget out as a datafile
uint8_t datafile[8192];
uint16_t oindex = 0;
void dputi(int i) {
    if (oindex > 8192 - 2) {
	fprintf(stderr,"output buffer exhausted");
	exit(1);
    }
    datafile[oindex++] = i>>8;
    datafile[oindex++] = i & 255;
}
void dprint_widget(widget *w) {
    w->off = oindex;    /* set widget's offset in data file */
    dputi(w->x);
    dputi(w->y);
    dputi(w->w);
    dputi(w->h);
    dputi(w->type);
    dputi(0); // next ptr
    dputi(0); // child ptr
    dputi(w->rt_flags);
    dputi(0); // text ptr
    dputi(w->x1);
    dputi(w->y1);
    if (w->type == HSLIDE) {
	dputi(w->x + (w->d * w->w / w->x1) - 3);
    }
    else dputi(w->d);
    dputi(0); // app call reference (to be filled out by runtime)
}
// adjust widget pointers
void dadjust(widget *w) {
    widget *n;
    if (w->next) {
	datafile[w->off+10] = w->next->off >> 8;
	datafile[w->off+11] = w->next->off & 255;
    }
    if (w->child) {
	datafile[w->off+12] = w->child->off >> 8;
	datafile[w->off+13] = w->child->off & 255;
    }
    if (w->text) {
	datafile[w->off+16] = w->toff >> 8;
	datafile[w->off+17] = w->toff & 255;
    }
    for (n = w->child; n; n = n->next)
	dadjust(n);
}
// layout strings
void dstring(widget *w) {
    widget *n;
    char *s,*d;
    s = w->text;
    d = datafile + oindex;
    if (s) {
	w->toff = oindex;
	strcpy(d,s);
	oindex += strlen(s) + 1;
    }
    for (n = w->child; n; n = n->next)
	dstring(n);
}
// layout names
void dnames(widget *w) {
    widget *n;
    char *s,*d;
    s = w->ctext;
    d = datafile + oindex;
    if (s && strncmp(s, "noname", 6)) {
	strcpy(d,s);
	oindex += strlen(s) + 1;
	dputi(w->off);
    }
    for (n = w->child; n; n = n->next)
	dnames(n);
}

void declare(widget *w) {
    widget *n;
    printf("widget %s;\n", w->ctext);
    for (n = w->child; n; n = n->next)
	declare(n);
}

void list(widget *w) {
    widget *n;
     dprint_widget(w);
     for (n = w->child; n; n = n->next)
	list(n);
}


int main( int argc, char *argv[]) {
    int x, found;
    close(0);
    x= open("test.rc", O_RDONLY);  // fixme: debuging
    while (1) {
	word();
	found = 0;
       	if (*wordb == 0) break;
	if (*wordb == '#') { do_comment(); continue;}
	for (x = 0; cmds[x].txt; x++) {
	    if (!strcmp(wordb, cmds[x].txt)) {
		cmds[x].call();
		found = 1;
		break;
	    }
	}
	if (!found) 
	fprintf(stderr, "unknown op >%s<\n", wordb);
    }
    head->vsize(head);
    head->hsize(head);
    head->vset(head, head->h);
    head->hset(head, head->w);
    head->vpos(head, head->x, head->y);
    //declare(head);
    dputi(0); // save room for dict pointer
    list(head);
    dstring(head);
    datafile[0] = oindex >> 8;
    datafile[1] = oindex & 255;
    dnames(head);
    dputi(0);
    dadjust(head);
    write(1, datafile, oindex);
    fprintf(stderr,"len %d\n", oindex);
}
