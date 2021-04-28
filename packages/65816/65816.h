#ifndef __65816_
#define __65816_

#include <stdio.h>
#include "parser.h"

int	firstPass(options *,char *,address *);
int 	doDirectives(options *,line *,char *,address *);
int	secondPass(options *,char *,address *,FILE *);
void	globalCleanUp(void);
void incrLC(address *,address);


#endif
