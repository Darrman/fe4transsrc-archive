srcdir=..
include $(srcdir)/Makefile.cfg

OBJ=append.o dup2x.o lz77.o rle.o io.o fe4enc.o

FE4ENC=fe4enc$(E)
PROGS=$(FE4ENC)
SIFSTUB=$(srcdir)/misc/sifstub$(E)
MENUFONT=newfont.bin
COMPRESSEDFONT=zfont.bin
SIFFILE=menufont.sif

all: $(PROGS)

menufont: $(COMPRESSEDFONT) $(SIFFILE)
$(COMPRESSEDFONT): $(FE4ENC) $(MENUFONT)
	./$(FE4ENC) $(MENUFONT) $(COMPRESSEDFONT)
$(SIFFILE): $(SIFSTUB) $(COMPRESSEDFONT)
	$(SIFSTUB) $(COMPRESSEDFONT) c2b $@
$(SIFSTUB):
	$(MAKE) -C $(srcdir)/misc
$(FE4ENC): $(OBJ)
	$(CC) $(OBJ) $(CFLAGS) -o $@
cleansif:
	$(RM) *.sif
clean: cleansif
	$(RM) *.o
veryclean: clean
	$(RM) $(FE4ENC)
spotless: veryclean
	$(RM) $(COMPRESSEDFONT)
