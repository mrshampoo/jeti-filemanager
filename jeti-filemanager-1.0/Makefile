TARGET = jeti

prefix = /usr

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
	cp $(BINDIR)/$(TARGET) $(prefix)/bin/
	mkdir -p $(prefix)/$(CONFDIR)
	cp $(CONFDIR)/$(CONFIG) $(prefix)/$(CONFDIR)/

uninstall: $(prefix)/bin/$(TARGET) $(prefix)/$(CONFDIR)
	rm $(prefix)/bin/$(TARGET)
	rm -r $(prefix)/$(CONFDIR)

.PHONY : clean
clean:
	rm -f $(BINDIR)/$(TARGET) $(OBJECTS)
	rm -r $(BINDIR) $(OBJDIR)
