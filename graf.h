/* graphics API */


void graf_setclip(int x, int y, int w, int h);
void graf_clear(void);
void graf_setpixel(int x, int y);
void graf_cset(int color);
void graf_char_draw(int x, int y, char *glyph);
void graf_put_mouse(int x, int y);
void graf_unput_mouse();
void graf_bar(int x, int y, int w, int h);
void graf_blit(int x, int y, char *bitmap);
char *graf_setbuf(char *p);
void graf_draw_mode(unsigned char mode);
void graf_stiple(int x, int y, int w, int h, char *data);

struct graf_bitmap_s {
    uint16_t width;    // in pixels MSB
    uint16_t height;   // in pixels MSB
    unsigned char pixels[];
};
/* pixels are laid out row first then column on byte boundaries.  each
   byte worth of screen data is encoded into two bytes: the first
   being the AND mask, and the second is the OR mask.
 */

#define GRAF_MODE_SET  0
#define GRAF_MODE_XOR  1


