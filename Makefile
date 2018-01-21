CFLAGS=-std=gnu11 -Wall -pedantic-errors -O2
LDLIBS=-lX11

all: dwmstatus

dwmstatus: dwmstatus.o
dwmstatus.o: dwmstatus.c

clean:
	$(RM) dwmstatus
