all: snazzy.a test

CFLAGS = -DUNIX 
SRC = snazzy.c label.c button.c ll-sdl.c font.c poplist.c hslide.c panel.c
SRC += menu.c window.c text.c
OBJ = $(SRC:.c=.o)


$(OBJ): $(SRC)

snazzy.a: $(OBJ) snazzy.h ll.h test.frm
	$(AR) -rcs snazzy.a $(OBJ)
	rm -f $(OBJ)

test: snazzy.a
	$(CC) -lSDL2 -o test test.c snazzy.a

test.bin:
	$(MAKE) -f Makefile.6809

test.frm: guipe/guipe guipe/test.rc
	cd guipe; ./guipe > ../test.frm

guipe/guipe:  guipe/guipe.c
	$(MAKE) -C guipe all

clean:
	rm -f *.o snazzy *~ test.h test.frm test snazzy.a test.map test.bin
	$(MAKE) -C guipe clean
