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
/* $Id: parse.c,v 1.1 2001/06/14 21:46:05 j10 Exp $ */

#include <stdio.h>
#include <ctype.h>
#include "parse.h"

static char tokbuf[256];
static const char *tokptr = NULL;

char *getNext(const char *token)
{
   int i;

   if(token) tokptr = token;

   /* skip whitespace */
   while(isspace(*tokptr) && *tokptr) tokptr++;
   if(!*tokptr) return NULL;

   /* if is alpha */
   if(isalpha(*tokptr) || *tokptr == '.') {
      for(i = 0; (isalpha(*tokptr) || isdigit(*tokptr) || *tokptr == '.') && *tokptr; i++) {
         tokbuf[i] = *tokptr;
         tokptr++;
      }
      tokbuf[i] = 0;
      return tokbuf;
   } else if(isdigit(*tokptr)) {
      for(i = 0; isxdigit(*tokptr) && *tokptr; i++) {
         tokbuf[i] = *tokptr;
         tokptr++;
      }
      tokbuf[i] = 0;
      return tokbuf;
   } else if(ispunct(*tokptr)) {
      for(i = 0; ispunct(*tokptr) && *tokptr; i++) {
         tokbuf[i] = *tokptr;
         tokptr++;
      }
      tokbuf[i] = 0;
      return tokbuf;
   }

   return NULL;
}

