ifeq ($(shell uname),Darwin)
  LIBTOOL ?= glibtool
else
  LIBTOOL ?= libtool
endif

ifneq ($(VERBOSE),1)
  LIBTOOL +=--quiet
endif

CFLAGS  +=-Wall -I../libvterm/include -std=c99

ifeq ($(DEBUG),1)
  CFLAGS +=-ggdb -DDEBUG
endif

ifeq ($(PROFILE),1)
  CFLAGS +=-pg
  LDFLAGS+=-pg
endif

CFILES=$(wildcard *.c)
HFILES=$(wildcard *.h)
OBJECTS=$(CFILES:.c=.lo)
LIBRARY=libtermchanges.la

HFILES_INT=$(wildcard *.h) $(HFILES)

VERSION_CURRENT=0
VERSION_REVISION=0
VERSION_AGE=0

all: $(LIBRARY)
	
../libvterm/libvterm.la:
	cd ../libvterm && make

$(LIBRARY): $(OBJECTS) ../libvterm/libvterm.la
	@echo LINK $@
	@$(LIBTOOL) --mode=link --tag=CC $(CC) -fPIC -rpath /usr/local/lib -shared -version-info $(VERSION_CURRENT):$(VERSION_REVISION):$(VERSION_AGE) -o $@ $^ -L../libvterm/.libs -lvterm

%.lo: %.c $(HFILES_INT)
	@echo CC $<
	@$(LIBTOOL) --mode=compile --tag=CC $(CC) $(CFLAGS) -o $@ -c $<

.PHONY clean:
	rm *.lo *.la *.o
