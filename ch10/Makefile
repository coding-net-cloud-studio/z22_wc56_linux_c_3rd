# Makefile for Beginning Linux Programming 3rd Edition
# Chapter 10

# sudo apt-get install electric-fence

CC=gcc
# 注释:CC变量定义了使用的C编译器为gcc

DEBUG=debug1 debug2 debug3 debug4
CINFO=cinfo assert
MEMORY=efence checker

CFLAGS=-g
LDFLAGS=-g

ALL= $(DEBUG) $(CINFO) $(MEMORY)

all: $(ALL)

cinfo.o: cinfo.c
	$(CC) -c $(CFLAGS) -DDEBUG cinfo.c

assert: assert.o
	$(CC) -o assert $(LDFLAGS) assert.o -lm

efence: efence.o
	$(CC) -o efence $(LDFLAGS) efence.o -lefence

clean:
	@rm -f $(ALL) *~ *.o
