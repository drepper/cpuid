.PHONY: all clean

CC = gcc -std=gnu99
CFLAGS = $(OPT) $(DEBUG) $(WARN)
OPT = -O2
DEBUG = -g
WARN = -Wall

all: cpuid

cpuid: cpuid.c

clean:
	-rm -f cpuid
