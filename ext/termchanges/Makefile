ifeq ($(shell uname),Darwin)
    LIBTOOL ?= glibtool
else
    LIBTOOL ?= libtool
endif

ifneq ($(VERBOSE),1)
    LIBTOOL +=--quiet
endif

CFLAGS  +=-Wall -Iinclude -std=c99
LDFLAGS +=-lutil

ifeq ($(DEBUG),1)
  CFLAGS +=-ggdb -DDEBUG
endif

ifeq ($(PROFILE),1)
  CFLAGS +=-pg
  LDFLAGS+=-pg
endif

CFLAGS  +=$(shell pkg-config --cflags vterm)
LDFLAGS +=$(shell pkg-config --libs   vterm)

HFILES=$(wildcard *.h)
CFILES=$(wildcard *.c)
OBJECTS=$(CFILES:.c=.lo)

LIBRARY=libtermcastserver.la

PREFIX=/usr/local
LIBDIR=$(PREFIX)/lib

$(LIBRARY): $(OBJECTS)
	@echo LINK $@
	@$(LIBTOOL) --mode=link --tag=CC $(CC) -rpath $(LIBDIR) -o $@ $^ $(LDFLAGS)

%.lo: %.c $(HFILES)
	@echo CC $<
	@$(LIBTOOL) --mode=compile --tag=CC $(CC) $(CFLAGS) -o $@ -c $<

.PHONY: clean
clean:
	$(LIBTOOL) --mode=clean rm -f $(OBJECTS)

.PHONY: install
