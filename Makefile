snazzy: snazzy.c
	$(CC) -lSDL2 -o snazzy snazzy.c radio.c menu.c area.c layout.c \
	button.c label.c check.c


clean:
	rm -f *.o snazzy *~
