# 65816 cross assembler v2.0 (c)1994 Jeremy Gordon (jgordon@animator.slip.umd.edu)
#CC = gcc
#CFLAGS = -ansi $(DEFINES)

# Uncomment these for Amiga (SAS C)
#CC=sc
#DEFINES = DEF=AMIGA	# this is for the amiga compilation

# some machines (dec's) need strdup, so uncommment ....
SOURCE = 65816.c parser.c label.c list.c machine.c address.c opcode.c\
		evaluate.c smc.c  # strdup.c
OBJS = 65816.o parser.o label.o list.o machine.o address.o opcode.o\
		evaluate.o smc.o  # strdup.o

65816: $(OBJS)
	$(CC) $(CFLAGS) -o 65816 $(OBJS) $(DEFINES) $(LIBS)
# Use the following line instead for Amiga (SAS C)
#	SC LINK $(SOURCE) $(DEFINES) $(LIBS)
clean:
	rm -f *.o 65816 bindiff test.o test.smc test.b test.s test.e 65816.tar
tar:
	tar -cvf 65816.tar 65816.doc Makefile *.c *.h hdmademo.a KNOWN_BUGS\
	  readme.1st CHANGES
zip:
	zip 65816.zip 65816.doc Makefile *.c *.h hdmademo.a KNOWN_BUGS\
	  readme.1st CHANGES
bind:	65816
	emxbind emx.exe 65816 65816.exe
bindiff: bindiff.o machine.o
	gcc $(CFLAGS) -o bindiff bindiff.o machine.o
tools:	bindiff
backup: tar
	cp 65816.tar backup/.
	mv backup/65816.tar.z backup/65816.tar.z.old
	rm 65816.tar
	gzip backup/65816.tar
	sync
distrib: backup
	rm backup/65816.tar.z.uue
	uuencode backup/65816.tar.z 65816.tar.z > backup/65816.tar.z.uue
65816.o:	65816.h address.h parser.h label.h list.h machine.h directvs.h\
			evaluate.h smc.h
parser.o:	parser.h opcode.h machine.h
list.o:		list.h machine.h
label.o:	label.h machine.h
machine.o:	machine.h
address.o:	address.h parser.h machine.h
opcode.o:	opcode.h opcodes.h
evaluate.o:	evaluate.h label.h machine.h
smc.o:		smc.h machine.h
