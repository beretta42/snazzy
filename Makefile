snazzy: snazzy.c
	$(CC) -lSDL2 -o snazzy snazzy.c radio.c menu.c area.c layout.c \
	button.c label.c check.c menuitem.c

test.c: snazzy
	./snazzy > test.c

rt: rt.c test.c
	$(CC) -lSDL2 -o rt rt.c label.c button.c check.c radio.c area.c \
	menu.c snazzyrt.c menuitem.c

clean:
	rm -f *.o snazzy *~ rt test.c
