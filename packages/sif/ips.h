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
/* $Id: ips.h,v 1.1 2001/06/27 03:55:13 j10 Exp $ */
#ifndef __ips_h
#define __ips_h

typedef struct Ips
{
   int mode;
   FILE *fp;
} Ips;

#define IPS_READ  0
#define IPS_WRITE 1

Ips *ips_open(const char *filename,int mode);
void ips_close(Ips *ips);
int ips_read(void *buffer,unsigned long *addr,unsigned int *size,Ips *ips);
int ips_write(void *buffer,unsigned long addr,unsigned int size,Ips *ips);

#endif

