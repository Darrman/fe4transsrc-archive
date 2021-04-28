/* $Id: fe4enc.h,v 1.1 2001/07/02 05:28:26 j10 Exp $ */
#ifndef __fe4c_h
#define __fe4c_h

#define WINDOW_SIZE 16384
#define WINDOW_MASK (WINDOW_SIZE - 1)
#define HASH_SIZE   4096
#define HASH_MASK   (HASH_SIZE - 1)

#define MIN_MATCH   2
#define MAX_MATCH  49

extern long hash[HASH_SIZE];
extern long prev[WINDOW_SIZE];
extern unsigned long fpos;

#define HASH(key1,key2)                                  \
   prev[fpos & WINDOW_MASK] = hash[(key2 << 5) ^ key1],  \
   hash[(key2 << 5) ^ key1] = fpos;

typedef struct LZ77Info
{
   unsigned short distance;
   unsigned char  length;
} LZ77Info;

extern void lz77_setup();
extern int FindMatch(LZ77Info *bestmatch,register unsigned char *readahead);

extern int rle_analyse(unsigned char *readahead,int len);
extern int dup2x_analyse(unsigned char *readahead,int len);
extern int preappend_analyse(unsigned char *readahead,int len);
extern int postappend_analyse(unsigned char *readahead,int len);
extern int lz77_analyze(LZ77Info *info,unsigned char *readahead,int level,int len);

extern long filelength(FILE *fp);

extern unsigned char *file;

#endif
