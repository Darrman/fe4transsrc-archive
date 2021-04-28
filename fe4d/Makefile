srcdir=..
include $(srcdir)/Makefile.cfg

FILES=fe4_00 fe4_01 fe4_02 fe4_03 fe4_04 fe4_05 fe4_06 fe4_07 fe4_08 \
	fe4_09 fe4_0A fe4_0B fe4_0C fe4_0D fe4_0E fe4_0F femain shop \
	fe4_missing00 fe4_missing01 fe4_missing02 fe4_missing03 fe4_missing04 \
	fe4_missing05 fe4_missing06 fe4_missing07 fe4_missing08 \
	fe4_missing09 fe4_missing0A fe4_missing0B fe4_missing0C fe4_missing0D \
	fe4_missing0E fe4_missing0F chap
SJFILES=enm1 knight chrnam army weap city
FFILES=skill objective config
FFILES2=battleopts battleopts2
FFILES3=menu menu2
FFILES4=menu3
DUMPS=$(addsuffix .txt,$(addprefix script/,$(FILES)))
SJDUMPS=$(addsuffix .txt,$(addprefix script/,$(SJFILES)))
FDUMPS=$(addsuffix .txt,$(addprefix script/,$(FFILES)))
FDUMPS2=$(addsuffix .txt,$(addprefix script/,$(FFILES2)))
FDUMPS3=$(addsuffix .txt,$(addprefix script/,$(FFILES3)))
FDUMPS4=$(addsuffix .txt,$(addprefix script/,$(FFILES4)))
PPT=$(addsuffix .ppt,$(addprefix ppt/,$(FILES)))

FE4PG=fe4pg$(E)
FE4D=fe4d$(E)
FE4DR=fe4dr$(E)
FE4SJD=fe4sjd$(E)
FE4GA=fe4ga$(E)
FE4FD=fe4fd$(E)
SPECSCRIPT=mkspec.sh

PROGS=$(FE4PG) $(FE4D) $(FE4DR) $(FE4SJD) $(FE4GA) $(FE4FD)

all: $(PROGS)
ppt: $(PPT)
$(SPECSCRIPT): $(FE4GA) $(FE4ROM)
	./$(FE4GA) $(FE4ROM) > $@
	chmod 755 $@
$(FE4GA): fe4ga.o
	$(CC) -o $@ fe4ga.o $(LDFLAGS)
$(FE4PG): fe4pg.o
	$(CC) -o $@ fe4pg.o $(LDFLAGS)
$(FE4D): fe4d.o
	$(CC) -o $@ fe4d.o $(LDFLAGS)
$(FE4DR): fe4dr.o
	$(CC) -o $@ fe4dr.o $(LDFLAGS)
$(FE4SJD): fe4sjd.o
	$(CC) -o $@ fe4sjd.o $(LDFLAGS)
$(FE4FD): fe4fd.o
	$(CC) -o $@ fe4fd.o $(LDFLAGS)
dump: $(DUMPS) $(SJDUMPS) $(FDUMPS) $(FDUMPS2) $(FDUMPS3) $(FDUMPS4)
script/enm1.txt: $(FE4SJD) $(FE4ROM)
	./$(FE4SJD) $(FE4ROM) 3857b 4 3 enm1 > $@
script/knight.txt: $(FE4SJD) $(FE4ROM)
	./$(FE4SJD) $(FE4ROM) 38cb3 5 4 knight > $@ 
script/chrnam.txt: $(FE4SJD) $(FE4ROM)
	./$(FE4SJD) $(FE4ROM) 39c59 6 5 chrnam > $@ 
script/army.txt: $(FE4SJD) $(FE4ROM)
	./$(FE4SJD) $(FE4ROM) 3dae8 7 6 army > $@ 
script/weap.txt: $(FE4SJD) $(FE4ROM)
	./$(FE4SJD) $(FE4ROM) 3de12 8 7 weap > $@ 
script/city.txt: $(FE4SJD) $(FE4ROM)
	./$(FE4SJD) $(FE4ROM) 3f717 9 8 city > $@
script/chap.txt: ppt/chap.ppt $(FE4D) $(FE4ROM) fe4.euc
	./$(FE4D) $(FE4ROM) fe4.euc $< 16 > $@
script/%.txt: ppt/%.ppt $(FE4D) $(FE4ROM) fe4.euc
	./$(FE4D) $(FE4ROM) fe4.euc $< > $@
$(FDUMPS): script/%.txt: ppt/%.ppt $(FE4FD) $(FE4ROM)
	./$(FE4FD) -s -n BANK80 $(FE4ROM) $< > $@
$(FDUMPS2): script/%.txt: ppt/%.ppt $(FE4FD) $(FE4ROM)
	./$(FE4FD) -s -n BANK85 $(FE4ROM) $< > $@
$(FDUMPS3): script/%.txt: ppt/%.ppt $(FE4FD) $(FE4ROM)
	./$(FE4FD) -a 7 $(FE4ROM) $< > $@
$(FDUMPS4): script/%.txt: ppt/%.ppt $(FE4FD) $(FE4ROM)
	./$(FE4FD) -a 7 -s -n BANK85 $(FE4ROM) $< > $@
ppt/shop.ppt: $(FE4PG) $(FE4ROM)
	./$(FE4PG) -h -r "ad0b3-ad1bd" -e "@lmh" -o $@ $(FE4ROM)
ppt/skill.ppt: $(FE4PG) $(FE4ROM)
	./$(FE4PG) -h -r "d700-d860" -e "25a9@lm85" -o $@ $(FE4ROM)
ppt/objective.ppt: $(FE4PG) $(FE4ROM)
	./$(FE4PG) -h -r "dd74-dd98" -e "@lm80" -o $@ $(FE4ROM)
ppt/config.ppt: $(FE4PG) $(FE4ROM)
	./$(FE4PG) -h -r "e9a0-ea4e" -e "25a9@lm85" -o $@ $(FE4ROM)
ppt/battleopts.ppt: $(FE4PG) $(FE4ROM)
	./$(FE4PG) -h -r "5a820-5a870" -e "25a9@lm85" -o $@ $(FE4ROM)
ppt/battleopts2.ppt: $(FE4PG) $(FE4ROM)
	./$(FE4PG) -h -r "5a9a0-5a9ce" -e "25a9@lm85" -o $@ $(FE4ROM)
ppt/menu.ppt: $(FE4PG) $(FE4ROM)
	./$(FE4PG) -h -r "5df98-5dfb9" -e "@lmh.." -o $@ $(FE4ROM)
ppt/menu2.ppt: $(FE4PG) $(FE4ROM)
	./$(FE4PG) -h -r "5dfbf-5dfdb" -e "@lmh.." -o $@ $(FE4ROM)
ppt/menu3.ppt: $(FE4PG) $(FE4ROM)
	./$(FE4PG) -h -r "5e4af-5e4dd" -e "@lm" -o $@ $(FE4ROM)
ppt/chap.ppt: $(FE4PG) $(FE4ROM)
	./$(FE4PG) -h -r "d9d65-d9d7a" -e "@lm" -o $@ $(FE4ROM)
ppt/%.ppt: ppt/%.spec $(FE4PG) $(FE4ROM)
	./$(FE4PG) `cat $<` -o $@ $(FE4ROM)
ppt/%.spec: $(SPECSCRIPT)
	./$(SPECSCRIPT)
cleansif:
clean: cleansif
	$(RM) *.o
veryclean: clean
	$(RM) $(PROGS)
spotless: veryclean
	$(RM) script/*.txt ppt/*.ppt ppt/*.spec $(SPECSCRIPT)
fe4d.o: evil.c
fe4dr.o: evil.c
