#include "coco.h"
#include "snazzy.h"

#define keystrobe (*(volatile unsigned char *)0xff02)
#define keyread   (*(volatile unsigned char *)0xff00)
unsigned char ktab[8];
unsigned char ktab1[8];
volatile unsigned char key;
volatile unsigned char meta;
extern int mouse_b;

/* keycode to ascii table */
unsigned char atab[] = {
    '@', 'h', 'p', 'x', '0', '8', KEY_RET /*enter*/,
    'a', 'i', 'q', 'y', '1', '9', 0 /*clear*/,
    'b', 'j', 'r', 'z', '2', ':', KEY_ESC /*break*/,
    'c', 'k', 's', KEY_UP/*up*/, '3', ';', 0 /*alt*/,
    'd', 'l', 't', KEY_DOWN/*dn*/, '4', ',', 0 /*cntl*/,
    'e', 'm', 'u', KEY_BS, '5', '-', 0 /*f1*/,
    'f', 'n', 'v', KEY_DEL, '6', '.', 0 /*f2*/,
    'g', 'o', 'w', ' ', '7', '/', 0 /*shift*/,
};

/* keycode to shifted ascii table */
unsigned char satab[] = {
    '@', 'H', 'P', 'X', '0', '(', 10 /*enter*/,
    'A', 'I', 'Q', 'Y', '!', ')', 0 /*clear*/,
    'B', 'J', 'R', 'Z', '"', '*', 0 /*break*/,
    'C', 'K', 'S', 0/*up*/, '#', '+', 0 /*alt*/,
    'D', 'L', 'T', 0/*dn*/, '$', '<', 0 /*cntl*/,
    'E', 'M', 'U', KEY_LEFT, '%', '=', 0 /*f1*/,
    'F', 'N', 'V', KEY_RIGHT, '&', '>', 0 /*f2*/,
    'G', 'O', 'W', ' ', '\'', '?', 0 /*shift*/,
};

/* poll the coco keyboard - called from interrupt */
void kpoll(void)
{
    unsigned char m = 0;
    unsigned char b = 0xfe;
    int i,j;
    /* check for joystick(mouse) buttons */
    keystrobe = 0xff;
    mouse_b = (~keyread) & 0xf;
    if (mouse_b) return;
    /* copy existing ktab to prime */
    for(i = 0; i < 8; i++)
	ktab1[i] = ktab[i];
    /* read keys into table */
    for(i = 0; i < 8; i++){
	keystrobe = b;
	ktab[i] = (~keyread) & 0x7f;
	b = (b << 1) + 1;
    }
    /* gather and mask shift keys */
    m += (ktab[7] & 0x40) ? 1 : 0;
    ktab[7] &= ~0x40;
    m <<= 1;
    if ( (ktab[4] & 0x40) || (ktab[1] & 0x40) ){
	m+= 1;
    }
    ktab[4] &= ~0x40;
    ktab[1] &= ~0x40;
    m <<= 1;
    m += (ktab[3] & 0x40) ? 1 : 0;
    ktab[3] &= ~0x40;
    /* find new char code, if any */
    for (i = 0; i < 8; i++) {
	b = (ktab[i] ^ ktab1[i]) & ktab[i];
	for (j = 0; j < 7; j++) {
	    if (b & 1) {
		key = i * 7 + j + 1;
		meta = m;
		return;
	    }
	    b = b >> 1;
	}
    }
}


/**
 * keyboard_main - Handle the keyboard presses
 */
void getkey(void){
    if (meta & 4)
	key = satab[key-1];
    else
	key = atab[key-1];
};




