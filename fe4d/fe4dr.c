/*
   $Id: fe4dr.c,v 1.5 2002/02/05 05:12:52 j10 Exp $
   FE4 Dump RAT
   (c) 2001 Jay2E
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "evil.c"

typedef struct Block
{
   unsigned long addr;
   unsigned long size;
} Block;

Block *block;

void *xmalloc(unsigned int size)
{
   void *p;
   p = malloc(size);
   if(!p) {
      fprintf(stderr,"Out of memory.\n");
      exit(1);
   }
   return p;
}

int sorter(const Block *a,const Block *b)
{
   return a->addr > b->addr ? 1 : -1;
}

int main(int argc,char *argv[])
{
   FILE *fp;
   unsigned long *ppt, count;
   long nppt, pointer;
   int i, j, c, combine = 0, bits16 = 0, sjismode = 0, addme = 0;
   Block b;

   if(argc < 3) {
      fprintf(stderr,"Usage: fe4dr [-csja<n>] <rom.smc> <table.ppt>\n"
                     "where '-c' combines addresses\n"
                     "      '-s' uses 16-bit pointers\n"
                     "      '-j' uses SJIS mode\n"
                     "      '-a<n>' offsets by n\n");
      return 1;
   }

   while((c = getopt(argc,argv,"csja:")) > 0) {
      if(c == 'c') combine = 1;
      else if(c == 's') bits16 = 1;
      else if(c == 'j') sjismode = 1;
      else if(c == 'a') addme = strtol(optarg,NULL,10);
      else {
         fprintf(stderr,"Invalid option '-%c'.\n",c);
         return 1;
      }
   }

   // open ppt
   fp = fopen(argv[optind + 1],"rb");
   if(!fp) {
      fprintf(stderr,"%s: File not found.\n",argv[optind + 1]);
      return 1;
   }

   fseek(fp,0L,SEEK_END);
   nppt = ftell(fp) / 4;
   fseek(fp,0L,SEEK_SET);

   // load ppt
   ppt = xmalloc(nppt * 4);
   fread(ppt,4,nppt,fp);
   fclose(fp);

   // allocate blocks
   block = xmalloc(sizeof(Block) * nppt);

   fp = fopen(argv[optind],"rb");
   if(!fp) {
      free(ppt);
      free(block);
      fprintf(stderr,"%s: File not found.\n",argv[optind]);
      return 1;
   }

   for(i = 0; i < nppt; i++) {
      fseek(fp,ppt[i],SEEK_SET);
      pointer = ftell(fp) - 0x200L;
      fread(&pointer,bits16 ? 2 : 3,1,fp);
      pointer &= 0x3FFFFF;

      block[i].addr = pointer + 0x200;
      block[i].size = addme;

      fseek(fp,block[i].addr + addme,SEEK_SET);
      if(sjismode) {
         do {
            block[i].size += 2;
            c = 0;
            fread(&c,2,1,fp);
         } while(c);
      } else {
         do {
            if((c = getc(fp)) == 0) {
               c = getc(fp);
               block[i].size += 2;
               for(j = 0; j < evil[c]; j++) {
                  getc(fp);
                  block[i].size++;
               }
            } else {
               block[i].size++;
            }
         } while(c != 1);
      }
   }

   qsort(block,nppt,sizeof(Block),(int (*)(const void *,const void *))sorter);
   count = 0;
   b.addr = block[0].addr;
   b.size = block[0].size;
   for(i = 0; i < nppt; i++) {
      if(b.addr + b.size == block[i + 1].addr && i + 1 < nppt && combine) {
         b.size += block[i + 1].size;
      } else {
         printf("\t%.6lX-%.6lX\t# %ld bytes\n",b.addr,b.addr + b.size - 1,b.size);
         b.addr = block[i + 1].addr;
         b.size = block[i + 1].size;
      }
      count += block[i].size;
   }

   free(block);
   free(ppt);
   fclose(fp);

   return 0;
}
