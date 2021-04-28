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
/* $Id: rat.c,v 1.7 2001/08/01 15:54:37 j10 Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "cmdline.h"
#include "parse.h"
#include "section.h"
#include "rat.h"

Rat *LoadRAT(const char *filename,Section *s)
{
   char *token, *p;
   char line[256];
   unsigned int section = SECTION_PUBLIC;
   unsigned long linenum = 0, blockstart, blockend;
   FILE *fp;
   Rat *rat;

   if(!filename || !s) return NULL;

   rat = (Rat *)malloc(sizeof(Rat));
   if(!rat) {
      fprintf(stderr,"sif: Out of memory.\n");
      exit(1);
   }
   memset(rat,0,sizeof(Rat));

   /* open rat file */
   fp = fopen(filename,"r");
   if(!fp) {
      fprintf(stderr,"%s: File not found.\n",filename);
      exit(1);
   }

   /* read first line */
   fgets(line,256,fp);
   linenum++;

   p = strchr(line,'#'); /* remove comments */
   if(p) *p = 0;

   token = getNext(line);

   /* Make sure '.rat' is found at the beginning of the file */
   if(!token || strcmp(token,".rat")) {
      fprintf(stderr,"%s %ld: Error! '.rat' must be the first 4 characters of a RAT file.\n",filename,linenum);
      fclose(fp);
      exit(1);
   }

   while(fgets(line,256,fp)) {  /* read it line by line */
      linenum++;

      p = strchr(line,'#'); /* remove comments */
      if(p) *p = 0;

      token = getNext(line);
      if(!token) continue;   /* if line is empty, read next line */

#define tokenis(x) !strcmp(token,x)
#define generror(x) {                              \
   fprintf(stderr,"%s %ld: " x,filename,linenum);  \
   fclose(fp);                                     \
   exit(1);                                        \
}
      /* if token is 'END' break */
      if(tokenis("END")) break;

      /* check for directives */
      if(tokenis("PUBLIC")) { section = SECTION_PUBLIC; continue; }
      else if(tokenis("RESERVED")) { section = SECTION_RESERVED; continue; }
      else if(tokenis("SECTION")) { /* declare a section */
         token = getNext(NULL);
         if(!token) generror("Expecting section name.\n");
         section = ChangeSection(s,token);
         continue;
      } else if(tokenis("COMMON")) { /* common section */
         token = getNext(NULL);
         if(!token) generror("Expecting section name.\n");
         section = ChangeSection(s,token);
         while((token = getNext(NULL)) != NULL) { /* get common section name */
            int i;

            for(i = 0; i < s->numscn; i++) {  /* search section list */
               if(tokenis(s->sectionlist[i])) {
                  setcommon(s,section,i);
                  break;
               }
            }
            if(i == s->numscn) {
               fprintf(stderr,"%s %ld: Warning: Section `%s' not found.\n",filename,linenum,token);
            }
         }
         continue;
      }

      /* if token is not an address */
      if(!isxdigit(*token))
         generror("Parse error. Expecting address start.\n");

      /* read block start */
      blockstart = strtol(token,NULL,16);
      if((int)blockstart < opts.headersize)
         generror("Address is less than header size.\n");

      /* get the separator */
      token = getNext(NULL);
      if(!token)
         generror("Unexpected end of line.\n");

      /* check for the separator */
      if(strcmp(token,"-"))
         generror("'-' expected.\n");

      /* read block end */
      token = getNext(NULL);
      if(!token)
         generror("Unexpected end of line.\n");

      /* if token is not and address */
      if(!isdigit(*token))
         generror("Parse error. Expecting address end.\n");

      blockend = strtol(token,NULL,16);

      if(blockend < blockstart)
         generror("Starting address less than ending address.\n");

      /* create a block in memory */
      CreateBlock(rat,blockstart,blockend,section);
   }
#undef tokenis
#undef generror
   fclose(fp);

   return rat;
}

/* joins 2 rat's together */
Rat *JoinRAT(Rat *a,Rat *b)
{
   if(!a) return b;
   if(!b) return a;

   /* place the last chain to the top of the second */
   a->last->next = b->top;
   a->last = b->last;
   free(b);
   return a;
}

void SaveRAT(const char *filename,Rat *rat,Section *s)
{
   FILE *fp;
   BLOCK *next;
   int cursec;
   int i, j;
   unsigned long remain = 0;

   if(!rat || !filename || !s) return;

   if(!rat->top) return;

   if(strcmp(filename,"-") == 0) fp = stdout;
   else {
      fp = fopen(filename,"w");
      if(!fp) {
         fprintf(stderr,"sif: Can't save RAT to file `%s'.\n",filename);
         return;
      }
   }

   next = rat->top;

   fprintf(fp,".rat\n\n");

   /* print out all the sections first to ensure section ordering */
   for(i = 4; i < s->numscn; i++) {
      if(s->common[i][0] || s->common[i][1]) {
         fprintf(fp,"COMMON %s ",s->sectionlist[i]);
         for(j = 0; j < 64; j++) {
            if(iscommon(s,i,j)) {
               fprintf(fp,"%s ",s->sectionlist[j]);
            }
         }
         fprintf(fp,"\n");
      } else {
         fprintf(fp,"SECTION %s\n",s->sectionlist[i]);
      }
   }
   fprintf(fp,"\n");

   cursec = -1;
   while(next) {
      if(next->section != cursec) {
         fprintf(fp,"SECTION %s\n",s->sectionlist[next->section]);
         cursec = next->section;
      }
      if(next->size > (unsigned int)opts.esbsize) {
         fprintf(fp,"\t%.6lX-%.6lX\t\t# %d bytes\n",next->address,next->address+next->size-1,next->size);
         remain += next->size;
      }
      next = next->next;
   }
   fprintf(fp,"END\n");
   fprintf(fp,"# %ld bytes remaining.\n",remain);
   if(fp != stdout) fclose(fp);
}

void MergeRAT(Rat *rat)
{
   BLOCK *b, *chain;
   unsigned long a1s, a1e, a2s, a2e;

   if(!rat) return;

   /* start with the top node */
   b = rat->top;
   while(b) {
      chain = b->next;

      a1s = b->address;         /* get start address */
      a1e = a1s + b->size - 1;  /* get end address */

      while(chain) {
         a2s = chain->address;
         a2e = a2s + chain->size - 1;

         /* now test for overlap */
         if((a1s <= a2s && a2s <= a1e) ||
            (a2s <= a1s && a1s <= a2e)) {
            /* if sections don't match, output conflict */
            if(b->section != chain->section) {
               fprintf(stderr,"sif: Conflicting addresses (%lX-%lX) ~ (%lX-%lX) in RAT.\n",a1s,a1e,a2s,a2e);
               break;
            }

            /* if there is overlap, enlarge current block */
            b->address = a1s < a2s ? a1s : a2s;
            b->size = ((a1e > a2e ? a1e : a2e) + 1) - b->address;
            chain->size = 0; /* delete conflicted block */

#ifdef __DEBUG__
            if(opts.debug) {
               printf("debug: sif: Merged blocks (%lX-%lX) ~ (%lX-%lX) -> (%lX-%lX).\n",
                  a1s,a1e,a2s,a2e,b->address,b->address + b->size - 1);
            }
#endif
         }

         chain = chain->next;
      }

      b = b->next;
   }
}

void FreeRAT(Rat *rat)
{
   BLOCK *cur, *next;

   if(!rat) return;

   cur = rat->top;
   if(cur) next = cur->next;
   while(cur) {
      free(cur);
      cur = next;
      if(cur) next = cur->next;
   }
   free(rat);
}

/* create a block */
void CreateBlock(Rat *rat,unsigned long start,unsigned long end,int section)
{
   BLOCK *block;
   BLOCK *nb;
   unsigned long size, newend;

   if(!rat) return;

   end++;
   size = end - start;  /* get block size */

   block = rat->last;

   /* check for 32k boundary crossing  */
   while(size > 0x8000L || ((((start - opts.headersize) & 0x7FFFL) + size) & 0x8000L)) {
      newend = (((start - opts.headersize) + 0x8000L) & 0xFFFF8000L) + opts.headersize; /* get end of 32k boundary */

      nb = (BLOCK *)malloc(sizeof(BLOCK));
      if(!nb) {
         fprintf(stderr,"sif: Out of memory.\n");
         exit(1);
      }

      /* allocate next block */
      if(!rat->top) {       /* if first block */
         rat->top = nb;     /* place block in head node */
         block = rat->top;
      } else {
         block->next = nb;    /* otherwise place block in next node on the chain */
         block = block->next;
      }

      /* assign blocks with values */
      block->size = newend - start;
      block->section = section;
      block->address = start;
      block->next = NULL;

      start = newend; /* start at beginning of 32k */
      size = end - start;
   }

   /* if there are block remaining */
   if(size) {
      nb = (BLOCK *)malloc(sizeof(BLOCK));
      if(!nb) {
         fprintf(stderr,"sif: Out of memory.\n");
         exit(1);
      }

      /* allocate next block */
      if(!rat->top) {       /* if first block */
         rat->top = nb;     /* place block in head node */
         block = rat->top;
      } else {
         block->next = nb;    /* otherwise place block in next node on the chain */
         block = block->next;
      }

      /* assign blocks with values */
      block->size = size;
      block->section = section;
      block->address = start;
      block->next = NULL;
   }
   rat->last = block;
}

unsigned long FindBlock(Rat *rat,Section *s,int section,unsigned int size)
{
   BLOCK *block, *best;
   unsigned long addr, smallest = 0x8000;

   if(!rat || !rat->top) return (unsigned long)-1;

   block = rat->top;
   if(opts.bestfit) {  /* if best fit strategy is used */
      best = block;
      while(block) { /* search for smallest block */
         if((block->section == section  || iscommon(s,block->section,section)) &&
            block->size >= size) {
            if(block->size < smallest) {
               smallest = block->size;
               best = block;
            }
         }
         block = block->next;
      }
      /* return the block if it fits */
      if((best->section == section  || iscommon(s,best->section,section)) &&
          best->size >= size) {
         addr = best->address;
         best->address += size;
         best->size -= size;
         return addr;        /* return address of smallest block */
      }
   } else { /* if first fit strategy is used */
      while(block) {
         if((block->section == section  || iscommon(s,block->section,section)) &&
            block->size >= size) {
            addr = block->address;
            block->address += size;
            block->size -= size;
            return addr; /* return address of first avaiable block */
         }
         block = block->next;
      }
   }

   return (unsigned long)-1;
}
