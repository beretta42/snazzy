all: snazzy.a test

SRC = snazzy.c label.c button.c ll-sdl.c font.c poplist.c hslide.c panel.c
SRC += menu.c window.c text.c
OBJ = $(SRC:.c=.o)

snazzy.a: $(SRC) snazzy.h ll.h test.frm
	$(CC) -c snazzy.c
	$(CC) -c label.c
	$(CC) -c button.c
	$(CC) -c ll-sdl.c
	$(CC) -c font.c
	$(CC) -c poplist.c
	$(CC) -c hslide.c
	$(CC) -c panel.c
	$(CC) -c menu.c
	$(CC) -c window.c
	$(CC) -c text.c
	$(AR) -rcs snazzy.a $(OBJ)
	rm -f $(OBJ)

test: snazzy.a
	$(CC) -lSDL2 -o test test.c snazzy.a



test.frm: guipe/guipe guipe/test.rc
	cd guipe; ./guipe > ../test.frm

guipe/guipe:  guipe/guipe.c
	$(MAKE) -C guipe all

clean:
	rm -f *.o snazzy *~ test.h test.frm test snazzy.a test.map test.bin
	$(MAKE) -C guipe clean
