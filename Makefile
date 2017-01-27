TARGET = jeti

DESTDIR ?= /
PREFIX ?= /usr

CONFIG = jeti.rc
CONFDIR = share/jeti
OBJDIR = obj
INCDIR = include
BINDIR = bin

INCLUDES = $(addprefix $(INCDIR)/,autocomplete.h command-window.h directorys.h environment.h handleflags.h navigation.h projecttypes.h soundeffects.h systemlog.h window.h dialog-window.h)
OBJECTS := $(addprefix $(OBJDIR)/,main.o handleflags.o systemlog.o soundeffects.o navigation.o environment.o autocomplete.o window.o command-window.o directorys.o dialog-window.o)

CC ?= gcc
CFLAGS ?= -g -Wall
LIBS ?= -lncurses

all:    $(OBJDIR) $(BINDIR)/$(TARGET)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(OBJDIR)/%.o: %.c $(INCLUDES)
	$(CC) $(CFLAGS) $(LIBS) -c $< -I$(INCDIR) -o $@

$(BINDIR)/$(TARGET): $(OBJECTS)
	mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $(LIBS) $^ -o $@

.PHONY : all
install: all
	install -D $(BINDIR)/$(TARGET) -t $(DESTDIR)$(PREFIX)/bin/

.PHONY : uninstall
uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(TARGET)

.PHONY : clean
clean:
	rm -f $(BINDIR)/$(TARGET) $(OBJECTS)
	rm -r $(BINDIR) $(OBJDIR)
