#include <stdio.h>
#include <string.h>
#include <stdlib.h>

unsigned char widthtab[0x100];
unsigned char widths[0x100];

FILE *rom = 0, *tbl = 0;

void destruct()
{
   if(rom) fclose(rom);
   if(tbl) fclose(tbl);
}

int main(int argc,char *argv[])
{
   int hex;
   char eq, ch, nl;
   int i;

   if(argc != 3) {
      printf("Usage: wid2c <vwftable> <table> [> output.c]\n");
      return 1;
   }

   atexit(destruct);

   rom = fopen(argv[1],"rb");
   if(!rom) {
      fprintf(stderr,"%s: File not found.\n",argv[1]);
      return 1;
   }
   tbl = fopen(argv[2],"r");
   if(!tbl) {
      fprintf(stderr,"%s: File not found.\n",argv[2]);
      return 1;
   }

   fread(widthtab,1,256,rom);

   memset(widths,0x08,0x100);

   while(!feof(tbl)) {
      if(fscanf(tbl,"%X%c%c%c",&hex,&eq,&ch,&nl)==EOF) break;
      widths[(int)ch] = widthtab[hex - 0x10];
   }

   printf("unsigned char widths[256] = {");
   for(i = 0; i < 256; i++) {
      if((i % 16) == 0) printf("\n\t");
      printf("0x%.2X, ",widths[i]);
   }
   printf("\n};\n");

   return 0;
}
