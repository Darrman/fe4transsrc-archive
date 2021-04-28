/*
	SIF - Script Insertor Format
	Copyright (C) 2001 Jay
	
	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.
	
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	
	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
/* $Id: cmdline.c,v 1.2 2001/06/28 01:22:30 j10 Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cmdline.h"

/* define the amount of memory to allocate to the command */
#define COMMANDLINESIZE 8192
static char commandline[COMMANDLINESIZE];

int ParseCommandLine(int argc,char **argv,CommandLineOptions *info)
{
   int i;
   FILE *fp;
   long len;
   char *p;

   memset(info,0,sizeof(CommandLineOptions));
   memset(commandline,0,COMMANDLINESIZE);

   /* set up defaults */
   info->headersize = 0x200;
   info->outputfilename = "-";

   /* read the commandline into a string */
   for(i = 1; i < argc; i++) {
      /* process response files for non-UNIX environments */
      if(*argv[i] == '@') {
         fp = fopen(argv[i] + 1,"r");
         if(!fp) {
            fprintf(stderr,"%s: Response file not found.\n",argv[i] + 1);
            exit(1);
         }

         fseek(fp,0L,SEEK_END);
         len = ftell(fp);
         fseek(fp,0L,SEEK_SET);

         /* read info command-line */
         fread(commandline + strlen(commandline),len,1,fp);

         fclose(fp);

         strcat(commandline," ");
      } else {
         /* concatenate commandline */
         strcat(commandline,argv[i]);
         strcat(commandline," ");
      }
   }

   /* grab first token */
   p = strtok(commandline," \t\n\r");
   while(p) {
      /* if it is a commandline option */
      if(*p == '-') {
         p++;
         /* if's a long option */
         if(*p == '-') {
            p++;
            if(strcmp(p,"hirom") == 0) info->hirom = 1;
            else if(strcmp(p,"bestfit") == 0) info->bestfit = 1;
            else if(strcmp(p,"merge") == 0) info->merge = 1;
            else if(strcmp(p,"noheader") == 0) info->headersize = 0;
            else if(strcmp(p,"verbose") == 0) info->verbose = 1;
#ifdef __DEBUG__
            else if(strcmp(p,"debug") == 0) info->debug = 1;
#endif
            else if(strcmp(p,"esb") == 0) {
               p = strtok(NULL," \t\n\r");
               if(!p) {
                  fprintf(stderr,"sif: Expected integer value after --esb.\n");
                  exit(1);
               }
               info->esbsize = strtol(p,NULL,10);
            } else {
               fprintf(stderr,"sif: Unknown option --%s.\n",p);
               exit(1);
            }
         } else {
            while(*p) {
               switch(*p) {
                  case 'h':   info->hirom = 1; break;
                  case 'b':   info->bestfit = 1; break;
                  case 'm':   info->merge = 1; break;
                  case 'n':   info->headersize = 0; break;
                  case 'v':   info->verbose = 1; break;
#ifdef __DEBUG__
                  case 'd':   info->debug = 1; break;
#endif
                  case 'e':   if(p[1]) info->esbsize = strtol(p + 1,NULL,10);
                              else {
                                 p = strtok(NULL," \t\n\r");
                                 if(!p) {
                                    fprintf(stderr,"sif: Expected integer value after -e.\n");
                                    exit(1);
                                 }
                                 info->esbsize = strtol(p,NULL,10);
                              }
                              goto done;
                  case 'o':   if(p[1]) info->outputfilename = p + 1;
                              else info->outputfilename = strtok(NULL," \t\n\r");
                              goto done;
                  case 'r':   if(p[1]) info->ratfilename = p + 1;
                              else info->ratfilename = strtok(NULL," \t\n\r");
                              goto done;
                  default:    fprintf(stderr,"sif: Unknown option '%c'.\n",*p);
                              break;
               }
               p++;
            }
done: ;
         }
      } else { /* else if it's not a commandline option */
         if(strncmp(p + strlen(p) - 4,".sif",4) == 0) {
            info->sif = (const char **)realloc(info->sif,(info->nsifs + 1) * sizeof(const char **));
            if(!info->sif) {
               fprintf(stderr,"sif: Out of memory.\n");
               exit(1);
            }
            info->sif[info->nsifs] = p;
            info->nsifs++;
         } else if(strncmp(p + strlen(p) - 4,".rat",4) == 0) {
            info->rat = (const char **)realloc(info->rat,(info->nrats + 1) * sizeof(const char **));
            if(!info->rat) {
               fprintf(stderr,"sif: Out of memory.\n");
               exit(1);
            }
            info->rat[info->nrats] = p;
            info->nrats++;
         } else {
            fprintf(stderr,"sif: Unrecognized file or option '%s'.\n",p);
            exit(1);
         }
      }
      p = strtok(NULL," \t\n\r");
   }

   return 0;
}

