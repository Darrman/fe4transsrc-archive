/*
	SIF - Script Insertor Format
	Copyright (C) 2001 Jay
	
	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.
	
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	
	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
/*
   $Id: scanz.c,v 1.1 2001/06/28 19:51:55 j10 Exp $
   (c) 1997 - 2000 Jay (tennj@yahoo.com)
   Version 1.1

   Program Description:
   Generates a RAT table of free space by analysis a rom for
   consecutive bytes that seem unused.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __GNUC__
#define PACKED __attribute__((packed))
#else
#define PACKED
#endif
#define MIN_MATCH 0x20

/* cart type */
const char *Cart_Type[256] =
{
   "ROM","ROM + RAM","ROM + SRAM",
   "ROM + DSP","ROM + DSP + RAM","ROM + DSP + SRAM","FX"
};

/* ram sizes */
const char *RAM_Size[256] =
{
   "0 KBits","16 KBits","32 KBits","64 KBits","128 KBits","256 KBits"
};

/* country codes */
const char *Country_Code[256] =
{
   "Japan","America","PAL","Sweden","Finland","Denmark","France",
   "Holland","Spain","Germany","Italy","China","India","Korea"
};

typedef struct snes_info
{
   char title[21]             PACKED;
   unsigned char mode         PACKED;
   unsigned char type         PACKED;
   unsigned char romsize      PACKED;
   unsigned char ramsize      PACKED;
   unsigned char country      PACKED;
   unsigned char maker        PACKED;
   unsigned char version      PACKED;
   unsigned short checksumc   PACKED;
   unsigned short checksum    PACKED;
   unsigned char pad[0xa]     PACKED;
   unsigned short nmi         PACKED;
   unsigned char pad2[0x10]   PACKED;
   unsigned short reset       PACKED;
} snes_info;

snes_info info;

int main(int argc,char *argv[])
{
   long i, j, zero;
   unsigned char *rom;
   FILE *fp;
   long start, end, bytes = 0, romsize;
   const long off[4] = { 0x7fc0L, 0x81c0L, 0xffc0L, 0x101c0L };
   int header = 1, hirom = 0;

   /* check params */
   if(argc != 3) {
      fprintf(stderr,"Usage: scanz <rom.smc> <zero>\n");
      return 1;
   }

   /* load rom */
   fp = fopen(argv[1],"rb");
   if(!fp) {
      fprintf(stderr,"%s: File not found\n",argv[1]);
      return 1;
   }

   /* get romsize */
   fseek(fp,0L,SEEK_END);
   romsize = ftell(fp);
   fseek(fp,0L,SEEK_SET);

   /* allocate memory for rom */
   rom = (unsigned char *)malloc(romsize);
   if(!rom) {
      fprintf(stderr,"scanz: Out of memory.\n");
      fclose(fp);
      return 1;
   }

   /* read to mem */
   fread(rom,1,romsize,fp);
   fclose(fp);

   /* detect header */
   for(i = 0; i < 4; i++) {
      memcpy(&info,rom + off[i],sizeof(snes_info));
      if((info.checksum + info.checksumc) == 0xFFFF) break;
   }

   /* print header info */
   switch(i) {
      case 0: header = 0; break;
      case 1: hirom = 0; break;
      case 2: header = 0;
      case 3: hirom = 1; break;
      default: fprintf(stderr,"%s: Bad checksum.\n",argv[1]); break;
   }

   info.title[20] = 0;

   printf(".rat\n\n");

   printf("# Filename: %s\n",argv[1]);
   printf("# Header: %s\n",header ? "Yes" : "No");
   printf("# Title: %s\n",info.title);
   printf("# HiROM: %s\n",hirom ? "Yes" : "No");
   printf("# Type: %s\n",Cart_Type[info.type]);
   printf("# Rom Size: ");
   switch(info.romsize) {
      case 0x8: printf("2 MB\n"); break;
      case 0x9: printf("4 MB\n"); break;
      case 0xA: printf("8 MB\n"); break;
      case 0xB: printf("16 MB\n"); break;
      case 0xC: printf("32 MB\n"); break;
      default:  printf("Unknown\n"); break;
   }
   printf("# Ram Size: %s\n",RAM_Size[info.ramsize]);
   printf("# File Size: %ld\n",romsize);
   printf("# Country: %s\n\n",Country_Code[info.country]);
   printf("# NMI/VBL: %.4X\n",info.nmi);
   printf("# Reset: %.4X\n\n",info.reset);

   printf("PUBLIC\n");

   zero = strtol(argv[2],NULL,16);

   for(i = 0; i < romsize; i += 0x10) {
      /* match MIN_MATCH zeroes */
      for(j = 0; j < MIN_MATCH; j++)
         if(rom[i + j] != zero) break;

      /* if all match then grab as much as possible */
      if(j == MIN_MATCH) {
         start = i;
         for(j = 0; j < romsize - i; j++)
            if(rom[i + j] != zero) break;
         end = i + j;
         end &= 0xFFFFFFF0;
         end--;
         if((end & 0xFFF) == (header ? 0x1FF : 0xFFF)) {
            printf("\t%.6lX-%.6lX\t# %ld bytes\n",start,end,end - start + 1);
            bytes += end - start + 1;
         }
         i = end & 0xFFFFFFF0;
      }
   }
   printf("END\n# %ld bytes total.\n",bytes);
   free(rom);

   return 0;
}

