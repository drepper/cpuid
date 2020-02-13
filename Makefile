.PHONY: all clean install dist

VERSION := $(shell sed -n 's/Version:[[:space:]]*\(.*\)/\1/p' cpuid.spec)
prefix = /usr
bindir = $(prefix)/bin

CC = gcc -std=gnu99
CFLAGS = $(OPT) $(DEBUG) $(WARN)
OPT = -O2
DEBUG = -g
WARN = -Wall

all: cpuid

cpuid: cpuid.c

install: all
	install -D cpuid $(DESTDIR)$(bindir)/cpuid

dist:
	rm -f cpuid-$(VERSION)
	ln -s . cpuid-$(VERSION)
	tar acvfh cpuid-$(VERSION).tar.xz cpuid-$(VERSION)/{Makefile,cpuid.c,cpuid.spec,COPYING,README.md}
	rm -f cpuid-$(VERSION)

clean:
	-rm -f cpuid
