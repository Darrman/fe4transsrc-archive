# This makefile builds external packages and installs them

ifeq ($(DESTDIR),)
DESTDIR=$(PWD)/..
endif

all: .patchok
	make -C 65816
	make -C sif
	make -C TableLib
.patchok: 65816-2.0-2.patch
	@echo 
	@echo "Do you wish to apply 65816-2.0-2.patch to the 65816 assembler?"
	@echo "This is a redundant question since you need this patch to build"
	@echo "the project properly. Say 'y' if you're confused."
	@echo -n "Apply 65816-2.0-2.patch? [y] "
	@read ans && \
	if test -z "$$ans" -o "$$ans" = "y"; then \
		cd 65816 && patch -p1 < ../65816-2.0-2.patch && touch ../$@; \
	fi
install:
	install 65816/65816 $(DESTDIR)/bin
	install sif/sif $(DESTDIR)/bin
	install sif/tools/ips2sif $(DESTDIR)/bin
	install sif/tools/scanz $(DESTDIR)/bin
	install sif/sif.h $(DESTDIR)/include
	install sif/tools/libsif.a $(DESTDIR)/lib
	install TableLib/tbl.h $(DESTDIR)/include
	install TableLib/libtbl.a $(DESTDIR)/lib
clean:
	make -C 65816 clean
	make -C sif veryclean
	make -C TableLib clean
	