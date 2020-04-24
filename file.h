/********************
stdio.h API
*********************/

#include "stdint.h"

#define O_RDONLY    1
#define O_WRONLY    2

void finit(void);
int file_open(char *pathname, int flags);
int file_read(int fd, void *buf, size_t count);
int file_write(int fd, const void *ptr, size_t count);
int file_close(int fd);

