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
/* $Id: cmdline.h,v 1.3 2001/06/28 01:22:30 j10 Exp $ */

#ifndef __cmdline_h
#define __cmdline_h

typedef struct CommandLineOptions
{
   int hirom;
   int bestfit;
   int merge;
   int headersize;
   int esbsize;
   int verbose;
   int debug;
   int nsifs;
   int nrats;
   const char **sif;
   const char **rat;
   const char *outputfilename;
   const char *ratfilename;
} CommandLineOptions;

extern CommandLineOptions opts;
int ParseCommandLine(int argc,char **argv,CommandLineOptions *info);

#endif

