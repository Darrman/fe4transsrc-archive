/*
   $Id: fe4ga.c,v 1.2 2002/03/23 05:00:59 j10 Exp $
   Reads in a pointer tables and generate a script for fe4pg to take in as command line
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NPOINTERS 0x231
#define snes2pc(x) ((x & 0x3FFFFF) + 0x200)

long pointers[NPOINTERS];
unsigned char *rommap;

void setbits(unsigned char *map,long start,long end);

int main(int argc,char *argv[])
{
   FILE *fp;
   int i, c, c2, count;
   unsigned long start, end;

   if(argc != 2) {
      fprintf(stderr,"usage: %s <fe4.smc>\n",argv[0]);
      return 1;
   }

   fp = fopen(argv[1],"rb");
   if(!fp) {
      fprintf(stderr,"%s: File not found.\n",argv[1]);
      return 1;
   }

   rommap = (unsigned char *)malloc(0x400200/8);
   if(!rommap) {
      fprintf(stderr,"Out of memory.\n");
      fclose(fp);
      return 1;
   }
   memset(rommap,0,0x400200/8);

   memset(pointers,0,sizeof(pointers));
   fseek(fp,0xfeac2L,SEEK_SET);
   for(i = 0; i < NPOINTERS; i++)
      fread(&pointers[i],3,1,fp);

   end = 0xFFFFFFFF;
   start = snes2pc(pointers[0]);
   count = 0;
   printf("#!/bin/sh\n");
   for(i = 1; i <= NPOINTERS; i++) {
      if(i != NPOINTERS) end = snes2pc(pointers[i]);
      if(i == NPOINTERS || end - start > 0x8000) {
         fseek(fp,snes2pc(pointers[i - 1]),SEEK_SET);
         c = c2 = 0;
         while(!feof(fp)) {
            c = getc(fp);
            if(c == 0xFF && c2 == 0xFF) break;
            c2 = c;
         }
         setbits(rommap,start,ftell(fp));
         printf("echo -e \"-h -p -r \\\"%lX-%lX\\\" -e \\\"@lmh\\\" -m \\\"(06|07)003D\\\"\" > ppt/fe4_%.2X.spec\n",start,ftell(fp),count++);
         start = snes2pc(pointers[i]);
      }
   }

   setbits(rommap,0xDFE80,0xE01FF);
   printf("echo -e \"-h -p -r \\\"0DFE80-0E01FF\\\" -e \\\"FD58@lmh\\\"\" > ppt/femain.spec\n");

   start = end = count = 0;
   while(end < 0x400200) {
      while(!(rommap[end >> 3] & (1 << (end & 7))) && end < 0x400200) end++;
      printf("echo -e \"-h -p -r \\\"%lX-%lX\\\" -e \\\"@lmh\\\" -m \\\"(06|07)003D\\\"\" > ppt/fe4_missing%.2X.spec\n",start,end - 1,count++);
      start = end;
      while((rommap[start >> 3] & (1 << (start & 7))) && start < 0x400200) start++;
      end = start;
   }
   free(rommap);
   fclose(fp);

   return 0;
}

void setbits(unsigned char *map,long start,long end)
{
   for(; start <= end; start++)
      map[start >> 3] |= (1 << (start & 7));
}
