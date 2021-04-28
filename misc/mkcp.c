/* $Id: mkcp.c,v 1.3 2002/09/14 21:16:33 j10 Exp $ */

#include <stdio.h>
#include <sif.h>

char cp[] =
   "(c) 2000-2002 FE4 Translations\n"
   "(c) 2000-2002 Boo & Jay\n"
   "http://fe4trans.freeshell.org\n";

int main()
{
   SifWriter *sif;

   sif = CreateSifWriter("fe4-j2e.sif",NULL,"fe4map.so");
   if(!sif) {
      fprintf(stderr,"fe4-j2e.sif: File could not be created.\n");
      return -1;
   }

   WriteSifBlock(sif,cp,sizeof(cp),3244700,"PUBLIC",0,0);
   CloseSifWriter(sif);
   return 0;
}
