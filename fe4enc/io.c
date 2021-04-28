/*
   $Id: io.c,v 1.1 2001/07/02 05:28:26 j10 Exp $
   A few IO routines
*/

#include <stdio.h>

long filelength(FILE *fp)
{
   long save, len;

   save = ftell(fp);
   fseek(fp,0L,SEEK_END);
   len = ftell(fp);
   fseek(fp,save,SEEK_SET);

   return len;
}