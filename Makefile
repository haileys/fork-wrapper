DESTDIR ?=
PREFIX ?= /usr/local

CFLAGS = -Wall -Wextra -pedantic -Werror -std=gnu99

.PHONY: all install

all: fork-wrapper

install: fork-wrapper
	cp $< $(DESTDIR)$(PREFIX)/bin/
