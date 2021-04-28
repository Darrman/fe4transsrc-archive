# Makefile for DJGPP
# Requires GNU make

OBJS=hash.o string.o tbl.o
CFLAGS=-Wall -Werror -W -ansi -pedantic -O2 -fomit-frame-pointer -funroll-loops -g

.PHONY: all clean

all: libtbl.a

libtbl.a: $(OBJS)
	$(AR) rs $@ $(OBJS)

%.o: %.c tbl.h tbintern.h
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f *.o
