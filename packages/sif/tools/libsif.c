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
   $Id: libsif.c,v 1.5 2003/04/25 08:26:54 j10 Exp $
   SIF file handling library
*/

/* Last modified: 23/06/2006
   This file was modified by Dark Twilkitri */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* Sif Writer Struct */
struct SifWriter
{
   long nscns;
   char **scnlist;
   FILE *fp;
};

#include "sif.h"
#include "section.h"

/* creates a file for Sif Writing */
SifWriter *CreateSifWriter(const char *filename,const char *sections[],const char *dlmap)
{
   SifWriter *w;
   int i;
   SIF_HEADER h;
   long strtablen = 0;
   int scindex[60];
   int dlindex = 0;
   SIF_SECTION_HEADER sh;
   long next;

   /* alloc memory */
   w = malloc(sizeof(SifWriter));
   if(!w) goto error_0;
   memset(w,0,sizeof(SifWriter));

   if(sections) {
      /* count the number of sections */
      for(w->nscns = 0; sections[w->nscns]; w->nscns++)
         if(w->nscns >= 60) goto error_1;

      /* alloc section list */
      w->scnlist = (char **)malloc(w->nscns * sizeof(char *));
      if(!w->scnlist) goto error_1;

      /* dup the sections */
      for(i = 0; i < w->nscns; i++) {
         w->scnlist[i] = (char *)malloc(strlen(sections[i]) + 1);
         if(!w->scnlist) {
            while(--i > 0) free(w->scnlist[i]);
            goto error_2;
         }
         scindex[i] = strtablen;
         strtablen += strlen(sections[i]) + 1;
         strcpy(w->scnlist[i],sections[i]);
      }
   }

   if(dlmap) {
      dlindex = strtablen;
      strtablen += strlen(dlmap) + 1;
   }

   memset(&h,0,sizeof(h));
   h.magic = SIF_MAGIC;
   h.version = SIF_VERSION;
   h.os = 0;
   h.time = time(NULL);

   /* next addr in header */
   next = sizeof(h);

   /* set pointer to string table */
   if(strtablen) {
      h.strptr = next;
      next += strtablen + 4;
   }

   /* set pointer to sections */
   if(sections) {
      h.scnptr = next;
      next += w->nscns * sizeof(SIF_SECTION_HEADER) + 4;
   }

   /* set pointer to dlmap */
   if(dlmap) {
      h.mapptr = next;
      next += 12;
   }

   /* set pointer to blocks */
   h.blkptr = next;

   w->fp = fopen(filename,"wb");
   if(!w->fp) goto error_3;

   /* write header */
   fwrite(&h,sizeof(h),1,w->fp);

   /* write string table */
   if(strtablen) {
      fwrite(&strtablen,4,1,w->fp);
      if(sections) {
         for(i = 0; i < w->nscns; i++) {
            fwrite(sections[i],strlen(sections[i]),1,w->fp);
            putc(0,w->fp);
         }
      }
      if(dlmap) {
         fwrite(dlmap,strlen(dlmap),1,w->fp);
         putc(0,w->fp);
      }
   }

   /* write section table */
   if(sections) {
      fwrite(&w->nscns,4,1,w->fp);
      for(i = 0; i < w->nscns; i++) {
         sh.scn = i + 4;
         sh.offset = scindex[i];
         fwrite(&sh,sizeof(sh),1,w->fp);
      }
   }

   /* write map */
   if(dlmap) {
      long a;
      a = 1;
      fwrite(&a,4,1,w->fp); /* 1 map */
      a = 0;
      fwrite(&a,4,1,w->fp); /* write reserved */
      fwrite(&dlindex,4,1,w->fp);
   }

   return w;
error_3:
   for(i = 0; i < w->nscns; i++) free(w->scnlist[i]);
error_2:
   free(w->scnlist);
error_1:
   free(w);
error_0:
   return NULL;
}

/* write a block */
int WriteSifBlock(SifWriter *w,void *block,unsigned int size,unsigned long addr,const char *section,int map,unsigned long flags)
{
   SIF_BLOCK_HEADER h;
   int i;

   if(!w) return -1;

   memset(&h,0,sizeof(h));
   if((int)section < 64) {
      h.flag.section = (int)section;
   } else {
      if(strcmp(section,"PUBLIC") == 0) h.flag.section = SECTION_PUBLIC;
      else if(strcmp(section,"RESERVED") == 0) h.flag.section = SECTION_RESERVED;
      else if(strcmp(section,"DIRECT") == 0) h.flag.section = SECTION_DIRECT;
      for(i = 0; i < w->nscns; i++) {
         if(strcmp(section,w->scnlist[i]) == 0) {
            h.flag.section = i + 4;
            break;
         }
      }
   }
   h.flag.map = map;
   if(flags & SIF_LOCAL) h.flag.local = 1;
   if(flags & SIF_USE16) h.flag.use16 = 1;
   if(flags & SIF_NOMERGE) h.flag.nomerge = 1;
   if(flags & SIF_ASM) h.flag.asmptrs = 1;
   if(flags & SIF_ASM3) h.flag.asmptrs3 = 1;
   if(flags & SIF_HASM) h.flag.halfasm = 1;
   if(flags & SIF_NOPTR) h.flag.noptr = 1;
   if(flags & SIF_BLOCK) h.flag.blocked = 1;
   h.address = addr;
   h.size = size;
   fwrite(&h,SIF_BLOCK_HEADER_SIZE,1,w->fp);
   fwrite(block,size,1,w->fp);

   return 0;
}

/* closes the SifWriter object */
int CloseSifWriter(SifWriter *w)
{
   int i;
   SIF_BLOCK_HEADER h;

   if(!w) return -1;

   memset(&h,0,sizeof(h));
   h.flag.eof = 1;
   fwrite(&h,SIF_BLOCK_HEADER_SIZE,1,w->fp);
   fclose(w->fp);
   for(i = 0; i < w->nscns; i++) free(w->scnlist[i]);
   free(w->scnlist);
   free(w);

   return 0;
}
