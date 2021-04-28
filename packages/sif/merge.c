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
/* $Id: merge.c,v 1.4 2003/04/25 08:26:40 j10 Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash.h"
#include "merge.h"
#include "sif.h"

/* creates a MergeDB */
MergeDB *CreateMergeDB()
{
   MergeDB *db;

   db = (MergeDB *)malloc(sizeof(MergeDB));
   if(db) memset(db,0,sizeof(MergeDB));

   return db;
}

void FreeMergeDB(MergeDB *db)
{
   MergeDBEntry *cur = NULL, *next = NULL;

   /* free up the list */
   cur = db->list;
   if(cur) next = cur->next;
   while(cur) {
      free(cur->data);
      free(cur);
      cur = next;
      if(cur) next = cur->next;
   }

   free(db);
}

void AddMergeDB(MergeDB *db,unsigned char *buf,unsigned int size,unsigned long addr,int section)
{
   MergeDBEntry *entry;
   unsigned long key;

   /* allocate memory for a merge entry */
   entry = (MergeDBEntry *)malloc(sizeof(MergeDBEntry));
   if(!entry) {
      fprintf(stderr,"sif: Out of memory.\n");
      exit(1);
   }

   entry->data = (unsigned char *)malloc(size);
   if(!entry->data) {
      fprintf(stderr,"sif: Out of memory.\n");
      exit(1);
   }

   entry->address = addr;
   entry->size = size;
   entry->section = section;
   memcpy(entry->data,buf,size);
   /* add to front of the list */
   entry->next = db->list;
   db->list = entry;

   /* insert a hash entry for fast search */
   key = *(unsigned long *)(buf + size - 4);
   key = (key >> 8) & 0xFFFFFF;

   InsertHash(db->hash,(void *)entry,key);
}

/* find the merged data */
int FindMerge(MergeDB *db,unsigned char *buf,unsigned int size,int section,unsigned long *mergeaddr)
{
   MergeDBEntry *found;
   unsigned long key;

   key = *(unsigned long *)(buf + size - 4);
   key = (key >> 8) & 0xFFFFFF;

   found = FindHash(db->hash,key);
   while(found) {
      if(section == -1 || found->section == section) {
         if(memcmp(buf,found->data + found->size - size,size) == 0) {
            *mergeaddr = found->address + (found->size - size);
            return 0;
         }
      }
      found = FindNextHash();
   }

   return -1;
}
