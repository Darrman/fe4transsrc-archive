/*
	tbintern.h
	Definitions used internally by TBLLib
	Users should not include this file
*/                               
#ifndef __TBINTERN_H
#define __TBINTERN_H

typedef char int8;
typedef unsigned char uint8;
typedef short int16;
typedef unsigned short uint16;
typedef long int32;
typedef unsigned long uint32;
typedef int boolean;

typedef struct TBLENTRY
{
	uint32 value;
	char *string;
	int size;
} TBLENTRY;

typedef struct __tag_TBLINFO
{
	TBLENTRY *entry;
	TBLENTRY **hv;
	TBLENTRY **hs;
	uint16 nEntries;
	unsigned int nMaxCollisionsV;
	unsigned int nMaxCollisionsS;
} __tag_TBLINFO;

extern unsigned int hashsize;
int TBLAPICALL hash(uint32 value);
int TBLAPICALL hashs(const char *string);
int TBLAPICALL hinc(uint32 value);
int TBLAPICALL hincs(const char *string);

char *strdup(const char *s);

#endif
