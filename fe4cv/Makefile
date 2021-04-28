srcdir=..
include $(srcdir)/Makefile.cfg

ifdef WITH_SDL
FE4CV=fe4cv$(E)
endif
FE4RIPPL=fe4rippl$(E)
PROGS=$(FE4CV) $(FE4RIPPL)

all: $(PROGS)

$(FE4CV): fe4cv.o
	$(CC) -o $@ fe4cv.o $(LDFLAGS) $(LIBSDL)
$(FE4RIPPL): fe4rippl.o
	$(CC) -o $@ fe4rippl.o $(LDFLAGS)
cleansif:
clean: cleansif
	$(RM) *.o
veryclean: clean
	$(RM) $(PROGS)
spotless: veryclean
