

int ll_init(void);
void ll_cset(int color);
void ll_deinit(void);
void ll_clear(void);
void ll_setpixel(int x, int y);
void ll_hline(int x, int y, int w);
void ll_vline(int x, int y, int h);
void ll_bar(int x, int y, int w, int h);
void ll_box(int x, int y, int w, int h);
void ll_char_draw(int x, int y, unsigned char *p);
void ll_puts(int x, int y, char *t);
void ll_putn(int x, int y, char *t, int max);
void ll_draw_back(int x, int y, int w, int h);
void ll_loop(void);
