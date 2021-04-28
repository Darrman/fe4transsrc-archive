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
/* $Id: sif.h,v 1.5 2001/08/28 23:23:59 j10 Exp $ */

/*
 * Sif - Script Insertion Format
 * Version: 5.0
 * Copyright (c) 1997 - 2001 Jay!
 */

/* Last modified: 05/07/04
   This file was modified by Dark Twilkitri */
#ifndef __sif_h
#define __sif_h

#define SIF_MAGIC             0x1a464953L
#define SIF_VERSION           0x0500

/* Sif Header */
typedef struct SIF_HEADER
{
	unsigned long magic;       /* 'SIF\x1A' (0x1a464953)*/
	unsigned short version;    /* Version (0x0500) */
	unsigned short os;         /* OS byte */
	unsigned long time;        /* Time/Date stamp */
	unsigned long blkptr;      /* Block Header start */
	unsigned long scnptr;      /* Section table start */
	unsigned long mapptr;      /* Mapping libraries start */
   unsigned long strptr;      /* String table pointer */
	unsigned long rsvd;        /* Reserved */
} SIF_HEADER;

/* Block header */
typedef struct SIF_BLOCK_HEADER
{
	struct {
		unsigned int eof		: 1;	/* End of file */
		unsigned int section	: 6;	/* Section it belongs to */
		unsigned int map		: 5;	/* Map to use */
		unsigned int local	: 1;	/* find local bank */
		unsigned int use16	: 1;	/* write 16-bit pointer */
		unsigned int nomerge : 1;	/* don't merge */
		unsigned int extend	: 1;	/* Extension block follows */
		unsigned int asmptrs    : 1;    /* text uses pointers embedded in ASM */
		unsigned int asmptrs3   : 1;    /* text uses pointers embedded in ASM (type 3) */
		unsigned int noptr      : 1;    /* text has no pointers */
		unsigned int blocked    : 1;    /* text is blocked (in block format ) */
		unsigned int reserved: 12;	/* Reserved */
	} flag;
	unsigned long address;			/* address of relocation */
	unsigned short size;				/* size of block */
} SIF_BLOCK_HEADER;

#define SIF_BLOCK_HEADER_SIZE 10

typedef struct SIF_SECTION_HEADER
{
   unsigned short scn;
   unsigned short offset;
} SIF_SECTION_HEADER;

/* Sif Writer struct */
struct SifWriter;
typedef struct SifWriter SifWriter;

#define SIF_USE16    0x1000
#define SIF_NOMERGE  0x2000
#define SIF_ASM      0x4000
#define SIF_NOPTR    0x8000
#define SIF_ASM3     0x10000
#define SIF_BLOCK    0x20000
#define SIF_LOCAL    0x800

SifWriter *CreateSifWriter(const char *filename,const char *sections[],const char *dlmap);
int WriteSifBlock(SifWriter *w,void *block,unsigned int size,unsigned long addr,const char *section,int map,unsigned long flags);
int CloseSifWriter(SifWriter *w);

#endif
