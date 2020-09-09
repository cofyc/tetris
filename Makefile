# The default target
all::

CC = cc
RM = rm -f

CFLAGS = -std=c99 -g -O2 -Wall
LDFLAGS = -lncurses -lm

LIB_H += tetris.h
LIB_H += ct_block.h
LIB_H += ct_debug.h
LIB_H += ct_display.h
LIB_H += ct_game.h
LIB_H += usage.h
LIB_H += wrapper.h

LIB_OBJS += tetris.o
LIB_OBJS += ct_block.o
LIB_OBJS += ct_debug.o
LIB_OBJS += ct_display.o
LIB_OBJS += ct_game.o
LIB_OBJS += usage.o
LIB_OBJS += wrapper.o
LIB_OBJS += argparse/argparse.o

$(LIB_OBJS): $(LIB_H)

argparse/argparse.o: argparse/Makefile
	cd argparse && make

tetris: $(LIB_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

.PHONY: all install clean indent tags

all:: tetris

install: all
	install -m 755 tetris /usr/local/bin/

clean:
	$(RM) tetris
	# object files
	$(RM) *.o
	# indent backup files
	$(RM) *~

indent:
	# try to find out all typenames defined by 'typedef'
	test -e ~/.indent.pro && cp ~/.indent.pro .indent.pro
	sed -n 's/.*typedef\s.*\s\([a-zA-Z_]\+\);/\1/p' *.[ch] | xargs \
		-Itype echo '-T type' >> .indent.pro
	# additional typenames
	echo '-T WINDOW' >> .indent.pro
	indent *.[ch]
	rm -f .indent.pro
	rm -f *~

tags:
	ctags -R --c-kinds=+p --fields=+S .
