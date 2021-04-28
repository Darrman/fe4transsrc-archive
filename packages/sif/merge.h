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
/* $Id: merge.h,v 1.3 2003/04/25 08:26:40 j10 Exp $ */

#ifndef __merge_h
#define __merge_h

#include "hash.h"

typedef struct MergeDBEntry
{
   unsigned long address;
   unsigned int size;
   int section;
   unsigned char *data;
   struct MergeDBEntry *next;
} MergeDBEntry;

typedef struct MergeDB
{
   MergeDBEntry *list;
   HASHENTRY hash[HASH_SIZE];
} MergeDB;

MergeDB *CreateMergeDB();
void FreeMergeDB(MergeDB *db);
void AddMergeDB(MergeDB *db,unsigned char *buf,unsigned int size,unsigned long addr,int section);
int FindMerge(MergeDB *db,unsigned char *buf,unsigned int size,int section,unsigned long *mergeaddr);

#endif

