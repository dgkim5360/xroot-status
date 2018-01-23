CC=gcc
CFLAGS=-std=gnu11 -Wall -pedantic-errors -O2
LDLIBS=-lX11 -lasound

PREFIX=/usr/local

all: dwmstatus

dwmstatus: dwmstatus.o
dwmstatus.o: dwmstatus.c

clean:
	$(RM) dwmstatus *.o

install: all
	mkdir -p "$(DESTDIR)$(PREFIX)/bin"
	cp -f dwmstatus "$(DESTDIR)$(PREFIX)/bin"
	chmod 755 "$(DESTDIR)$(PREFIX)/bin/dwmstatus"

uninstall:
	rm -f "$(DESTDIR)$(PREFIX)/bin/dwmstatus"
