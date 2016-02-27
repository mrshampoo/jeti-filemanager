TARGET = jeti

DESTDIR = /usr/

CONFIG = jeti.rc
CONFDIR = share/jeti
OBJDIR = obj
INCDIR = include
BINDIR = bin

INCLUDES = $(addprefix $(INCDIR)/,autocomplete.h command-window.h directorys.h environment.h handleflags.h navigation.h projecttypes.h soundeffects.h systemlog.h window.h)
OBJECTS := $(addprefix $(OBJDIR)/,main.o handleflags.o systemlog.o soundeffects.o navigation.o environment.o autocomplete.o window.o command-window.o directorys.o)

CC = gcc
CFLAGS = -g -Wall
LIBS = -lncurses

$(OBJDIR)/%.o: %.c $(INCLUDES)
	mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) $(LIBS) -c $< -I$(INCDIR) -o $@

$(BINDIR)/$(TARGET): $(OBJECTS)
	mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $(LIBS) $^ -o $@

install: $(BINDIR)/$(TARGET) $(CONFDIR)/$(CONFIG)
	cp $(BINDIR)/$(TARGET) $(DESTDIR)bin/
	mkdir -p $(DESTDIR)$(CONFDIR)
	cp $(CONFDIR)/$(CONFIG) $(DESTDIR)$(CONFDIR)/
	cp $(CONFDIR)/README $(DESTDIR)$(CONFDIR)/
	cp $(CONFDIR)/CHANGELOG $(DESTDIR)$(CONFDIR)/

uninstall: $(DESTDIR)bin/$(TARGET) $(DESTDIR)$(CONFDIR)
	rm $(DESTDIR)bin/$(TARGET)
	rm -r $(DESTDIR)$(CONFDIR)

.PHONY : clean
clean:
	rm -f $(BINDIR)/$(TARGET) $(OBJECTS)
	rm -r $(BINDIR) $(OBJDIR)
