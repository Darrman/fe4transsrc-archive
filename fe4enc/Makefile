srcdir=..
include $(srcdir)/Makefile.cfg

OBJ=append.o dup2x.o lz77.o rle.o io.o fe4enc.o

FE4ENC=fe4enc$(E)
PROGS=$(FE4ENC)
SIFSTUB=$(srcdir)/misc/sifstub$(E)
MENUFONT=newfont.bin
STATFONT=statusfont.bin
COMPRESSEDFONT=zfont.bin
COMPRESSEDSTATFONT=zstatus.bin
SIFFILE=menufont.sif
STATSIFFILE=statfont.sif

all: $(PROGS)

menufont: $(COMPRESSEDFONT) $(SIFFILE) $(COMPRESSEDSTATFONT) $(STATSIFFILE)
$(COMPRESSEDFONT): $(FE4ENC) $(MENUFONT)
	./$(FE4ENC) $(MENUFONT) $(COMPRESSEDFONT)
$(COMPRESSEDSTATFONT): $(FE4ENC) $(STATFONT)
	./$(FE4ENC) $(STATFONT) $(COMPRESSEDSTATFONT)
$(SIFFILE): $(SIFSTUB) $(COMPRESSEDFONT)
	$(SIFSTUB) $(COMPRESSEDFONT) c2b $@
$(STATSIFFILE): $(SIFSTUB) $(COMPRESSEDSTATFONT)
	$(SIFSTUB) $(COMPRESSEDSTATFONT) 1b7b $@
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
