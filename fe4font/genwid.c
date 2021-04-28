/*
   $Id: genwid.c,v 1.1 2001/08/27 18:39:58 j10 Exp $
   Generates a width table from the font
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int ReturnWidth(unsigned short f[0x10][0x02])
{
   int maxwidth = 0, i, j;
   unsigned int bit, b1, b2;
   for(i = 0; i < 0x10; i++) {
      b1 = ((f[i][0] >> 8) & 0xFF) | (f[i][0] & 0xFF);
      b2 = ((f[i][1] >> 8) & 0xFF) | (f[i][1] & 0xFF);
      bit = (b1 << 8) | b2;
      for(j = 0x10; j; j--) {
         if(bit & 1) break;
         bit >>= 1;
      }
      if(j > maxwidth) maxwidth = j;
   }

   return maxwidth + 2;
}

int main(int argc,char *argv[])
{
   FILE *fp;
   unsigned char *font, *vwf;
   unsigned short ch[0x10][0x02];
   int i, y, pos;

   if(argc != 3) {
      fprintf(stderr,"Usage: genwid <input.bin> <output.bin>\n");
      return 1;
   }

   fp = fopen(argv[1],"rb");
   if(!fp) {
      fprintf(stderr,"%s: File not found.\n",argv[1]);
      return 1;
   }
   font = (unsigned char *)malloc(0x4000);
   vwf = (unsigned char *)malloc(256);
   if(!font || !vwf) {
      fprintf(stderr,"Out of memory.\n");
      fclose(fp);
      return 1;
   }

   fread(font,1,0x4000,fp);
   fclose(fp);

   pos = 0;
   for(i = 0; i < 256; i++) {
      for(y = 0;y < 0x8; y++) {
         ch[y][0] = *(unsigned short *)(font + pos);
         pos += 2;
      }
      for(y = 0; y < 0x8; y++) {
         ch[y][1] = *(unsigned short *)(font + pos);
         pos += 2;
      }
      pos += 0xE0;
      for(y = 0x8; y < 0x10; y++) {
         ch[y][0] = *(unsigned short *)(font + pos);
         pos += 2;
      }
      for(y = 0x8; y < 0x10; y++) {
         ch[y][1] = *(unsigned short *)(font + pos);
         pos += 2;
      }
      vwf[i] = ReturnWidth(ch);
      pos -= 0x100;
      if((i & 7) == 7) pos += 0x100;
      //pos |= 0x8000;
   }

   fp = fopen(argv[2],"wb");
   if(!fp) {
      fprintf(stderr,"%s: Can't open file.\n",argv[2]);
   } else {
      vwf[0xAF] = 6;
      fwrite(vwf,1,256,fp);
      fclose(fp);
   }

   free(font);
   free(vwf);

   return 0;
}
