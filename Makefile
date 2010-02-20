# The default target
all::

CC = gcc
RM = rm -f

CFLAGS = -std=c99 -g -O2 -Wall -D_POSIX_SOURCE -D_GNU_SOURCE
LDFLAGS = -lncurses -lm -lpthread

LIB_H += ctetris.h
LIB_H += ct_blocks.h

LIB_OBJS += ctetris.o
LIB_OBJS += ct_usage.o
LIB_OBJS += ct_wrapper.o
LIB_OBJS += ct_blocks.o

LIBS =

$(LIB_OBJS): $(LIB_H)

ctetris: $(LIB_OBJS)
	$(CC) $(CFLAGS) -o $@ $(LDFLAGS) $^ $(LIBS)

.PHONY: all install clean indent tags

all:: ctetris

install: all
	install -m 755 ctetris /usr/local/bin/

clean:
	$(RM) ctetris
	# indent backup files
	$(RM) *~
	# library objects
	$(RM) $(LIB_OBJS)

indent:
	# try to find out all typenames defined by 'typedef' of c
	test -e ~/.indent.pro && cp ~/.indent.pro .indent.pro
	sed -n 's/.*typedef\s.*\s\([a-zA-Z_]\+\);/\1/p' *.[ch] | xargs \
		-Itype echo -T type >> .indent.pro
	indent *.[ch]
	rm -f .indent.pro

tags:
	ctags -R --c-kinds=+p --fields=+S .
