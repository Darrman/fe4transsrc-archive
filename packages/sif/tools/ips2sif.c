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
/*
   $Id: ips2sif.c,v 1.3 2003/04/25 08:26:54 j10 Exp $
   (c) 2001 Jay (tennj@yahoo.com)
   Version v1.0

   Program Description:
   Converts a IPS file to SIF file
*/
#include <stdio.h>
#include <string.h>
#include <section.h>
#include <time.h>
#include <sif.h>
#include "../ips.h"
#include "../section.h"

unsigned char buffer[65536];
int main(int argc,char *argv[])
{
   FILE *sif;
   SIF_HEADER h;
   SIF_BLOCK_HEADER bh;
   Ips *ips;
   unsigned long addr;
   unsigned int size;

   if(argc != 3) {
      fprintf(stderr,"Usage: ips2sif <patch.ips> <output.sif>\n");
      return 1;
   }

   /* open the ips */
   ips = ips_open(argv[1],IPS_READ);
   if(!ips) {
      fprintf(stderr,"%s: Could not load ips file.\n",argv[1]);
      return 1;
   }

   /* open sif */
   sif = fopen(argv[2],"wb");
   if(!sif) {
      fprintf(stderr,"%s: Can't open for write.\n",argv[2]);
      ips_close(ips);
      return 1;
   }

   /* set up the main header */
   h.magic = SIF_MAGIC;
   h.version = SIF_VERSION;
   h.os = 0;
   h.time = time(NULL);
   h.blkptr = sizeof(h);
   h.scnptr = 0;
   h.mapptr = 0;
   h.strptr = 0;
   h.rsvd = 0;
   /* write sif header */
   fwrite(&h,sizeof(h),1,sif);

   /* read each block of ips */
   while(ips_read(buffer,&addr,&size,ips)) {
      memset(&bh,0,sizeof(bh));
      bh.flag.section = SECTION_DIRECT; /* place as direct section */
      bh.address = addr;
      bh.size = size;
      fwrite(&bh,SIF_BLOCK_HEADER_SIZE,1,sif);
      fwrite(buffer,size,1,sif);
   }
   /* write EOF */
   memset(&bh,0,sizeof(bh));
   bh.flag.eof = 1;
   fwrite(&bh,SIF_BLOCK_HEADER_SIZE,1,sif);

   fclose(sif);
   ips_close(ips);

   return 0;
}
