/* 

   A Label Widget

*/

#include <stdint.h>
#include "ll.h"
#include "snazzy.h"



void do_label(widget *w, int ev) {
    if (ev == EV_DRAW)
	ll_puts(w->x1, w->y1, gpt(w->text));
}


