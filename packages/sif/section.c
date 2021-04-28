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
/* $Id: section.c,v 1.3 2001/06/28 01:22:30 j10 Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "section.h"

Section *CreateSection()
{
   Section *s;

   s = (Section *)malloc(sizeof(Section));
   if(!s) return NULL;
   memset(s,0,sizeof(Section));

   s->numscn = 4;
   s->sectionlist[0] = "PUBLIC";
   s->sectionlist[1] = "RESERVED";
   s->sectionlist[2] = "DIRECT";
   s->sectionlist[3] = "UNUSED";

   return s;
}

void FreeSection(Section *s)
{
   int i;

   for(i = 4; i < s->numscn; i++)
      if(s->sectionlist[i])
         free(s->sectionlist[i]);
   free(s);
}

int ChangeSection(Section *s,const char *name)
{
   int i;

   if(s->numscn >= 64) {
      fprintf(stderr,"sif: Too many sections declared (max 64).\n");
      exit(1);
   }

   /* search for the section, otherwise a new is created */
   for(i = 0; i < s->numscn; i++)
      if(!strcmp(s->sectionlist[i],name)) return i;

   /* create new section */
   s->sectionlist[s->numscn] = strdup(name);
   return s->numscn++;
}

unsigned char *AllocateSectionMap()
{
   int i;
   unsigned char *scnmap;

   scnmap = (unsigned char *)malloc(64);
   if(!scnmap) {
      fprintf(stderr,"sif: Out of memory.\n");
      exit(1);
   }
   for(i = 0; i < 64; i++) scnmap[i] = i;
   return scnmap;
}

char *strdup(const char *str)
{
   int len;
   char *p;

   len = strlen(str);
   p = (char *)malloc(len + 1);
   if(!p) {
      fprintf(stderr,"sif: Out of memory.\n");
      exit(1);
   }
   strcpy(p,str);

   return p;
}
