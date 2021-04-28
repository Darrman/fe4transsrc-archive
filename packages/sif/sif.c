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
/* $Id: sif.c,v 1.6 2003/04/25 08:26:40 j10 Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "section.h"
#include "cmdline.h"
#include "rat.h"
#include "sif.h"
#include "ips.h"
#include "map.h"
#include "merge.h"

unsigned char buffer[65536];

int ParseSif(const char *filename,Rat *rat,Section *s,MergeDB *mergedb,Ips *ips)
{
   SIF_HEADER sh;
   SIF_BLOCK_HEADER h;
   SIF_SECTION_HEADER ssh;
   FILE *fp;
   char *stringtable = NULL;
   int i, ret = -1;
   unsigned char *scnmap;
   unsigned long address, snesaddr;
   Map *map = NULL;

   /* open SIF file */
   fp = fopen(filename,"rb");
   if(!fp) {
      fprintf(stderr,"%s: File not found.\n",filename);
      goto error_0;
   }

   /* read header */
   fread(&sh,sizeof(sh),1,fp);
   if(sh.magic != SIF_MAGIC) {
      fprintf(stderr,"%s: Invalid SIF file.\n",filename);
      goto error_1;
   }

   if(sh.version != SIF_VERSION && sh.version != 0x0350)
   {
      fprintf(stderr,"%s: Incompatible SIF version.\n",filename);
      goto error_1;
   }

#ifdef __DEBUG__
   if(opts.debug) printf("debug: %s: v%s detected.\n",filename,sh.version == 0x350 ? "3.5" : "5.0");
#endif

   /* backward compatibility with SIF v3.5 */
   if(sh.version == 0x0350) {
      memset(&sh,0,sizeof(sh));
      sh.blkptr = 6;
   }

   /* string table pointer */
   if(sh.strptr) {
      long len;
      /* seek */
      fseek(fp,sh.strptr,SEEK_SET);
      /* read length of table */
      fread(&len,4,1,fp);
      /* allocate memory for string table */
      stringtable = (char *)malloc(len);
      /* read in the table */
      fread(stringtable,len,1,fp);
   }

   scnmap = AllocateSectionMap();
   /* read sections */
   if(sh.scnptr) {
      long nscns;

      if(!sh.strptr) {
         fprintf(stderr,"%s: Sections have undefined names.\n",filename);
         goto error_3;
      }

      /* seek to section info */
      fseek(fp,sh.scnptr,SEEK_SET);
      fread(&nscns,4,1,fp);

#ifdef __DEBUG__
      if(opts.debug) printf("debug: %s: %ld remapped sections.\n",filename,nscns);
#endif
      if(nscns < 0 || nscns >= 60) {
         fprintf(stderr,"%s: Invalid number of sections defined in SIF file.\n",filename);
         goto error_3;
      }

#ifdef __DEBUG__
      if(opts.debug) {
         printf("debug: %s: Section->Mapped\n",filename);
         printf("debug: %s: ---------------\n",filename);
      }
#endif

      /* remap */
      for(i = 0; i < nscns; i++) {
         fread(&ssh,sizeof(ssh),1,fp);
         if(ssh.scn < 4) {
            fprintf(stderr,"%s: Sif file contains remapping default sections.\n",filename);
				goto error_3;
			}
         scnmap[ssh.scn] = ChangeSection(s,&stringtable[ssh.offset]);
#ifdef __DEBUG__
         if(opts.debug) printf("debug: %s:    %.2d  ->  %.2d\n",filename,ssh.scn,scnmap[ssh.scn]);
#endif
      }
   }

   /* load maps */
   if(sh.mapptr) {
      long nmaps;
      long moffset;

      /* seek to MAP */
      fseek(fp,sh.mapptr,SEEK_SET);
      /* read number of maps */
      fread(&nmaps,4,1,fp);
      fseek(fp,4L,SEEK_CUR); /* skip reserved */
      if(nmaps > 1) {
         fprintf(stderr,"%s: Multiple maps not supported.\n",filename);
         goto error_3;
      }
      for(i = 0; i < nmaps && i < 32; i++) {
         fread(&moffset,4,1,fp);
         /* load map here */
         map = LoadMap(&stringtable[moffset]);
         if(!map) {
            fprintf(stderr,"%s: Cannot load '%s'.\n",filename,&stringtable[moffset]);
            goto error_3;
         }
#ifdef __DEBUG__
         if(opts.debug) printf("debug: %s: Loaded map '%s'.\n",filename,&stringtable[moffset]);
#endif
      }
   }

   /* process blocks */
   fseek(fp,sh.blkptr,SEEK_SET);
   while(!feof(fp)) {
      /* read block header */
      fread(&h,SIF_BLOCK_HEADER_SIZE,1,fp);
      if(h.flag.eof) break;

      /* process extensions */
      if(h.flag.extend) {
         fseek(fp,h.size,SEEK_CUR);
         continue;
      }

      /* read in buffer */
      fread(buffer,h.size,1,fp);

      /* if section is direct, then write literal bytes */
      if(h.flag.section == SECTION_DIRECT) {
#ifdef __DEBUG__
         if(opts.debug)
            printf("debug: %s: Direct block of size %d written to 0x%.8lX.\n",filename,h.size,h.address);
#endif
         ips_write(buffer,h.address,h.size,ips);
         continue;
      }

      /* do merge */
      if(mergedb && !h.flag.nomerge) {
         if(FindMerge(mergedb,buffer,h.size,h.flag.use16 ? scnmap[h.flag.section] : -1,&address) == 0) {
#ifdef __DEBUG__
            if(opts.debug)
               printf("debug: %s: Block of size %d merged to 0x%.8lX.\n",filename,h.size,address);
#endif
            ips_write(&address,h.address,h.flag.use16 ? 2 : 3,ips);
            continue;
         }
      }

      /* find block */
      address = FindBlock(rat,s,scnmap[h.flag.section],h.size);
      if((long)address == -1) {
         fprintf(stderr,"%s: There is no block greater than size %d bytes remaining in section '%s'.\n",
            filename,h.size,s->sectionlist[scnmap[h.flag.section]]);
         goto error_4;
      }

      /* modify the pointer */
      if(h.flag.map == 0 || !map) snesaddr = pc2snes(address - opts.headersize);
      else snesaddr = map->mapfunc(address - opts.headersize,h.flag.map);

      ips_write(&snesaddr,h.address,h.flag.use16 ? 2 : 3,ips);
      /* write block */
      ips_write(buffer,address,h.size,ips);
      /* add buffer to mergeDB */
      if(mergedb) AddMergeDB(mergedb,buffer,h.size,snesaddr,scnmap[h.flag.section]);
#ifdef __DEBUG__
         if(opts.debug) {
            printf("debug: %s: Block of size %d relocated to 0x%.8lX(%.6lX).\n",filename,h.size,address,snesaddr);
            printf("debug: %s: sec = %d/%d, map = %d, adr = %lX.\n",filename,h.flag.section,
               scnmap[h.flag.section],h.flag.map,h.address);
         }
#endif
   }

   ret = 0;

error_4:
   if(map) FreeMap(map);
error_3:
   free(scnmap);
/* error_2: */
   if(stringtable) free(stringtable);
error_1:
   fclose(fp);
error_0:
   return ret;
}

