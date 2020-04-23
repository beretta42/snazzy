/* graphics API */

void _graf_setclip(int x, int y, int w, int h);
void _graf_clear(void);
void _graf_setpixel(int x, int y);
void _graf_cset(int color);
void _graf_char_draw(int x, int y, char *glyph);
void _graf_put_mouse(int x, int y);
void _graf_unput_mouse(int x, int y);
void _graf_bar(int x, int y, int w, int h);
