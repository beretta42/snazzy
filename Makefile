all: snazzy

snazzy: snazzy.c test.h snazzy.h
	$(CC) -lSDL2 -o snazzy snazzy.c font.c

test.h: guipe/guipe
	cd guipe; ./guipe > ../test.h

guipe/guipe:  guipe/guipe.c
	$(make) -C guipe all

clean:
	rm -f *.o snazzy *~ test.h
