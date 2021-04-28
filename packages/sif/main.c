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
/* $Id: main.c,v 1.9 2001/07/25 00:13:21 j10 Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cmdline.h"
#include "rat.h"
#include "sif.h"
#include "section.h"
#include "ips.h"
#include "merge.h"

static const char AppString[] = "Sif v5.0";
static void PostHelpMessage();
static void OnExit();

CommandLineOptions opts;
int ParseSif(const char *filename,Rat *rat,Section *s,MergeDB *mergedb,Ips *ips);

static Section *section = NULL;
static Ips *ips = NULL;
static MergeDB *mergedb = NULL;
static Rat *rat = NULL;

int main(int argc,char *argv[])
{
   int i;
   Rat *r;

   if(argc < 2) PostHelpMessage();
   atexit(OnExit);

   /* process commandline options */
   ParseCommandLine(argc,argv,&opts);

   section = CreateSection();
   if(!section) {
      fprintf(stderr,"sif: Out of memory.\n");
      return 1;
   }

   /* process all RAT tables */
   for(i = 0; i < opts.nrats; i++) {
      if(opts.verbose) printf("sif: Processing RAT file '%s'.\n",opts.rat[i]);
      r = LoadRAT(opts.rat[i],section);
      rat = JoinRAT(rat,r);
   }

   /* test for conflict and merge */
   MergeRAT(rat);

   if(opts.merge) {
      mergedb = CreateMergeDB();
      if(!mergedb) {
         fprintf(stderr,"sif: Out of memory.\n");
         return 1;
      }
   }

   if(!opts.outputfilename) opts.outputfilename = "-";
   ips = ips_open(opts.outputfilename,IPS_WRITE);
   if(!ips) {
      fprintf(stderr,"%s: Cannot open file for writing.\n",opts.outputfilename);
      return 1;
   }

   /* process all SIF objects */
   for(i = 0; i < opts.nsifs; i++) {
      if(opts.verbose) printf("sif: Processing SIF file '%s'.\n",opts.sif[i]);
      ParseSif(opts.sif[i],rat,section,mergedb,ips);
   }
   ips_close(ips);
   ips = NULL;

   if(opts.ratfilename) {
      SaveRAT(opts.ratfilename,rat,section);
   }

   return 0;
}

static void PostHelpMessage()
{
   printf("%s (Script Inserter Format)\n",AppString);
   printf("(c) 1997-2001 Jay (tennj@yahoo.com)\n");
   printf("-----------------------------------\n\n");
   printf("Usage: sif [options...] <files.rat...> <files.sif...> [-o output.ips]\n");
   printf("               ^               ^               ^          ^\n");
   printf("               |               |               |          |\n");
   printf("               |               |               |           --- Output filename\n");
   printf("               |               |               |\n");
   printf("               |               |                -------------- SIF Objects\n");
   printf("               |               |\n");
   printf("               |                ------------------------------ ROM Access Tables\n");
   printf("               |\n");
   printf("                ---------------------------------------------- Options\n");
   printf("where the options can be:\n");
   printf("-h, --hirom     to use hirom addressing\n");
   printf("-b, --bestfit   to use best-fit strategy to fit blocks\n");
   printf("-m, --merge     to enable merge optimization\n");
   printf("-n, --noheader  to ignore the 512 byte header\n");
   printf("-e n, --esb n   to eliminate small blocks < size 'n' after insert\n");
   printf("-o filename     to direct the output to a file specified by 'filename'\n");
   printf("-r filename     to save the updated RAT table to 'filename'\n");
   printf("-v, --verbose   to be verbose to stdout\n");
   exit(1);
}

static void OnExit()
{
   if(mergedb) FreeMergeDB(mergedb);
   if(section) FreeSection(section);
   if(rat) FreeRAT(rat);
   if(ips) ips_close(ips);
}
