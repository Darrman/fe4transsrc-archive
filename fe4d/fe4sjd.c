/*
   $Id: fe4sjd.c,v 1.3 2001/08/31 19:20:02 j10 Exp $
   FE4 SHIFT-JIS Dumper
   Since certain text in FE4 is stored SHIFT-JIS format, a separate dumper is used
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc,char *argv[])
{
   FILE *fp;
   long start, size, i;
   unsigned short pointer, jis;

   if(argc != 6) {
      fprintf(stderr,"Usage: fe4sjd <fe4.smc> <table start (hex)> <section> <map> <section name> [> output.txt]\n");
      return -1;
   }

   fp = fopen(argv[1],"rb");
   if(!fp) {
      fprintf(stderr,"%s: File not found.\n",argv[1]);
      return -1;
   }

   start = strtol(argv[2],NULL,16);

   fseek(fp,start,SEEK_SET);
   fread(&size,2,1,fp);
   size &= 0xFFFF;

   printf("# %ld %ld %s\n",strtol(argv[3],NULL,10),strtol(argv[4],NULL,10),argv[5]);
   printf("#*#[section %s]\n",argv[5]);
   printf("#*#[map %ld]\n",strtol(argv[4],NULL,10));
   printf("#*#[bits16]\n");
   printf("#*#[mode-sjis]\n");
   printf("# Table: 0x%.6lX\n\n",start);

   for(i = 0; i < size; i += 2) {
      fseek(fp,start + i,SEEK_SET);
      fread(&pointer,2,1,fp);
      fseek(fp,start + pointer,SEEK_SET);
      printf("# 0x%.6lX:\n",start + pointer);
      printf("# P: %.4X\n",pointer);
      printf("# @@%ld@@\n\n",start + i);

      fread(&jis,2,1,fp);
      while(jis) {
         printf("%c%c",jis & 0xFF,(jis >> 8) & 0xFF);
         fread(&jis,2,1,fp);
      }
      printf("\n\n#*# [exit]\n\n");
   }

   fclose(fp);

   return 0;
}
