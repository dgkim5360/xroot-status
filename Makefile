.POSIX:

include config.mk

all: xroot-status

xroot-status: xroot-status.o helpers.o
xroot-status.o: xroot-status.c config.h
helpers.o: helpers.c config.h

config.h: config.def.h
	cp $< $@

clean:
	$(RM) xroot-status *.o

install: all
	mkdir -p "$(DESTDIR)$(PREFIX)/bin"
	cp -f xroot-status "$(DESTDIR)$(PREFIX)/bin/"
	chmod 755 "$(DESTDIR)$(PREFIX)/bin/xroot-status"

uninstall:
	rm -f "$(DESTDIR)$(PREFIX)/bin/xroot-status"
