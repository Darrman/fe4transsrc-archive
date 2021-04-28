#ifndef __label_
#define __label_

#include "machine.h"

#define NORMAL 1
#define MTDF 1

typedef struct {
	char *label;
	address value;	
	int type;
	void *next;
	} labelListType;

int	labelOpenFile(char *);
void	labelCloseFile(void);
int	labelDefined(char *,address *);
void	labelAdd(char *,address);
void	labelCleanUp(void);
char 	*labelReplace(char *);
int 	validChar(char);
int 	labelValid(char *);	

#endif
