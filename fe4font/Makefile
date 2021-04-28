srcdir=..
include $(srcdir)/Makefile.cfg

GENWID=genwid$(E)
SIFSTUB=$(srcdir)/misc/sifstub$(E)
FONTFILE=font.bin
VWFTABLE=font.vwf
VWFSIF=vwftable.sif
FONTSIF=font.sif
PROGS=$(GENWID)

VPATH=$(srcdir)/misc

all: $(PROGS)
font: $(VWFTABLE) $(FONTSIF) $(VWFSIF)
$(VWFTABLE): $(FONTFILE) $(GENWID)
	./$(GENWID) $(FONTFILE) $(VWFTABLE)
$(FONTSIF): $(FONTFILE) $(SIFSTUB)
	$(SIFSTUB) $(FONTFILE) 128200 $@
$(VWFSIF): $(VWFTABLE) $(SIFSTUB)
	$(SIFSTUB) $(VWFTABLE) 119b71 $@
$(SIFSTUB): sifstub.c
	$(MAKE) -C $(srcdir)/misc
$(GENWID): genwid.o
	$(CC) -o $@ genwid.o $(LDFLAGS)
cleansif:
	$(RM) *.sif
clean: cleansif
	$(RM) *.o
veryclean: clean
	$(RM) $(PROGS)
spotless: veryclean
	$(RM) $(VWFTABLE)
