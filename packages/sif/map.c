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
/* $Id: map.c,v 1.5 2001/08/01 15:54:37 j10 Exp $ */

#include <stdlib.h>
#include "cmdline.h"
#include "map.h"

#ifdef __WIN32__
#include <windows.h>
#define dlopen(f,m) LoadLibraryA(f)
#define dlsym(h,s) (void *)GetProcAddress(h,s)
#define dlclose(h) FreeLibrary(h)
#define RTLD_LAZY
#define RTLD_NOW
#else
#include <dlfcn.h>
#endif

unsigned long pc2snes(unsigned long addr)
{
   unsigned short offset;
   unsigned char bank;

   if(opts.hirom) {
      offset = addr & 0xFFFF;
      bank = 0xC0 + ((addr >> 16) & 0xFF);
   } else {
      offset = (addr & 0xFFFF) | 0x8000;
      bank = 0x80 + ((addr >> 15) & 0xFF);
   }

   return (bank << 16) | offset;
}

Map *LoadMap(const char *filename)
{
   int (*id)();
   Map *m;

   m = (Map *)malloc(sizeof(Map));
   if(!m) goto error_0;

   m->handle = dlopen(filename,RTLD_LAZY);
   if(!m->handle) goto error_1;

   id = (int (*)())dlsym(m->handle,"id");
   if(!id) goto error_2;
   if(id() != 0x51F500) goto error_2;

   m->mapfunc = (unsigned long (*)(unsigned long,int))dlsym(m->handle,"mapfunc");
   if(!m->mapfunc) goto error_2;

   return m;
error_2:
   dlclose(m->handle);
error_1:
   free(m);
error_0:
   return NULL;
}

void FreeMap(Map *m)
{
   dlclose(m->handle);
   free(m);
}
