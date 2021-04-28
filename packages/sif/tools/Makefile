# Sif v5.0 - Script Insertion Format
# (c) 1997 - 2001 Jay

# Options
CC=gcc
CFLAGS=-I. -I.. -Wall -W -O2 -ansi -pedantic -g
LDFLAGS=
MAKE=make
RM=rm -f

SRC=scanz.c ips2sif.c libsif.c
OBJ=scanz.o ips2sif.o libsif.o

all: scanz ips2sif libsif.a

scanz: scanz.o
	$(CC) -o $@ scanz.o $(LDFLAGS)
ips2sif: ips2sif.o ../ips.o
	$(CC) -o $@ ips2sif.o ../ips.o $(LDFLAGS)
libsif.a: libsif.o
	$(AR) rs $@ $<
.c.o:
	$(CC) -o $@ $(CFLAGS) -c $<

dep: Makefile.dep

Makefile.dep: $(SRC)
	$(CC) -MM -I.. $(SRC) > $@

clean:
	$(RM) Makefile.dep *.o
veryclean: clean
	$(RM) scanz ips2sif libsif.a

include Makefile.dep


