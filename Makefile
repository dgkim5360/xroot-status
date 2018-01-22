CC=gcc
CFLAGS=-std=gnu11 -Wall -pedantic-errors -O2
LDLIBS=-lX11 -lasound

all: dwmstatus

dwmstatus: dwmstatus.o
dwmstatus.o: dwmstatus.c

clean:
	$(RM) dwmstatus *.o
