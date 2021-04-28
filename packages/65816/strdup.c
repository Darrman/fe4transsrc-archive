/***     65816 Cross Assembler v2.0 (c)1994 Jeremy Gordon     ***/


#include <string.h>
#include "machine.h"

char *strdup(char *s)
{
	char *p=(char *)malloc(strlen(s)+1);
	if(!p) memory_error();
	strcpy(p,s);
	return(p);
}
