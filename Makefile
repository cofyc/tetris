# The default target
all::

CC = gcc
RM = rm -f

CFLAGS = -std=c99 -O2 -Wall
LDFLAGS = -lncurses -lm

LIB_H += ctetris.h

LIB_OBJS += ctetris.o
LIB_OBJS += ct_usage.o
LIB_OBJS += ct_wrapper.o

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

