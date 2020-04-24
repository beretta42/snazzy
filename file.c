#include "stdint.h"
#include "file.h"
#include "string.h"

/********************
stdio.h API
*********************/
/*
   "mode" is a BASIC ascii mode: "I", "O".
   fd here is exactly a BASIC DEVNUM setting
*/
int open_ll(char mode);
int read_ll(void);
int write_ll(uint8_t c);
int close_ll(void);

#define DEVNUM *((uint8_t *)0x6f)
#define NAMBUF (uint8_t *)0x94c


#define MIN(a,b) (a<b ? a : b)


int file_open(char *pathname, int flags)
{
    char m;
    int ret;
    char *p;
    /* fixme: choose a fresh file no here? */
    DEVNUM = 1;
    memset(NAMBUF, ' ', 11);
    p = index(pathname, '.');
    if (p == 0)
	memcpy(NAMBUF, pathname, MIN(strlen(pathname),8));
    else {
	memcpy(NAMBUF, pathname, MIN(p-pathname, 8));
	memcpy(NAMBUF+8, p+1, MIN(strlen(p)-1, 8));
    }
    ret = open_ll(flags == O_RDONLY ? 'I' : 'O');
    if (ret) return -1;
    return DEVNUM;
}


int file_read(int fd, void *buf, size_t count)
{
    int r;
    uint8_t *p = (uint8_t *)buf;
    size_t num = 0;

    DEVNUM = fd;
    while(count--){
	r = read_ll();
	if (r < 0)
	    goto out;
	*p++ = r;
	num++;
    }
 out:
    return num;
}

int file_write(int fd, const void *ptr, size_t count)
{
    int r = 0;
    uint8_t *p = (uint8_t *)ptr;
    size_t num = 0;
    DEVNUM = fd;
    while(count--){
	r = write_ll(*p++);
	if (r < 0)
	    goto out;
	num++;
    }
 out:
    return num;
}

int file_close(int fd)
{
    int ret;
    DEVNUM = fd;
    ret = close_ll();
    return ret;
}
