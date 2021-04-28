/*
   $Id: sifstub.c,v 1.1 2001/07/26 21:19:56 j10 Exp $
*/
#include <stdio.h>
#include <stdlib.h>
#include <sif.h>

unsigned char buffer[65536];
int main(int argc,char *argv[])
{
   FILE *fp;
   long address, size;
   SifWriter *sif;

   if(argc != 4) {
      fprintf(stderr,"Usage: sifstub <input file> <address (hex)> <output file>\n");
      return -1;
   }

   fp = fopen(argv[1],"rb");
   if(!fp) {
      fprintf(stderr,"%s: File not found.\n",argv[1]);
      return -1;
   }

   address = strtol(argv[2],NULL,16);

   sif = CreateSifWriter(argv[3],NULL,NULL);
   if(!sif) {
      fprintf(stderr,"%s: File could not be created.\n",argv[3]);
      fclose(fp);
      return -1;
   }

   fseek(fp,0,SEEK_END);
   size = ftell(fp);
   fseek(fp,0,SEEK_SET);

   fread(buffer,1,size,fp);
   fclose(fp);

   WriteSifBlock(sif,buffer,size,address,"DIRECT",0,0);
   CloseSifWriter(sif);

   return 0;
}

