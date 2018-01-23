.POSIX:

include config.mk

all: status

status: status.o helpers.o
status.o: status.c config.h
helpers.o: helpers.c config.h

config.h: config.def.h
	cp $< $@

clean:
	$(RM) status *.o

install: all
	mkdir -p "$(DESTDIR)$(PREFIX)/bin"
	cp -f status "$(DESTDIR)$(PREFIX)/bin/xroot-status"
	chmod 755 "$(DESTDIR)$(PREFIX)/bin/xroot-status"

uninstall:
	rm -f "$(DESTDIR)$(PREFIX)/bin/xroot-status"
