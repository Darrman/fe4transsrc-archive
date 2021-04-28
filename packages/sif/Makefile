# Sif v5.0 - Script Insertion Format
# (c) 1997 - 2001 Jay

# Options
CC=gcc
CFLAGS=-D__DEBUG__ -Wall -W -O2 -ansi -pedantic -g
LDFLAGS=-ldl
MAKE=make
RM=rm -f

# Objects
SRC=main.c cmdline.c parse.c rat.c sif.c ips.c section.c hash.c map.c merge.c
OBJS=main.o cmdline.o parse.o rat.o sif.o ips.o section.o hash.o map.o merge.o
SIF=sif

.PHONY: all dep clean veryclean tools

all: $(SIF) tools
tools:
	$(MAKE) -C tools
$(SIF): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS)
.c.o:
	$(CC) -o $@ $(CFLAGS) -c $<

dep: Makefile.dep
	$(MAKE) dep -C tools
Makefile.dep: $(SRC)
	$(CC) -MM $(SRC) > $@

clean:
	$(RM) Makefile.dep *.o
	$(MAKE) clean -C tools
veryclean: clean
	$(RM) $(SIF)
	$(MAKE) veryclean -C tools

include Makefile.dep
