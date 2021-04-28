/*
   $Id: fe4fd.c,v 1.3 2001/12/24 18:05:11 j10 Exp $
   FE4 SHIFT-JIS Fixed Text Dumper
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc,char *argv[])
{
   FILE *fp, *ppt;
   long p2p, addr, pointer;
   unsigned short jis;
   int c, i;
   int bit16 = 0, direct = 0, section = 0, addme = 0;
   char scname[256];

   if(argc < 3) {
      fprintf(stderr,"Usage: fe4fd [-sd] [-n <sectionname>] [-a <offset>] <fe4.smc> <ppt-table> [> output.txt]\n");
      return -1;
   }

   while((c = getopt(argc,argv,"sda:n:")) > 0) {
      switch(c) {
         case 's': bit16 = 1; break;
         case 'd': direct = 1; break;
         case 'n': if(strlen(scname) >= 256) {
                     fprintf(stderr,"Section name too long.\n");
                     return 1;
                   }
                   strcpy(scname,optarg);
                   section = 1;
                   break;
         case 'a': addme = strtol(optarg,NULL,10);
                   break;
         default: fprintf(stderr,"Unknown option -%c.\n",c);
                  return 1;
      }
   }

   fp = fopen(argv[optind],"rb");
   if(!fp) {
      fprintf(stderr,"%s: File not found.\n",argv[optind]);
      return -1;
   }

   ppt = fopen(argv[optind+1],"rb");
   if(!ppt) {
      fprintf(stderr,"%s: File not found.\n",argv[optind+1]);
      fclose(fp);
      return -1;
   }

   if(section) printf("# 0 0 %s\n[section %s]\n",scname,scname);
   if(direct) printf("[section DIRECT]\n");
   if(bit16) printf("[bits16]\n");
   printf("[mode-sjis]\n\n");

   while(fread(&p2p,4,1,ppt)) {
      fseek(fp,p2p,SEEK_SET);
      pointer = ftell(fp) - 0x200L;
      fread(&pointer,bit16 ? 2 : 3,1,fp);
      addr = (pointer & 0x3FFFFF) + 0x200L;
      fseek(fp,addr,SEEK_SET);

      printf("# 0x%.6lX:\n",addr);
      printf("# P: %.4lX\n",p2p);
      printf("# @@%ld@@\n\n",direct ? addr : p2p);

      if(addme != 0) printf("[");
      for(i = 0; i < addme; i++) {
         printf("$%.2X",fgetc(fp));
      }
      if(addme != 0) printf("]\n");

      fread(&jis,2,1,fp);
      while(jis) {
         printf("%c%c",jis & 0xFF,(jis >> 8) & 0xFF);
         fread(&jis,2,1,fp);
      }
      printf("\n\n[exit]\n\n");
   }

   fclose(ppt);
   fclose(fp);

   return 0;
}
