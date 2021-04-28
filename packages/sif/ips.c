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
/* $Id: ips.c,v 1.4 2001/06/30 22:09:47 j10 Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ips.h"

Ips *ips_open(const char *filename,int mode)
{
   Ips *ips;
   FILE *fp;
   char magic[5];

   ips = (Ips *)malloc(sizeof(Ips));
   if(!ips) goto error_0;

   if(strcmp(filename,"-") == 0) {
      ips->mode = mode;
      if(mode == IPS_READ) ips->fp = stdin;
      else {
         ips->fp = stdout;
         fwrite("PATCH",5,1,ips->fp);
      }
      return ips;
   }

   if(mode == IPS_READ) {
      fp = fopen(filename,"rb");
      if(!fp) goto error_1;
      fread(magic,5,1,fp);
      if(strncmp(magic,"PATCH",5)) goto error_2;
   } else {
      fp = fopen(filename,"wb");
      if(!fp) goto error_1;
      fwrite("PATCH",5,1,fp);
   }
   ips->mode = mode;
   ips->fp = fp;

   return ips;
error_2:
   fclose(fp);
error_1:
   free(ips);
error_0:
   return NULL;
}

void ips_close(Ips *ips)
{
   if(ips) {
      if(ips->mode == IPS_WRITE) fwrite("EOF",3,1,ips->fp);
      if(ips->fp != stdout) fclose(ips->fp);
   }
}

int ips_read(void *buffer,unsigned long *addr,unsigned int *size,Ips *ips)
{
   unsigned long ad = 0;  /* addr */
   unsigned short sz = 0; /* size */
   unsigned char rb;      /* run byte */

   fread(&ad,3,1,ips->fp);
   if(ad == 0x464F45) return 0;
   fread(&sz,2,1,ips->fp);
   if(sz != 0) {
      *addr = ((ad >> 16) & 0xFF) | (ad & 0x00FF00) | ((ad & 0xFF) << 16);
      *size = ((sz >> 8) & 0xFF) | ((sz & 0xFF) << 8);
      return fread(buffer,*size,1,ips->fp);
   } else {  /* process RLE */
      fread(&sz,2,1,ips->fp); /* read run count */
      fread(&rb,1,1,ips->fp); /* read run byte */
      *addr = ((ad >> 16) & 0xFF) | (ad & 0x00FF00) | ((ad & 0xFF) << 16);
      *size = ((sz >> 8) & 0xFF) | ((sz & 0xFF) << 8);
      memset(buffer,(int)rb,*size);
      return *size;
   }
}

int ips_write(void *buffer,unsigned long addr,unsigned int size,Ips *ips)
{
   putc((addr >> 16) & 0xFF,ips->fp);
   putc((addr >> 8) & 0xFF,ips->fp);
   putc(addr & 0xFF,ips->fp);
   putc((size >> 8) & 0xFF,ips->fp);
   putc(size & 0xFF,ips->fp);
   return fwrite(buffer,size,1,ips->fp);
}
