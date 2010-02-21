# The default target
all::

CC = gcc
RM = rm -f

CFLAGS = -std=c99 -g -O2 -Wall 
LDFLAGS = -lncurses -lm -lpthread

LIB_H += ct.h
LIB_H += ct_blocks.h
LIB_H += ct_display.h

LIB_OBJS += ct.o
LIB_OBJS += ct_usage.o
LIB_OBJS += ct_wrapper.o
LIB_OBJS += ct_blocks.o
LIB_OBJS += ct_display.o

LIBS =

$(LIB_OBJS): $(LIB_H)

ct: $(LIB_OBJS)
	$(CC) $(CFLAGS) -o $@ $(LDFLAGS) $^ $(LIBS)

.PHONY: all install clean indent tags

all:: ct

install: all
	install -m 755 ct /usr/local/bin/

clean:
	$(RM) ct
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
