srcdir=..
include $(srcdir)/Makefile.cfg

all: dcenc.sif

dcenc.sif: dcenc.ips
	$(IPS2SIF) dcenc.ips $@
dcenc.ips: dcenc.asm
	$(X86AS) -c -hi dcenc.asm
	$(CP) dcenc.b $@
cleansif:
	$(RM) *.sif
clean: cleansif
	$(RM) dcenc.lst dcenc.b
veryclean: clean
	$(RM) dcenc.ips
spotless: veryclean
