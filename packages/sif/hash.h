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
/* $Id: hash.h,v 1.2 2001/06/27 05:02:51 j10 Exp $ */

#ifndef __hash_h
#define __hash_h

#define HASH_SIZE 69351

typedef struct HASHENTRY
{
   void *data;
   unsigned long key;
} HASHENTRY;

int InsertHash(HASHENTRY entry[HASH_SIZE],void *data,unsigned long key);
void *FindHash(HASHENTRY entry[HASH_SIZE],unsigned long key);
void *FindNextHash();

#endif

