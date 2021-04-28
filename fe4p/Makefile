srcdir=..
include $(srcdir)/Makefile.cfg

FE4S=fe4s$(E)
FE4P=fe4p$(E)
WID2C=wid2c$(E)
WIDTHS_C=widths.c
VWFTAB=$(srcdir)/fe4font/font.vwf
ENGTBL=$(srcdir)/fe4font/english.tbl

PROGS=$(FE4S) $(FE4P) $(WID2C)

all: $(PROGS)

$(FE4S): fe4s.o
	$(CC) -o $@ fe4s.o $(LDFLAGS)
$(FE4P): fe4p.o
	$(CC) -o $@ fe4p.o $(LDFLAGS) -ltbl -lsif
$(WID2C): wid2c.o
	$(CC) -o $@ wid2c.o $(LDFLAGS)
$(WIDTHS_C): $(VWFTAB) $(ENGTBL) $(WID2C) 
	./$(WID2C) $(VWFTAB) $(ENGTBL) > $@
$(VWFTAB):
	$(MAKE) font -C $(srcdir)/fe4font
fe4p.c: $(WIDTHS_C)
cleansif:
clean: cleansif
	$(RM) *.o
veryclean: clean
	$(RM) $(PROGS) $(WIDTHS_C)
spotless: veryclean
