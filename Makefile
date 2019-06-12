# xst - simple terminal fork with xresources support and other patches
# See LICENSE file for copyright and license details.
.POSIX:

VERSION = 0.7

SRC = st.c x.c
OBJ = $(SRC:.c=.o)

all: options xst

options:
	@echo st build options:
	@echo "CFLAGS  = $(STCFLAGS)"
	@echo "LDFLAGS = $(STLDFLAGS)"
	@echo "CC      = $(CC)"

.c.o:
	$(CC) $(STCFLAGS) -c $<

st.o: config.h st.h win.h
x.o: arg.h config.h st.h win.h

$(OBJ): config.h config.mk

st: $(OBJ)
	$(CC) -o xst $(OBJ) $(STLDFLAGS)

clean:
	rm -f xst $(OBJ) st-$(VERSION).tar.gz

dist: clean
	mkdir -p st-$(VERSION)
	cp -R FAQ LEGACY TODO LICENSE Makefile README config.mk\
		config.def.h st.info st.1 arg.h st.h win.h $(SRC)\
		st-$(VERSION)
	tar -cf - st-$(VERSION) | gzip > st-$(VERSION).tar.gz
	rm -rf st-$(VERSION)

install: st
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f xst $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/xst
	mkdir -p $(DESTDIR)$(MANPREFIX)/man1
	sed "s/VERSION/$(VERSION)/g" < st.1 > $(DESTDIR)$(MANPREFIX)/man1/xst.1
	chmod 644 $(DESTDIR)$(MANPREFIX)/man1/xst.1
	tic -sx st.info
	@echo Please see the README file regarding the terminfo entry of xst.

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/xst
	rm -f $(DESTDIR)$(MANPREFIX)/man1/xst.1

.PHONY: all options clean dist install uninstall
