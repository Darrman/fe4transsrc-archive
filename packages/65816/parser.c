/***     65816 Cross Assembler v2.0 (c)1994 Jeremy Gordon     ***/


#include <stdio.h>
#include <string.h>
#include <ctype.h>
#ifndef AMIGA
#include <malloc.h>
#endif

#include "machine.h"
#include "parser.h"
#include "opcode.h"

char *upper(char *str)
{
	int count;
	char *s;
	if(str!=NULL) {
	  s=(char *)malloc(strlen(str)+1);
	  if(!s) memory_error();
	  for(count=0;count<strlen(str);count++)
	    s[count]=toupper(str[count]);
	  s[strlen(str)]='\0';
	  return(s);
	}
	return(NULL);
}
char *strdupit(char *str)
{
	char *t;
	if(str==NULL) {
	  t=(char*)strdup("");
	  if(!t) memory_error();
	  return(t);
	}
	else {
	  t=(char*)strdup(str);
	  if(!t) memory_error();
	  return(t);
	}
}
options *parserCommandLine(int argc,char **argv)
{
	int count;
	options *opts=(options *)malloc(sizeof(options));
	if(!opts) memory_error();

	printf("65816 Cross Assembler v2.0 (c)1994 Jeremy Gordon\n");
	printf("please send bugfixes to jgordon@animator.slip.umd.edu\n");
#ifdef AMIGA
	printf("Amiga compilation by JackRipper/SBT <vic@escape.com>\n");
#endif
        printf("for help, use option -h\n");

	opts->debugMode=0;
	opts->source=NULL;
	opts->output=NULL;
	opts->labelFile=0;
	opts->listFile=0;
	opts->hiRom=0;
	opts->origin=0x8000;
	memset(&opts->header,0,sizeof(opts->header));

	count=1;
	while(count<argc) {
	  if( (strcmp(argv[count],"-debug")==0) ||
	      (strcmp(argv[count],"-d")==0) ) {
		opts->debugMode=1;
		printf("debug mode enabled.\n");	
		count++;
	  }
	  else if( (strcmp(argv[count],"-symbol")==0) ||
		   (strcmp(argv[count],"-s")==0) ) {
		opts->labelFile=1;
		printf("symbol table output enabled.\n");
		count++;
	  }
	  else if( (strcmp(argv[count],"-list")==0) ||
		   (strcmp(argv[count],"-l")==0)) {
		opts->listFile=1;
		printf("list file output enables.\n");
		count++;
	  }
	  else if( (strcmp(argv[count],"-hirom")==0) ||
		   (strcmp(argv[count],"-hi")==0) ) {
		opts->hiRom=1;
		printf("assembling with hirom addressing\n");
		count++;
	  }
	  else if( (strcmp(argv[count],"-o")==0) ) {
		opts->output=(char *)strdup(argv[count+1]);
		if(!opts->output) memory_error();
		count++;
	  }
	  else if( (strcmp(argv[count],"-c")==0) ) {
		opts->supress=1;
		printf("supressing linking phase\n");
		count++;
	  }
	  else if( (strcmp(argv[count],"-help")==0) ||
		   (strcmp(argv[count],"-h")==0) ) {
		printf("use:\n\t%s [options] <source file>\n\n",argv[0]);
		printf("options are:\n");
		printf("\t-symbol or -s enables symbol table output\n");
		printf("\t-list or -l enables list file output\n");
		printf("\t-c only generate object file (no smc)\n");
/*		printf("\t-o <fname> specifies output filename\n"); */
		printf("\t-hirom or -hi enables hirom addressing (broken)\n");
		printf("\t-debug or -d enables debug messages\n");
		printf("\t-help or -h lists this message\n");
		return(NULL);
	  }
	  else if(argv[count][0]!='-') {
		if(!opts->source) {
		  opts->source=(char *)strdup(argv[count]);
		  if(!opts->source) memory_error();
		  printf("using source file '%s'.\n",opts->source);
		}
		else {
		  printf("error: already using source file '%s'\n",opts->source);
		  return(NULL);
		}
		count++;
	  }
	  else {
		printf("unknown option: %s\n",argv[count]);
		count++;
	  }
	}	
	if(opts->source==NULL) {
		printf("error: no source file specified\n");
		return(NULL);
	}
	opts->listFileName=(char *)malloc(strlen(opts->source)+5);
	if(!opts->listFileName) memory_error();
	opts->labelFileName=(char *)malloc(strlen(opts->source)+5);
	if(!opts->labelFileName) memory_error();
	opts->intermediateFileName=(char *)malloc(strlen(opts->source)+5);
	if(!opts->intermediateFileName) memory_error();
	opts->smc=(char *)malloc(strlen(opts->source)+5);
	if(!opts->smc) memory_error();
        strcpy(opts->labelFileName,opts->source);
	if(strrchr(opts->source,'.')!=NULL)
        opts->labelFileName[((int)strrchr(opts->source,'.'))
                                -((int)opts->source)]='\0';

	if(!opts->output) {
          opts->output=(char *)malloc(strlen(opts->source)+5);
	  strcpy(opts->output,opts->labelFileName);
	  strcat(opts->output,".b");
	}

	strcpy(opts->smc,opts->labelFileName);
	strcpy(opts->listFileName,opts->labelFileName);
	strcpy(opts->intermediateFileName,opts->labelFileName);
	strcat(opts->intermediateFileName,".e");
	strcat(opts->labelFileName,".s");
	strcat(opts->listFileName,".l");
	strcat(opts->smc,".smc");
	if(opts->listFile)
          printf("using list file '%s'\n",opts->listFileName);

	if(opts->labelFile) {
	   	printf("using symbol table file '%s'\n",opts->labelFileName);
	}
        printf("using output file '%s'\n",opts->output);

	if(!opts->supress)
	  printf("using smc output file '%s'\n",opts->smc);
	return(opts);
}
line *parserReadline(FILE *fp,int *lineNumber)
{
	char *token,*buf=(char*)malloc(255),*comment,*temp;
	line *theLine=(line *)malloc(sizeof(line));
	if(!buf) memory_error();
	if(!theLine) memory_error();

	theLine->opcode=NULL;

	if(fgets(buf,255,fp)!=NULL) {
	  comment=strchr(buf,';');
	  if(comment) {
		comment[0]='\0';
		comment++;
	  }
	if((theLine->comment=strdupit(strtok(comment,"\n\r")))==NULL)
         	theLine->comment=strdupit(comment);
	token=strdupit(strtok(buf,DELIMETERS));
	if(strcmp(token,"")!=0) {
	  if(!opcode(upper(token))) {
	        theLine->label=upper(token);
		theLine->opcode=upper(strdupit(strtok(NULL,DELIMETERS)));
		if(strlen(theLine->label)>MAXLABELLEN)
		  theLine->label[MAXLABELLEN]='\0';
	  }
	  else  {
	   	theLine->label=(char *)strdup("");
		if(!theLine->label) memory_error();
	  	theLine->opcode=upper(token);
	  }
	  free(token);
	  if(theLine->opcode)
	    if(!((theLine->opcode[0]=='D')&&(theLine->opcode[1]=='C'))) {
	      theLine->first=strdupit(strtok(NULL,DELIMETERS));
	      theLine->second=upper(strtok(NULL,DELIMETERS));
	    }
	    else {
	      theLine->second=(char *)strdup("");
	      if(!theLine->second) memory_error();
	      theLine->first=(char *)strdupit(strtok(NULL,"\n\r"));
	    }
	}
	else {
	  free(buf);
	  *lineNumber=*lineNumber+1;
	  return(NULL);
	 }
	theLine->linenum=*lineNumber;
	*lineNumber=*lineNumber+1; 
	}
	else
	  return(NULL);
	return(theLine);
}
char *parserPrintline(line *theLine,address LC,int flag,address LC2)
{
	char *buf=(char *)malloc(255);
	if(!buf) memory_error();
	sprintf(buf,"%s %s %s %-6d %s %-5s %-20s %-10s\n",dectohex(LC,6),
			dectohex(LC2,6),dectohex(theLine->mode,2),
			theLine->linenum,theLine->label,theLine->opcode,theLine->first,theLine->second);
	if(flag)
	 printf("%s",buf);
	return(buf);
}
void optionsCleanUp(options *opts)
{
	if(opts->source)
	  free(opts->source);
	if(opts->labelFileName)
	  free(opts->labelFileName);
	if(opts->listFileName)
	  free(opts->listFileName);
	if(opts->output)
	  free(opts->output);
	if(opts->intermediateFileName)
	  free(opts->intermediateFileName);
	if(opts->smc)
	  free(opts->smc);
}
void lineCleanUp(line *theLine)
{
	if(theLine){
	if(theLine->comment)
	  free(theLine->comment);
	if(theLine->label)
	  free(theLine->label);
	if(theLine->opcode)
	  free(theLine->opcode);
	if(theLine->first)
	  free(theLine->first);
	if(theLine->second)
	  free(theLine->second);	
	free(theLine);
	}
}
