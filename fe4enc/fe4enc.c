/*
   $Id: fe4enc.c,v 1.1 2001/07/02 05:28:26 j10 Exp $
   Fire Emblem 4 Binary Compressor v1.0
   (c)1999 Jay 2 E
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fe4enc.h"

FILE *out;
unsigned char *file; /* buffer to store the input file */
unsigned long size, fpos;
unsigned char literal[64];
unsigned int lpos = 0;

static int DetermineEfficientMethod(int r,int d,int pr,int po,LZ77Info *l);
static void FlushLiterals();
static void IncrementFpos(int n);

int main(int argc,char *argv[])
{
   FILE *fp;
   int rlecount, duppairs, precnt, postcnt, method;
   short i;
   LZ77Info lzlen;

   if(argc != 3) {
      printf("Usage: fe4enc <input.bin> <output.bin>\n\n");
      return 1;
   }

   fp = fopen(argv[1],"rb");
   if(!fp) {
      fprintf(stderr,"%s: File not found.\n",argv[1]);
      return 1;
   }

   size = filelength(fp);

   file = (unsigned char *)malloc(size);
   if(!file) {
      fprintf(stderr,"Error: Out of memory. Tried to allocate %ld bytes.\n",size);
      fclose(fp);
      return 1;
   }

   fread(file,1,size,fp);
   fclose(fp);

   out = fopen(argv[2],"wb");
   if(!out) {
      fprintf(stderr,"%s: File could not be opened. Possibly out of space or access error.\n",argv[2]);
      free(file);
      return 1;
   }

   fpos = lpos = 0;
   lz77_setup();
   while(fpos < size) {
      printf("Compressing... %.1f%%\r",fpos*100.0/size);
      fflush(stdout);

      HASH(file[fpos],file[fpos+1]);

      rlecount = rle_analyse(file + fpos,size-fpos);
      duppairs = dup2x_analyse(file + fpos,size-fpos);
      precnt = preappend_analyse(file + fpos,size-fpos);
      postcnt = postappend_analyse(file + fpos,size-fpos);
      lz77_analyze(&lzlen,file + fpos,5,size-fpos);

      method = DetermineEfficientMethod(rlecount,duppairs,precnt,postcnt,&lzlen);
again:
      switch(method) {
         case 0: /* Method RLE */
            if(rlecount < 3) break;
            FlushLiterals();

            putc(0xF0 + (rlecount - 3),out);
            putc(file[fpos],out);
            IncrementFpos(rlecount);
            continue;
         case 1: /* Method Dup2x */
            if(duppairs < 1) break;
            FlushLiterals();

            putc(0x50 + (duppairs - 1),out);
            for(i = 0;i < duppairs;i++) {
               putc(file[fpos],out);
               IncrementFpos(2);
            }
            continue;
         case 2: /* Method PreAppend */
            if(precnt < 2) break;
            FlushLiterals();

            putc(0x60 + (precnt - 2),out);
            putc(file[fpos],out);
            for(i = 0;i < precnt;i++) {
               putc(file[fpos + 1],out);
               IncrementFpos(2);
            }
            continue;
         case 3: /* Method PostAppend */
            if(postcnt < 2) break;
            FlushLiterals();

            putc(0x70 + (postcnt - 2),out);
            putc(file[fpos + 1],out);
            for(i = 0;i < postcnt;i++) {
               putc(file[fpos],out);
               IncrementFpos(2);
            }
            continue;
         case 4: /* Method LZ77 */
            if(lzlen.length < 2) break;

            if(lzlen.distance < 512) {
               if(lzlen.length <= 5) {
                  FlushLiterals();
                  putc(0x80 + ((lzlen.length-2) << 2) + ((lzlen.distance >> 8) & 3),out);
                  putc(lzlen.distance & 0xFF,out);
               } else if(lzlen.length <= 9) {
                  FlushLiterals();
                  putc(0x90 + ((lzlen.length-6) << 2) + ((lzlen.distance >> 8) & 3),out);
                  putc(lzlen.distance & 0xFF,out);
               } else if(lzlen.length <= 13) {
                  FlushLiterals();
                  putc(0xA0 + ((lzlen.length-10) << 2) + ((lzlen.distance >> 8) & 3),out);
                  putc(lzlen.distance & 0xFF,out);
               } else if(lzlen.length <= 17) {
                  FlushLiterals();
                  putc(0xB0 + ((lzlen.length-14) << 2) + ((lzlen.distance >> 8) & 3),out);
                  putc(lzlen.distance & 0xFF,out);
               } else {
                  method = DetermineEfficientMethod(rlecount,duppairs,precnt,postcnt,NULL);
                  goto again;
               }
               IncrementFpos(lzlen.length);
               continue;
            } else if(lzlen.distance < WINDOW_SIZE) {
               if(lzlen.length <= 33) {
                  FlushLiterals();
                  putc(0xC0 + (((lzlen.length-2) >> 1) & 0xF),fp);
                  putc((((lzlen.length-2) & 1) << 7) | ((lzlen.distance >> 8) & 0x7F),out);
                  putc(lzlen.distance & 0xFF,out);
               } else if(lzlen.length <= 49) {
                  FlushLiterals();
                  putc(0xD0 + (((lzlen.length-34) >> 1) & 0xF),fp);
                  putc((((lzlen.length-34) & 1) << 7) | ((lzlen.distance >> 8) & 0x7F),out);
                  putc(lzlen.distance & 0xFF,out);
               } else {
                  method = DetermineEfficientMethod(rlecount,duppairs,precnt,postcnt,NULL);
                  goto again;
               }
               IncrementFpos(lzlen.length);
               continue;
            }   
            method = DetermineEfficientMethod(rlecount,duppairs,precnt,postcnt,NULL);
            goto again;
      }

      literal[lpos] = file[fpos];
      lpos++;
      fpos++;
      if(lpos >= 64) FlushLiterals();
   }
   FlushLiterals();
   putc(0xFF,out);
   printf("Compressing... 100.0%%\n");

   free(file);
   fclose(out);

   return 0;
}

/*
 * DetermineEfficientMethod:
 * This function determines the most efficient method to use by comparing the
 * uncompressed:compressed ratio for each method. It will return an index to the
 * best working method.
 */
static int DetermineEfficientMethod(int r,int d,int pr,int po,LZ77Info *l)
{
   double ratio[5], max = 0;
   short i;
   int index = 0;

   ratio[0] = (double)r / 2.0;
   ratio[1] = (double)d*2.0 / ((double)d + 1.0);
   ratio[2] = (double)pr*2.0 / ((double)pr + 2.0);
   ratio[3] = (double)po*2.0 / ((double)po + 2.0);
   if(l) ratio[4] = (double)l->length / ((double)l->distance < 512.0 ? 2.0 : 3.0);
   else ratio[4] = 0;

   for(i = 0;i < 5;i++) {
      if(ratio[i] > max) {
         max = ratio[i];
         index = i;
      }
   }

   return index;
}

static void FlushLiterals()
{
   if(!lpos) return;

   if(lpos <= 15+1) putc(0x00 + (lpos - 1),out);
   else if(lpos <= 15+17) putc(0x10 + (lpos - 17),out);
   else if(lpos <= 15+33) putc(0x20 + (lpos - 33),out);
   else if(lpos <= 15+49) putc(0x30 + (lpos - 49),out);

   fwrite(literal,1,lpos,out);
   lpos = 0;
}

static void IncrementFpos(int n)
{
   short i;

   fpos++;
   if(n == 1) return;

   for(i=1;i<n;i++) {
      HASH(file[fpos],file[fpos+1]);
      fpos++;
   }
}
