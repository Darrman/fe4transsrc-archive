#ifndef __parser_
#define __parser_

#include <stdio.h>
#include "smc.h"

#define DELIMETERS ",; \t\n\r"
#define MAXLABELLEN 15

typedef struct {
	address addr;
	address addrend;
	char *comment;
	char *label;
	char *opcode;
	char *first;
	char *second;
	int linenum;
	byte mode;
	} line;

typedef struct {
	char *source;
	char *labelFileName;
	char *listFileName;
	char *intermediateFileName;
	char *output;
	char *smc;
	smcHeader header;
	address origin;

	int labelFile;
	int listFile;
	int debugMode;
	int hiRom;
	int supress;
	} options;


char 	*upper(char *);    /* make sure to free returned string when done */
char 	*strdupit(char *); /* make sure to free returned string when done */
options	*parserCommandLine(int,char **);  /* must free */
line	*parserReadline(FILE *,int *); 	  /* must free */
char 	*parserPrintline(line *,address,int,address);    /* must free */
void	optionsCleanUp(options *);
void	lineCleanUp(line *);

#endif
