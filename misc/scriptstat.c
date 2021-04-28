/*
   $Id: scriptstat.c,v 1.1 2001/08/27 22:20:53 j10 Exp $
   Generates a percent completion for a script
*/
#include <stdio.h>
#include <string.h>
#include <ctype.h>

int main(int argc,char *argv[])
{
   FILE *fp;
   char *p;
   int linestranslated, linestotal;
   char line[1024];
   int i, cur, asciihit, japhit;

   if(argc < 2) {
      fprintf(stderr,"Usage: %s [scripts ...]\n",argv[0]);
      return 1;
   }

   if(strlen(argv[1]) > 1023) {
      fprintf(stderr,"Filename too long.\n");
      return 1;
   }

   printf("+----------------------------------------------------+---------+\n");
   printf("| Script                                             | %% Done  |\n");
   printf("+----------------------------------------------------+---------+\n");
   for(cur = 1; cur < argc; cur++) {
      linestranslated = 0;
      linestotal = 0;
      fp = fopen(argv[cur],"r");
      if(!fp) {
         fprintf(stderr,"%s: File not found.\n",argv[1]);
         return 1;
      }

      while(fgets(line,1024,fp)) {
         p = strchr(line,'#');
         if(p) *p = 0;

         asciihit = japhit = 0;
         for(i = 0; i < (int)strlen(line); i++) {
            if(line[i] == '[') {
               while(i < (int)strlen(line) && line[i] != ']') i++;
               continue;
            }
            if(!isspace(line[i])) {
               if(isascii(line[i])) asciihit++;
               else japhit++;
            }
         }
         if(japhit == 0 && asciihit != 0) linestranslated++;
         if(japhit != 0 || asciihit != 0) linestotal++;
      }
      strcpy(line,argv[cur]);
      p = strrchr(line,'/');
      if(p) p++;
      else {
         p = strrchr(line,'\\');
         if(p) p++;
         else p = argv[cur];
      }

      printf("| %-50s | %6.1f%% |\n",p,linestranslated * 100.0 / linestotal);
      fclose(fp);
   }
   printf("+----------------------------------------------------+---------+\n");

   return 0;
}
