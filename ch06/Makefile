ALL=ipmode moveadd pad screen1 color keypad multiw1 subscl

all: ipmode moveadd pad screen1 color keypad multiw1 subscl chinese

#Uncomment and edit the line below if necessary
#CFLAGS=-I/usr/include/ncurses

CC=gcc

LDFLAGS=-lncurses

chinese: chinese.c
	$(CC) $(CFLAGS) -o $@ $< -lncursesw

%: %.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	-@rm -f $(ALL) chinese *~
