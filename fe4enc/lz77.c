/*
   $Id: lz77.c,v 1.1 2001/07/02 05:28:26 j10 Exp $
   LZ77 routines
*/
#include <stdio.h>
#include <string.h>
#include "fe4enc.h"

long hash[HASH_SIZE];   /* hash table */
long prev[WINDOW_SIZE]; /* links to previous matches */

typedef struct LZ77ConfigInfo
{
   short goodlength;
   short nicelength;
   short maxchain;
} LZ77ConfigInfo;

struct LZ77ConfigInfo configtab[] =
{
/*    good  nice  chain */
/*0*/{4,     8,     4   },
/*1*/{4,    16,     8   },
/*2*/{4,    32,     16  },
/*3*/{8,    128,    256 },
/*4*/{32,   258,    1024},
/*5*/{32,   258,    4906}
};

static short goodlength = 0;
static short nicelength = 0;
static short maxchain = 0;

static short maxmatch = MAX_MATCH;

void lz77_setup()
{
   memset(hash,-1,HASH_SIZE*sizeof(long));
   memset(prev,-1,WINDOW_SIZE*sizeof(long));
}

/* lz77_compress:
 * Returns a distance and length from scanning the buffer for a match shorto the
 * LZ77Info struct. The return value is the number of bytes generated.
 */
int lz77_analyze(LZ77Info *info,unsigned char *readahead,int level,int len)
{
   if(level < 0 || level > 5) {
      fprintf(stderr,"Warning: Compression level must be between 0 and 5. Using defaults.\n");
      level = 3;
   }

   memset(info,0,sizeof(LZ77Info));

   /* Grab config info */
   goodlength = configtab[level].goodlength;
   nicelength = configtab[level].nicelength;
   maxchain   = configtab[level].maxchain;

   if(len < maxmatch) maxmatch = (short)len;

   if(prev[fpos] != -1) {
      FindMatch(info,readahead);
      return info->length;
   }

   return 0;
}

int FindMatch(LZ77Info *bestmatch,register unsigned char *readahead)
{
   long curmatch = prev[fpos];
   register unsigned char *match;
   short chainlen = maxchain;
   short len;

	bestmatch->length = MIN_MATCH-1;
	do {
		match = &file[curmatch];
		if(*match != *readahead ||
			*(match + bestmatch->length) != *(readahead + bestmatch->length)) continue;

		for(len = MIN_MATCH-1;len < maxmatch;len++)
			if(match[len] != readahead[len]) break;

		if(len > bestmatch->length) {
         if(len > goodlength) chainlen >>= 1;

			bestmatch->length = len;
			bestmatch->distance = (unsigned short)(fpos - curmatch);

			if(len >= nicelength) break;
		}
	} while((curmatch = prev[curmatch & WINDOW_MASK]) > -1 && --chainlen);

	return bestmatch->length;
}
