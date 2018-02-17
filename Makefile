NAME      = id3rled

PKGS      = taglib_c
INCS     := $(shell pkg-config --cflags $(PKGS)) -I./
CFLAGS   := -std=gnu11 -ggdb -W -Wall -Wextra $(INCS) $(CFLAGS)

LIBS     := $(shell pkg-config --libs $(PKGS)) -lreadline
LDFLAGS  := $(LIBS) $(LDFLAGS) -Wl,--export-dynamic

SRCS  = $(wildcard *.c)
HEADS = $(wildcard *.h)
OBJS  = $(foreach obj,$(SRCS:.c=.o),$(obj))

PREFIX     ?= /usr
INSTALLDIR := $(DESTDIR)$(PREFIX)

$(NAME): $(OBJS)
	@echo $(CC) -o $@ $(OBJS)
	@$(CC) -o $@ $(OBJS) $(LDFLAGS)

$(OBJS): $(HEADS)

.c.o:
	@echo $(CC) -c $< -o $@
	@$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

clean:
	rm -f $(NAME) $(OBJS)

all: $(NAME)

install:
	install -d $(INSTALLDIR)/bin
	install -m 755 $(NAME) $(INSTALLDIR)/bin/$(NAME)

uninstall:
	rm -f $(INSTALLDIR)/bin/$(NAME)
