srcdir=..
include $(srcdir)/Makefile.cfg

FE4DEC=fe4dec$(E)

all: $(FE4DEC)

$(FE4DEC): fe4dec.o 
	$(CC) -o $@ fe4dec.o $(LDFLAGS)
cleansif:
clean: cleansif
	$(RM) *.o
veryclean: clean
	$(RM) $(FE4DEC)
spotless: veryclean
