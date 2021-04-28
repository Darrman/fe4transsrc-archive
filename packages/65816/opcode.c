/***     65816 Cross Assembler v2.0 (c)1994 Jeremy Gordon     ***/


#include <string.h>
#include "opcode.h"
#include "opcodes.h"

int opcode(char *str)
{
	int loc=0,temp;
	while((loc<OPCODEMAX)&&((temp=strcmp(str,opcodes[loc]))!=0) )
		loc++;
	if(temp==0)
		return(1);
	else
		return(0);
}
