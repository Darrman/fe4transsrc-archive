/***     65816 Cross Assembler v2.0 (c)1994 Jeremy Gordon     ***/


#include <stdio.h>
#include <string.h>
#include <ctype.h>
#ifndef AMIGA
#include <malloc.h>
#endif

#include "machine.h"
#include "label.h"

static labelListType *labelList=NULL;
static int labelFileOpen=0;
static FILE *theLabelFile;

int labelOpenFile(char *name)
{
	if((theLabelFile=fopen(name,"w"))==NULL) {
	  printf("error: can't open symbol log file '%s'\n",name);
	  return(1);
	}
	labelFileOpen=1;
	return(0);
}
void labelCloseFile(void)
{
	if(labelFileOpen)
          fclose(theLabelFile);
}
int labelDefined(char *name,address *value)
{
	labelListType *temp=labelList;
	while(temp) {
	  if(strcmp((char *)((labelListType *)temp->label),name)==0) {
		*value=(address)temp->value;
		return(1);
	  }	
	  temp=(labelListType*)temp->next;
	}
	return(0);
}
void labelAdd(char *name, address val)
{
	char *buf=(char *)malloc(30),*t;
	labelListType *trail,*temp=labelList;
	labelListType *newnode=(labelListType *)malloc(sizeof(labelListType));
        if(!buf) memory_error();
	if(!newnode) memory_error();

        newnode->label=(char *)strdup(name);
	if(!newnode->label) memory_error();
        newnode->value=val;
        newnode->next=NULL;

	if(labelFileOpen) {
          sprintf(buf,"%-10s=$%s\n",newnode->label,(t=dectohex(newnode->value,6)));
	  free(t);
	  if(name[0]!='-')
	   fputs(buf,theLabelFile);
	}

	if(temp) {
	  while(temp) {
	   trail=temp;
	   temp=(labelListType *)temp->next; 
	  }
	  trail->next=newnode;
	}
	else
	  labelList=newnode;
	free(buf);
}
void labelDelete(char *name)
{
	int found=1;
	labelListType *temp=labelList,*trail=NULL;
	if(temp) {
	  while((((found=strcmp(temp->label,name))!=0)&&temp->next)&&found) {
	   trail=temp;		
	   temp=(labelListType *)temp->next;
	  }
	  if(found==0) {
	    if(trail) 
	      trail->next=temp->next;
	    else
	      labelList=temp->next;
	    free(temp);
	  }
	}
}
#define OPERATORS "!#$%^&*(){}[]+=/|<>-"
char *labelReplace(char *what)
{
	int ok;
	int end=strlen(what),count=0,start;
	char *newLine=(char*)malloc(255),temp[255];
	char *temp2;
	char newone[2]=" ";
	address value;
	if(!newLine) memory_error();
	newLine[0]='\0';
	
	do {
	  if( validChar(what[count])||(what[count]=='_')||(what[count]=='-')) {
	   temp2=(char *)strdup(what);
	   start=count;
	   if(what[count]=='-') {
	     count++;
	     while((count<end)&&(what[count]=='-'))
	       count++;
	     temp2[count]='\0';  
	     ok=1; 
	     if((strchr("*&^%+<>|/",what[count])==NULL)&&(what[count]!='\0')) {
	       if(count>(start+1)) {
		temp2[strlen(temp2)-1]='\0';
		count--;	
		ok=1;
		}
		else  {
		 count--;
		 ok=0;
		}
	    }
	    if(ok)  
	     if(labelDefined((char *)(temp2+start),&value)) {
		sprintf(temp,"%u",value);
		strcat(newLine,temp);
                labelDelete((char *)(temp2+start));
		free(temp2);
	     }
	     else {
                printf("\nerror: label '%s' not defined ",(char *)(temp2+start));
                free(temp2);
                free(newLine);
                return(NULL);   
             }
	    }
	   else {
	   while((count<end)&&(strchr(OPERATORS,what[count])==NULL))
		count++;	
	   temp2[count]='\0';
	   if(labelDefined((char *)(temp2+start),&value)) {
	     sprintf(temp,"%u",value); 
	     strcat(newLine,temp);
	     free(temp2);
	   }
	   else if((strcmp((char *)(temp2+start),"B")==0)||
		   (strcmp((char *)(temp2+start),"W")==0)||
		   (strcmp((char *)(temp2+start),"L")==0)) {
	     strcat(newLine,(char *)(temp2+start));
	     free(temp2);
	   }
	   else {
		printf("\nerror: label '%s' not defined ",(char *)(temp2+start));
		free(temp2);
		free(newLine);
		return(NULL);	
	   }
	 }
	  }
	  newone[0]=what[count];
	  strcat(newLine,newone);
	  count++;
	} while (count<end);
	return(newLine);
}
int validChar(char ch)
{
	if(((ch>='a')&&(ch<='z'))||
	   ((ch>='A')&&(ch<='Z')))
		return(1);
	return(0);
}
int labelValid(char *str)
{
	int count=1,length=strlen(str);
	if((str[0]=='_')||validChar(str[0]))
	  while((count<length)&&( (validChar(str[count])||(str[count]=='_'))||
			isdigit(str[count])) )
		count++;
	  if(count>=length)
		return(1);
	return(0);
}
void labelCleanUp(void)
{
	labelListType *trail,*temp=labelList;
	while(temp) {
	  trail=temp;
	  free(temp->label);
	  temp=(labelListType*)temp->next;
	  free(trail);
	}
}
