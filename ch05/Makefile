# Makefile for Beginning Linux Programming 4th Edition
# Chapter 5

MENU=menu1 menu2 menu3 menu4
TERMINAL=badterm password
KBHIT=kbhit

LDFLAGS=-lncurses

CC=gcc

ALL= $(MENU) $(TERMINAL) $(KBHIT)

all: $(ALL) badterm sizeterm menu5

# gcc badterm.c -o badterm -lncurses

badterm: badterm.c
	gcc $(CFLAGS)  -o $@ $< $(LDFLAGS)

sizeterm: sizeterm.c
	gcc $(CFLAGS)  -o $@ $< $(LDFLAGS)

menu5: menu5.c
	gcc $(CFLAGS)  -o $@ $< $(LDFLAGS)

clean:
	@rm -f $(ALL) badterm sizeterm menu5 *~
