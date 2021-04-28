/***     65816 Cross Assembler v2.0 (c)1994 Jeremy Gordon     ***/


#include <stdio.h>
#include <string.h>
#include <ctype.h>
#ifndef AMIGA
#include <malloc.h>
#endif

#include "evaluate.h"
#include "label.h"

int evaluateType(char *str)
{
	char previous=' ';
	int subcount=0,start,type,error;
	int count=0,length=strlen(str);
	char result[255]; 

	if(length) {
	while(count<length)
	  if((validChar(str[count])||str[count]=='_')&&(previous!=':')) {
		while((count<length)&&(validChar(str[count])||
			(isdigit(str[count])||str[count]=='_')))
				count++;	
		strcat(result,"65537");
		subcount+=5;
	  }	
	  else {
	    previous=str[count];
	    result[subcount]=str[count];
	    subcount++; count++;	
	    result[subcount]='\0';
	  }
	evaluate(result,&error,&type);
	if(error) {
	  return(-1);
	}
	else if(type==0xffffff)
	  return(2);
	else if(type==0xffff)
	  return(1);
	else if(type==0xff)
	  return(0);
	}
	else return(0);
}
address evaluate(char *str,int *error,address *type)
{
	stack *st=NULL;
	char *post;
	int current=0,end,start;
	opstruct op1,op2,op3;
	char temp[100],special;

	*error=0;
	
	if((post=toPostfix(str))!=NULL) {
	  end=strlen(post);
	  while(current<end) {
		start=current;
		while((post[current]>='0')&&(post[current]<='9')) 
		 current++;
		if(current!=start) {
		  strncpy(temp,(post+start),current-start);
		  *(temp+current-start)='\0';
		  op3.result=atoi(temp);
		  if(op3.result>0xffff)
		   op3.flag=0xffffffff;
		  else if(op3.result<=0xff) 
		   op3.flag=0xff;
		  else
		   op3.flag=0xffff;
		  push(&st,op3);
		}
		else if((((post[current]=='B')||
			(post[current]=='W'))||
			(post[current]=='L'))) {
                     op3.special=post[current];
		     current++;
		     push(&st,op3);
		}
		else if(post[current]!=' ') {
		  if(post[current+1]!='u')
		    op2=pop(&st);
		  op1=pop(&st);
		  if(op1.flag<op2.flag)
			op3.flag=op2.flag;
		  else
			op3.flag=op1.flag;
		  switch(post[current]) {
		    case '+':
			op3.result=op1.result+op2.result;
			break;
		    case '-':
	               if(post[current+1]=='u') {
                          op3.result=(op1.result^op1.flag)+1;
			  op3.flag=op1.flag;
                          current++;
                        }
			else
			  op3.result=op1.result-op2.result;
			break;
		    case '*':
			op3.result=op1.result*op2.result;
			break;
		    case '/':
			if(op2.result!=0)
			  op3.result=(op1.result-(op1.result%op2.result))/op2.result;
			else {
			  *error=1;
			   printf("\nerror: divide by zero ");
			   free(post);
			   return(0);
			} 
			break;
		    case '>':
			if(post[current+1]=='>') {
			  op3.result=op1.result>>op2.result;
			  current++;
			}
			else
			  op3.result=(op1.result>op2.result);
			break;
		    case '<':
			if(post[current+1]=='<') {
			  op3.result=op1.result<<op2.result;
			  current++;
			}
			else
			  op3.result=(op1.result<op2.result);
			break;
		    case '=':
			if(post[current+1]=='=') {
			  op3.result=(op1.result==op2.result);
			  current++;
			}
			else {
			  *error=1;	
		 	  printf("\nerror: invalid expression '%s' ",str);
			  free(post);
			  return(0);
			}
			break;
		    case '%':
			op3.result=(op1.result%op2.result);
			break;
		    case '^':
			op3.result=(op1.result^op2.result);
			break;
		    case '!':
			if(post[current+1]!='u'){
			  *error=1;
                          printf("\nerror: invalid expression '%s' ",str);
			  free(post);
			  return(0);
			}
			op3.result=(op1.result^op1.flag);
			break;
		    case '&':
			if(post[current+1]=='&') {
			 op3.result=(op1.result&&op2.result);
			 current++;
			}
			else
			  op3.result=(op1.result&op2.result);
			break;
		    case '|':
			if(post[current+1]=='|') {
			  op3.result=(op1.result||op2.result);
			  current++;
			}
			else
			  op3.result=(op1.result|op2.result);
			break;
		    case ':':
			if(op2.special=='B')
			  op3.flag=0xff;
			else if(op2.special=='W')
			  op3.flag=0xffff;
			else if(op2.special=='L')
			  op3.flag=0xffffff;
			else {
			  *error=1;
                          printf("\nerror: invalid expression '%s' ",str);
			  free(post);
			  return(0);
			}
			op3.result=op1.result&op3.flag;
			break; 
		    default:
			break;
		  }
		  op3.result=(op3.result&op3.flag);
		  push(&st,op3);
		  current++;
		}	
		else
		  current++;
	  }
	  op3=pop(&st);
	  free(post);
	  *type=(address)op3.flag;
	  return(op3.result&op3.flag);
	}
	else {
	  *error=1;
          printf("\nerror: invalid expression '%s' ",str);
	  free(post);
	  return(0);
	}
}
char *toPostfix(char *str)
{
	int current=0,oldcurrent;
	int binary=1;
	int end=strlen(str)-1,temp3;
	char last[255],temp[255],*result,junk[255],temp2[255];
	result=(char *)malloc(255);
	result[0]='\0';

	if(str[current]!='{') {
           if(append(result,scanForNumber(str,&current,junk))==NULL)
		binary=0;	
	   else
		binary=1;
	}
	while(current<=end) {
	  *temp='\0';
	  if(str[current]!='{')
	  if(scanForOperator(str,&current,temp)==NULL) {
		free(result);
		return(NULL);
	  }
	  if(str[current]=='{') {
		oldcurrent=current;
		temp3=scanForCloseParen(str,&current);
		if(temp3==-1) {
		  free(result);
		  return(NULL);
		}
		strcpy(temp2,(char *)(str+oldcurrent+1));
		*(temp2+(temp3-oldcurrent))='\0';
		strcpy(temp2,toPostfix(temp2));
	  }
	  else {
	    if(scanForNumber(str,&current,temp2)==NULL) {
		free(result);
		return(NULL);
	    }
	  }
	  if(strcmp(result,"")!=0)
	   strcpy(last,deleteLast(result,junk));
	  else
	   strcpy(last,"");
	  if(!binary) {
		append(result,last);
		strcat(temp,"u");
                append(result,temp2);
		append(result,temp);
	  }
	  else if(priority(last)<=priority(temp)) {
		append(result,last);
                append(result,temp2);
		append(result,temp);
	  }
	  else {
	    append(result,temp2);
	    append(result,temp);
	    append(result,last);
	  }
	  binary=1;
	 }
	return(result);
}
char *scanForNumber(char *str,int *current,char *junk)
{
	int pos=0;
	while((((str[*current]>='0')&&(str[*current]<='9')) ||
		((str[*current]=='B')||(str[*current]=='W')))||
		(str[*current]=='L')) {
	  junk[pos]=str[*current];
	  *current=*current+1;
	  pos++;
	}
	if(pos==0)
	  return(NULL);
	junk[pos]='\0';
	return(junk);
}
char *scanForOperator(char *str,int *current,char *junk)
{
	int count=0;
	junk[1]='\0';
	junk[2]='\0';
	switch(str[*current]) {
		case '+':
		  break;
		case '-':
		  break;
		case '/':
		  break;
		case '*':
		  break;
		case '^':
		  break;
		case '!':
		  break;
		case '%':
		  break;
	        case '&':
		  if(str[*current+1]=='&') {
			junk[count]='&';
			count++;
			*current=*current+1;
		  }
		  break;
		case '|':
                  if(str[*current+1]=='|') {
                        junk[count]='|';
                        count++;
                        *current=*current+1;
                  }
		  break;
		case ':':
		  break;
		case '=':
                  if(str[*current+1]=='=') {
                        junk[count]='=';
                        count++;
                        *current=*current+1;
	          }
		  break;
		default:
		 return(NULL);
	}
	junk[count]=str[*current];
	*current=*current+1;
	return(junk);
}
char *append(char *s,char *d)
{
	if(d!=NULL) {
	  strcat(s," ");
	  strcat(s,d);
	}
	else
	  return(NULL);
	return(s);
}
char *deleteLast(char *s,char *junk)
{
	int pos=strlen(s);
	if(strlen(s)) {
  	 while(s[pos]!=' ')
	   pos--;
	 strcpy(junk,s+pos+1);
	 s[pos]='\0';
	 return(junk);
	}
	else
	 return(NULL);
}
int priority(char *str)
{
	if(strcmp(str,"-")==0) 
	 return(5); 
	else if(strcmp(str,"+")==0)
	 return(5);
	else if(strcmp(str,"/")==0)
	 return(4);
	else if(strcmp(str,"*")==0)
	 return(4);
	else if(strcmp(str,"^")==0)
	 return(3);
	else if(strcmp(str,"%")==0)
	 return(3);
	else if(strcmp(str,">>")==0)
	 return(3);
	else if(strcmp(str,"<<")==0)
	 return(3);
	else if(strcmp(str,"==")==0)
	 return(3);
	else if(strcmp(str,"&")==0)
	 return(3);
	else if(strcmp(str,"&&")==0)
	 return(3);
	else if(strcmp(str,"|")==0)
	 return(3);
	else if(strcmp(str,"||")==0)
	 return(3);
	else if(strcmp(str,"-u")==0)
	 return(1);
	else if(strcmp(str,"!u")==0)
	 return(1);
	else if(strcmp(str,":")==0)
	 return(1);
	return(0);
}
int scanForCloseParen(char *str, int *current)
{
	int balance=0;
	do {
	  if(*(str+*current)=='{')
		balance++;
	  else if(*(str+*current)=='}')
		balance--;
	  *current=*current+1;	
	} while((balance!=0)&&(*current<strlen(str)));
	if (balance!=0)
	  return(-1);
	else
	  return(*current-2);
}
opstruct pop(stack **st)
{
	opstruct result;
	stack *temp=*st;
	if(*st!=NULL) {
	  *st=(stack *)(*st)->last;
	  result=temp->val;
	  free(temp);
	}
	return(result);
}
void push(stack **st,opstruct val)
{
	stack *newone=(stack *)malloc(sizeof(stack));
	newone->val=val;
	newone->last=(void *)*st;
	*st=newone;
}
