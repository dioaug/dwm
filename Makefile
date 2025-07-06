include config.mk

SRCDIR = src
BUILDDIR = build
BIN = $(BUILDDIR)/radium

SRC = $(wildcard $(SRCDIR)/*.c)
OBJ = $(patsubst $(SRCDIR)/%.c, $(BUILDDIR)/%.o, $(SRC))

all: $(BIN)

$(BUILDDIR)/%.o: $(SRCDIR)/%.c | $(BUILDDIR)
	$(CC) -c $(CFLAGS) $< -o $@

$(BIN): $(OBJ) | $(BUILDDIR)
	$(CC) -o $@ $(OBJ) $(LDFLAGS)

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

clean:
	rm -rf $(BUILDDIR) radium-$(VERSION).tar.gz

dist: clean
	mkdir -p radium-$(VERSION)
	cp -R LICENSE Makefile README.md config.mk radium.1\
		$(SRC) radium-$(VERSION)
	tar -cf radium-$(VERSION).tar radium-$(VERSION)
	gzip radium-$(VERSION).tar
	rm -rf radium-$(VERSION)

install: all
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f $(BIN) $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/radium
	mkdir -p $(DESTDIR)$(MANPREFIX)/man1
	sed "s/VERSION/$(VERSION)/g" < radium.1 > $(DESTDIR)$(MANPREFIX)/man1/radium.1
	chmod 644 $(DESTDIR)$(MANPREFIX)/man1/radium.1

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/radium\
		$(DESTDIR)$(MANPREFIX)/man1/radium.1

.PHONY: all clean dist install uninstall
