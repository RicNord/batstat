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
	mkdir -p /etc/systemd/user
	cp systemd/batstat.service /etc/systemd/user
	chmod 644 /etc/systemd/user/batstat.service

uninstall:
	rm -f "$(DESTDIR)$(PREFIX)/bin/batstat"
	rm -f "/etc/systemd/user/batstat.service"

clean:
	rm -f *.o batstat
