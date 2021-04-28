/***     65816 Cross Assembler v2.0 (c)1994 Jeremy Gordon     ***/


#include <stdio.h>
#include <string.h>

#include "machine.h"
#include "list.h"

static FILE *theListFile;

int listOpenFile(char *name)
{
	if((theListFile=fopen(name,"w"))==NULL) {
	  printf("error: can't open list file '%s'\n",name);
	  return(1);
	}
	return(0);
}
void listCloseFile(void)
{
	fclose(theListFile);
}
int listPutLine(char *buf)
{
	if(fputs(buf,theListFile)<0) {
	  printf("error: can't write to list file!\n");
	  return(1);
	}
	return(0);
}
void listCleanUp(void)
{
}
