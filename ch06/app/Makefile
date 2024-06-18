
ALL= curses_app

all: curses_app

#Uncomment and edit the line below if necessary
#CFLAGS=-I/usr/include/ncurses

CC=gcc

LDFLAGS=-lcurses

%: %.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	-@rm -f $(ALL) *~
