

int ll_init(void);
void ll_cset(int color);
void ll_deinit(void);
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
void ll_blit(int x, int y, unsigned char *bm);
void ll_roundrect(int x, int y, int w, int h, unsigned char *s, int r);

volatile extern int mouse_x;
volatile extern int mouse_y;
volatile extern int mouse_b;
volatile extern int mouse_f;
volatile extern int mouse_mf;
