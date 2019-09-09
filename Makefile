all: snazzy.a test

SRC = snazzy.c label.c button.c ll-sdl.c font.c poplist.c hslide.c
OBJ = $(SRC:.c=.o)

snazzy.a: $(SRC) snazzy.h ll.h test.frm
	$(CC) -c snazzy.c
	$(CC) -c label.c
	$(CC) -c button.c
	$(CC) -c ll-sdl.c
	$(CC) -c font.c
	$(CC) -c poplist.c
	$(CC) -c hslide.c
	$(AR) -rcs snazzy.a $(OBJ)
	rm -f $(OBJ)

test: snazzy.a
	$(CC) -lSDL2 -o test test.c snazzy.a



test.frm: guipe/guipe guipe/test.rc
	cd guipe; ./guipe > ../test.frm

guipe/guipe:  guipe/guipe.c
	$(make) -C guipe all

clean:
	rm -f *.o snazzy *~ test.h test.frm test snazzy.a
