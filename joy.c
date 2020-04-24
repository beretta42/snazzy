/*  Joystick Mouse/Touch Driver
    fixme: this code has a fixed idea of screen resolution, 256x192

    fixme: this needs to do a moving average to smooth jitters
 */

#include "snazzy.h"
#include "ll.h"

#define keystrobe (*(volatile unsigned char *)0xff02)
#define keyread   (*(volatile unsigned char *)0xff00)

uint16_t poll_joy(uint8_t axis);

/* we'll store our x and y coords in 9 bits
   so we don't have to know our screen res here */
volatile int mouse_x;
volatile int mouse_y;
volatile int mouse_b;
static int mouse_o;
volatile int mouse_f;
volatile int mouse_mf;

static int hidden = 0;

void do_joy(void) {
    int x1 = 0;
    int y1 = 0;
    uint8_t nb;
    x1 = poll_joy(0)/2;
    y1 = poll_joy(1)*3/8;
    if ((x1 != mouse_x) || (y1 != mouse_y)) {
	mouse_mf = 1;
    }
    if (hidden == 0) {
	if ((x1 != mouse_x) || (y1 != mouse_y)){
	    graf_unput_mouse();
	    graf_put_mouse(x1,y1);
	}
    }
    mouse_x = x1;
    mouse_y = y1;
    /* check for change in mouse buttons */
    if (mouse_b != mouse_o) {
	mouse_f = 1;
    }
    mouse_o = mouse_b;
}


void mouse_show(void) {
    di();
    if (hidden == 1){
	graf_put_mouse(mouse_x, mouse_y);
	hidden = 0;
    }
    ei();
}

void mouse_hide(void) {
    di();
    if (hidden == 0){
	graf_unput_mouse();
	hidden = 1;
    }
    ei();
}
