srcdir=..
include $(srcdir)/Makefile.cfg

FILES=$(basename $(wildcard *.txt))
SIFS=$(addsuffix .sif,$(FILES))
SCRIPT=$(addsuffix .txt,$(FILES))
COMPRESSABLESCRIPT=$(wildcard fe4_*.txt)

ifndef NOCOMPRESS
TABLE=fe4.tbl
TBLSIF=fe4tbl.sif
INCLUDETABLE=-i $(TABLE)
endif

FE4S=$(srcdir)/fe4p/fe4s$(E)
FE4P=$(srcdir)/fe4p/fe4p$(E)

DLMAP=fe4map.so

VPATH=$(srcdir)/fe4p

all: $(DLMAP)

script: $(SIFS) $(TBLSIF)
$(DLMAP): fe4map.c fe4map.def
ifeq ($(HOST),cygwin)
	$(CC) -mdll fe4map.c fe4map.def -o $@
else
	$(CC) -shared -Wl,-soname,fe4map.so fe4map.c -o $@
endif
ifndef NOCOMPRESS
$(TABLE) $(TBLSIF): $(FE4S) $(COMPRESSABLESCRIPT)
	$(FE4S) -o $(TABLE) -s $(TBLSIF) -1 296F0 -2 298F0 -3 29AF0 -4 29CF0 $(COMPRESSABLESCRIPT)
endif
%.sif: %.txt $(FE4P) $(TABLE) $(TBLSIF)
	$(FE4P) -o $@ $(INCLUDETABLE) $<
$(FE4S): fe4s.c
	$(MAKE) -C $(srcdir)/fe4p
$(FE4P): fe4p.c
	$(MAKE) -C $(srcdir)/fe4p
cleansif:
	$(RM) *.sif
clean: cleansif
veryclean: clean
	$(RM) $(DLMAP)
spotless: veryclean
	$(RM) $(TABLE) $(TBLSIF)
