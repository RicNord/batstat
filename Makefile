.PHONY: all clean

CC = gcc
PREFIX = /usr/local
CFLAGS = -Wall -Wextra -g
LDLIBS = $(shell pkg-config --cflags --libs libnotify)

all: batstat

batstat: batstat.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $< $(LDLIBS)

install: all
	mkdir -p "$(DESTDIR)$(PREFIX)/bin"
	cp -f batstat "$(DESTDIR)$(PREFIX)/bin"
	chmod 755 "$(DESTDIR)$(PREFIX)/bin/batstat"

uninstall:
	rm -f "$(DESTDIR)$(PREFIX)/bin/batstat"

clean:
	rm *.o batstat
