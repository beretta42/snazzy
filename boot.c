#include "file.h"


#define MAXBUF 256

unsigned char buf[MAXBUF];
unsigned char *ptr = 0;
unsigned char *end = 0;
int eof = 0;
int fd;

void call(unsigned int addr);


int fill(void) {
    int len = file_read(fd, buf, MAXBUF);
    if (len == 0) return -1;
    ptr = buf;
    end = buf + len;
    return 0;
}

int getc(void) {
    if (ptr == end)
	if (fill()) return -1;
    return *ptr++;
}

#define MMU0 ((volatile unsigned char *)0xffa0)
#define MMU1 ((volatile unsigned char *)0xffa8)


void mapram(void) {
    MMU0[4] = 0;
    MMU0[5] = 1;
    MMU0[6] = 2;
    MMU0[7] = 3;
}

void putc_coco3(int addr, unsigned char c) {
    mapram();
    *((unsigned char *)addr) = c;
    MMU0[4] = 0x3c;
    MMU0[5] = 0x3d;
    MMU0[6] = 0x3e;
    MMU0[7] = 0x3f;
}

void putc(int addr, unsigned char c) {
    *((char *)0xffdf) = 0;
    *((unsigned char *)addr) = c;
    *((char *)0xffde) = 0;
}

/* warning: ignored EOF */
unsigned int getw(void) {
    return getc() * 256 + getc();
}


void main(void) {
    unsigned int addr;
    unsigned int len;
    int flag;
    
    fd = file_open("KERNEL.BIN", O_RDONLY);

    while(1) {
	flag = getc();
	len = getw();
	addr = getw();
	if (flag == 0xff) break;
	while(len--)
	    putc_coco3(addr++, getc());
    }
    file_close(fd);
    mapram();
    call(addr);
}
