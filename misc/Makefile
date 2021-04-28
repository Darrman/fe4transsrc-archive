srcdir=..
include $(srcdir)/Makefile.cfg

SIFSTUB=sifstub$(E)
SCRIPTSTAT=scriptstat$(E)
MKCP=mkcp$(E)
PROGS=$(SIFSTUB) $(SCRIPTSTAT) $(MKCP)

all: $(PROGS)

$(SIFSTUB): sifstub.o
	$(CC) -o $@ sifstub.o $(LDFLAGS) -lsif
$(SCRIPTSTAT): scriptstat.o
	$(CC) -o $@ scriptstat.o $(LDFLAGS)
$(MKCP): mkcp.o
	$(CC) -o $@ mkcp.o $(LDFLAGS) -lsif
cleansif:
	$(RM) *.sif
clean: cleansif
	$(RM) *.o
veryclean: clean
	$(RM) $(PROGS)
spotless: veryclean
