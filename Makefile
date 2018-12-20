snazzy: snazzy.c
	$(CC) -lSDL2 -o snazzy snazzy.c radio.c menu.c area.c


clean:
	rm -f *.o snazzy *~
