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
/* $Id: rat.h,v 1.5 2001/07/07 04:42:18 j10 Exp $ */

#ifndef __rat_h
#define __rat_h

#include "section.h"

/* ROM Allocation block */
typedef struct BLOCK
{
   unsigned int size;      /* size of block */
   int section;            /* block section */
   unsigned long address;  /* address start */
   struct BLOCK *next;     /* pointer to next block */
} BLOCK;

/* RAT table structure */
typedef struct Rat
{
   BLOCK *top;
   BLOCK *last;
} Rat;

Rat *LoadRAT(const char *filename,Section *s);
Rat *JoinRAT(Rat *a,Rat *b);
void MergeRAT(Rat *rat);
void SaveRAT(const char *filename,Rat *rat,Section *s);
void FreeRAT(Rat *rat);

void CreateBlock(Rat *rat,unsigned long start,unsigned long end,int section);
unsigned long FindBlock(Rat *rat,Section *s,int section,unsigned int size);

#endif

