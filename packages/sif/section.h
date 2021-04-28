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
/* $Id: section.h,v 1.4 2001/06/28 01:43:55 j10 Exp $ */

#ifndef __section_h
#define __section_h

#define SECTION_PUBLIC     0
#define SECTION_RESERVED   1
#define SECTION_DIRECT     2
#define SECTION_UNKNOWN    3

typedef struct Section
{
  int numscn;
  char *sectionlist[64];
  unsigned long common[64][2];
} Section;

#define setcommon(s,x,y) (s->common[(x)][((y) >> 5) & 1] |= (1 << ((y) & 31)))
#define clearcommon(s,x,y) (s->common[(x)][((y) >> 5) & 1] &= ~(1 << ((y) & 31)))
#define iscommon(s,x,y) (s->common[(x)][((y) >> 5) & 1] & (1 << ((y) & 31)))

Section *CreateSection();
void FreeSection(Section *s);
int ChangeSection(Section *s,const char *name);
unsigned char *AllocateSectionMap();
char *strdup(const char *str);

#endif

