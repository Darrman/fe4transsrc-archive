# FE4 Makefile
# Type 'make' to build entire project

ifneq ($(wildcard Makefile.cfg),Makefile.cfg)
Makefile.cfg:
	@echo "Please run ./configure to configure project."
	@exit 1
endif

include Makefile.cfg

.PHONY: dump ppt menufont font script clean veryclean spotless checkin update
all:
ifneq ($(wildcard fe4.smc),fe4.smc)
	@echo "Error, please place fe4.smc in this directory."
	@exit 1
endif
	$(MAKE) -C fe4dec
	$(MAKE) -C fe4enc
	$(MAKE) -C fe4d
	$(MAKE) -C fe4font
	$(MAKE) -C fe4p
	$(MAKE) -C misc
ifndef NOCOMPRESS
	$(MAKE) -C dcenc
endif
	$(MAKE) -C script
dump:
	$(MAKE) dump -C fe4d
ppt:
	$(MAKE) ppt -C fe4d
menufont:
	$(MAKE) menufont -C fe4enc
font:
	$(MAKE) font -C fe4font
script:
	$(MAKE) script -C script
status:
	$(MAKE) scriptstat -C misc
	misc/scriptstat $(wildcard script/*.txt)
project: dump menufont font script
cleansif:
	$(MAKE) cleansif -C fe4dec
	$(MAKE) cleansif -C fe4enc
	$(MAKE) cleansif -C fe4d
	$(MAKE) cleansif -C fe4font
	$(MAKE) cleansif -C fe4p
	$(MAKE) cleansif -C misc
	$(MAKE) cleansif -C dcenc
	$(MAKE) cleansif -C script
clean:
	$(MAKE) clean -C fe4dec
	$(MAKE) clean -C fe4enc
	$(MAKE) clean -C fe4d
	$(MAKE) clean -C fe4font
	$(MAKE) clean -C fe4p
	$(MAKE) clean -C misc
	$(MAKE) clean -C dcenc
	$(MAKE) clean -C script
	$(RM) *.sif
veryclean:
	$(MAKE) veryclean -C fe4dec
	$(MAKE) veryclean -C fe4enc
	$(MAKE) veryclean -C fe4d
	$(MAKE) veryclean -C fe4font
	$(MAKE) veryclean -C fe4p
	$(MAKE) veryclean -C misc
	$(MAKE) veryclean -C dcenc
	$(MAKE) veryclean -C script
	$(RM) *.sif
spotless:
	$(MAKE) spotless -C fe4dec
	$(MAKE) spotless -C fe4enc
	$(MAKE) spotless -C fe4d
	$(MAKE) spotless -C fe4font
	$(MAKE) spotless -C fe4p
	$(MAKE) spotless -C misc
	$(MAKE) spotless -C dcenc
	$(MAKE) spotless -C script
	$(RM) *.sif fe4.ips remaining.rat
distclean: spotless
	$(RM) Makefile.cfg config.h config.log config.cache config.status .LICENSE.read
update:
	@./patch-update.sh

RATS=$(wildcard rat/*.rat)
ifndef NOCOMPRESS
COMPRESS_SIF=dcenc/dcenc.sif
else
COMPRESS_SIF=
endif
MENUFONT_SIF=fe4enc/menufont.sif
STATFONT_SIF=fe4enc/statfont.sif
FONT_SIFS=$(addprefix fe4font/,font.sif vwftable.sif)
SCRIPT_SIFS=$(addsuffix .sif,$(basename $(wildcard script/*.txt)))
ifndef NOCOMPRESS
SCRIPT_SIFS+=script/fe4tbl.sif
endif
MISC_SIFS=$(wildcard misc/*.sif)
SIFS=fe4-j2e.sif $(COMPRESS_SIF) $(MENUFONT_SIF) $(STATFONT_SIF) $(FONT_SIFS) $(SCRIPT_SIFS) $(MISC_SIF)
MAP=script/fe4map.so

VPATH=dcenc:fe4enc:fe4font:script

ips: fe4.ips
$(COMPRESS_SIF): dcenc.asm
	$(MAKE) -C dcenc
$(MENUFONT_SIF): newfont.bin
	$(MAKE) menufont -C fe4enc
$(STATFONT_SIF): statusfont.bin
	$(MAKE) menufont -C fe4enc
$(FONT_SIFS): font.bin english.tbl genwid.c
	$(MAKE) font -C fe4font
script/%.sif: script/%.txt
	$(MAKE) script -C script
script/fe4tbl.sif: $(wildcard script/fe4_*.txt)
	$(MAKE) script -C script
fe4-j2e.sif:
	$(MAKE) -C misc
	misc/mkcp$(E)
$(MAP): fe4map.c
	$(MAKE) -C script
fe4.ips: $(SIFS) $(RATS) $(MAP)
	env LD_LIBRARY_PATH=script \
	PATH="script:$(PATH)" \
	$(SIF) --bestfit --merge --hirom $(RATS) $(SIFS) -r remaining.rat -o $@
