/*
	$Id: fe4dec.c,v 1.2 2001/08/28 04:38:01 j10 Exp $
	Fire Emblem 4 - Decoder for Character Portraits
	This program also provides decompression routines for FE4 GFX.
   
	(c)1999 by JAY
*/

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

typedef unsigned char int8;
typedef unsigned short int16;
typedef unsigned long int32;
typedef unsigned long long int64;

#ifdef __GNUC__
#define PACKED __attribute__((packed))
#else
#define PACKED
#endif

struct snes_addr
{
	int16 offset PACKED;
	int8 bank PACKED;
} pt[256];
typedef struct snes_addr snes_addr;

inline int32 snestopc(snes_addr a)
{
	return ((((int32)a.bank - (a.bank >= 0xc0 ? 0xc0L : 0x80L)) << 16L) | (int32)a.offset) + 0x200L;
}

inline int getByte(FILE *stream)
{
	if(((ftell(stream) - 0x200) & 0x7FFF) == 0)
		fseek(stream,0x8000,SEEK_CUR);
	return getc(stream);
}

inline int putByte(int ch,FILE *stream)
{
	return putc(ch,stream);
}

int main(int argc,char **argv)
{
	FILE *rom, *bin;
	int32 start, end;
   int8 prefix;
	int i, c, method, count;
	int dist, len;

	if(argc != 3) {
		printf("Usage: fe4dec <input.smc> <output.bin>\n\n");
		return 1;
	}

	rom = fopen(argv[1],"rb");
	if(!rom) {
		printf("`%s' not found.\n",argv[1]);
		return 1;
	}

	bin = fopen(argv[2],"wb+");
	if(!bin) {
		printf("`%s' can't be open.\n",argv[2]);
		fclose(rom);
		return 1;
	}

	printf("Start (enter 0 for portrait decompress)>");
	fflush(stdout);
	scanf("%lx",&start);

	printf("Length>");
	fflush(stdout);
	scanf("%lx",&end);

	if(start == 0L) {
		fseek(rom,0xab6f9L,SEEK_SET);
		fread(pt,sizeof(snes_addr),256,rom);
	} else {
		fseek(rom,start,SEEK_SET);
	}

	for(c = 1;c <= (start ? 1 : 247);c++) {
		if(!start) fseek(rom,snestopc(pt[c-1]),SEEK_SET);
		printf("Decompressing... %d%%\r",((c-1)*100)/248);
		while((int32)ftell(bin) < end*c) {
			prefix = getByte(rom);
         if((int8)prefix == 0xFF) break;
			method = prefix >> 4;
			count = prefix & 15;
			switch(method) { // check encoding method
				case 0x0:	// literal
					for(i=0;i<count+1;i++) putByte(getByte(rom),bin);
					break;
				case 0x1:
					for(i=0;i<count+17;i++) putByte(getByte(rom),bin);
					break;
				case 0x2:
					for(i=0;i<count+33;i++) putByte(getByte(rom),bin);
					break;
				case 0x3:
					for(i=0;i<count+49;i++) putByte(getByte(rom),bin);
					break;                                  /*
				case 0x4:	// This method is guessed at
					for(i=0;i<count+1;i++) {
						prefix = getByte(rom);
						putByte(prefix,bin);
					}
					break;                                    */
				case 0x5:	// DUP 2x
					for(i=0;i<count+1;i++) {
						prefix = getByte(rom);
						putByte(prefix,bin);
						putByte(prefix,bin);
					}
					break;
				case 0x6:	// PREAPPEND
					prefix = getByte(rom);
					for(i=0;i<count+2;i++) {
						putByte(prefix,bin);
						putByte(getByte(rom),bin);
					}
					break;
				case 0x7:	// POSTAPPEND
					prefix = getByte(rom);
					for(i=0;i<count+2;i++) {
						putByte(getByte(rom),bin);
						putByte(prefix,bin);
					}
					break;
				case 0x8:	// lZ77
					dist = (count & 3) << 8;
					dist |= getByte(rom);
					len = (count >> 2) + 2;
					for(i=0;i<len;i++) {
						fseek(bin,-dist,SEEK_CUR);
						prefix = getByte(bin);
						fseek(bin,0,SEEK_END);
						putByte(prefix,bin);
					}
					break;
				case 0x9:	// lZ77
					dist = (count & 3) << 8;
					dist |= getByte(rom);
					len = (count >> 2) + 6;
					for(i=0;i<len;i++) {
						fseek(bin,-dist,SEEK_CUR);
						prefix = getByte(bin);
						fseek(bin,0,SEEK_END);
						putByte(prefix,bin);
					}
					break;
				case 0xA:	// LZ77
					dist = (count & 3) << 8;
					dist |= getByte(rom);
					len = (count >> 2) + 10;
					for(i=0;i<len;i++) {
						fseek(bin,-dist,SEEK_CUR);
						prefix = getByte(bin);
						fseek(bin,0,SEEK_END);
						putByte(prefix,bin);
					}
					break;
				case 0xB:	// LZ77
					dist = (count & 3) << 8;
					dist |= getByte(rom);
					len = (count >> 2) + 14;
					for(i=0;i<len;i++) {
						fseek(bin,-dist,SEEK_CUR);
						prefix = getByte(bin);
						fseek(bin,0,SEEK_END);
						putByte(prefix,bin);
					}
					break;
				case 0xC:
					dist = getByte(rom) << 8;
					dist |= getByte(rom);

					len = (((unsigned)dist >> 15) | (count << 1)) + 2;
					dist &= 0x7FFF;

					for(i=0;i<len;i++) {
						fseek(bin,-dist,SEEK_CUR);
						prefix = getByte(bin);
						fseek(bin,0,SEEK_END);
						putByte(prefix,bin);
					}
					break;
				case 0xD:	// LZ77
					dist = getByte(rom) << 8;
					dist |= getByte(rom);

					len = (((unsigned)dist >> 15) | (count << 1)) + 34;
					dist &= 0x7FFF;

					for(i=0;i<len;i++) {
						fseek(bin,-dist,SEEK_CUR);
						prefix = getByte(bin);
						fseek(bin,0,SEEK_END);
						putByte(prefix,bin);
					}
					break;
				case 0xE:	// RLE
					len = count << 8;
					len |= getByte(rom);
					prefix = getByte(rom);
					for(i=0;i<len+3;i++) putByte(prefix,bin);
					break;
				case 0xF:	// RLE
					prefix = getByte(rom);
					for(i=0;i<count+3;i++) putByte(prefix,bin);
					break;
				default:
					printf("Method %X is not supported yet.\n",method);
					printf("Debug:\n");
					printf("Stream position: %lXh\n",ftell(rom));
					printf("Code position: %lXh\n",ftell(bin));
					goto exitprog;
			}
		}
	}

	printf("Decompressing... 100%%\n");
	printf("Stream position: %lXh\n",ftell(rom));
	printf("Bytes written: %lXh\n",ftell(bin));
exitprog:
	printf("Done!\n\n");
	fclose(rom);
	fclose(bin);

	return 0;
}
