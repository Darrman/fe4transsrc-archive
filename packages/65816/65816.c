/***     65816 Cross Assembler v2.0 (c)1994 Jeremy Gordon	***/


#include <stdio.h>
#ifndef AMIGA
#include <malloc.h>
#endif
#include <stdlib.h>
#include <string.h>

#include "machine.h"
#include "65816.h"
#include "parser.h"
#include "label.h"
#include "list.h"
#include "directvs.h"
#include "address.h"
#include "evaluate.h"
#include "smc.h"

static int HiRom;

main(int argc, char **argv)
{

	FILE *out;
	address LC=0x8000;
        options *opts=(options *)malloc(sizeof(options));
	if(!opts) memory_error();

	if((opts=parserCommandLine(argc,argv))==NULL)
	  return(0);

	if(listOpenFile(opts->intermediateFileName))
	  return(0);

        if(opts->labelFile)
          if(labelOpenFile(opts->labelFileName))
            return(0);

	setHiRom(opts->hiRom);
	HiRom=opts->hiRom;

	printf("[entering first pass]\n");
        printf("building symbol table...");
        if(opts->debugMode)
          printf("\n");
	if(firstPass(opts,opts->source,&LC)) {
	  printf("interrupted.\n");
	  return(0);
	}
        printf("done.\n");
	labelCloseFile();
	listCloseFile();

	printf("[entering second pass]\n");
	printf("assembling code at $%s...",dectohex(opts->origin,6));
        if((out=fopen(opts->output,"wb"))==NULL) {
             printf("error: can't open ouput file '%s'\n",opts->output);
             return(1);
        }
	if(secondPass(opts,opts->intermediateFileName,&LC,out)) {
	  printf("interrupted.\n");
	  return(0);
	}
	fclose(out);

	unlink(opts->intermediateFileName);	

        if(!opts->supress)
         makeSMCFile(opts->output,opts->smc,opts->header,(word)(opts->origin&0xffff));

	globalCleanUp();
	optionsCleanUp(opts);
	printf("success.\n");
	return(0);
}
int firstPass(options *opts,char *fname,address *LC)
{
	FILE *fp;
	int linenumber=1;
	char aname[255]=">",*freeme;
        line *theLine=(line *)malloc(sizeof(line));
	address temp,dontcare;

	if(!theLine) memory_error();

	if((fp=fopen(fname,"r"))==NULL) {
	  printf("error: can't open source file '%s'\n",fname);
	  return(1);
	}
	strcat(aname,fname);
	strcat(aname,"\n");
	listPutLine(aname);

	while(!feof(fp)) {
         theLine=parserReadline(fp,&linenumber);     

         if(theLine!=NULL) {
	     temp=*LC;
	     if(strcmp(theLine->opcode,"")!=0)
             if(doDirectives(opts,theLine,fname,LC)) {
                fclose(fp);
                return(1);
             }
 
             if((strcmp(theLine->label,"")!=0)&&(theLine->mode!=0xfe))
               if((!labelDefined(theLine->label,&dontcare))||(theLine->label[0]=='-')) {
                   labelAdd(theLine->label,temp);    
                   if(!opts->debugMode)
                     printf(".");
                }
	     if((theLine->mode!=0xfe)&&(strcmp(theLine->opcode,"")!=0)) {
	     if(((*LC&0xff0000)-(temp&0xff0000))&&(strcmp(theLine->opcode,PAD)!=0)) 
		temp=(*LC&0xff0000+0x8000);
	     if(listPutLine(freeme=parserPrintline(theLine,temp,opts->debugMode,*LC))) {
		free(freeme);	
		fclose(fp);
		return(1);
	     }
	     free(freeme);
	   }
	   lineCleanUp(theLine);
           }
	}
	fclose(fp);
	return(0);
}
int secondPass(options *opts,char *fname,address *LC,FILE *fp)
{
	FILE *in,*fp2;
	line *theLine;
	char *buf=(char *)malloc(255),*freeme;
	address addr,last;
	int error,e;
	char currentFname[255];
	char *temp;

	if(!buf) memory_error(); 
	
	if((in=fopen(fname,"r"))==NULL) {
          printf("error: can't open intermediary file '%s'\n",fname);
	  return(1);
	}
	if(opts->listFile)
        if((fp2=fopen(opts->listFileName,"w"))==NULL) {
          printf("error: can't open list file '%s'\n",opts->listFileName);
          return(1);
        }

	while(!feof(in)) {
		fgets(buf,255,in);
		if(!feof(in)) {
		 theLine=(line *)malloc(sizeof(line));
		 if(!theLine) memory_error();
		 if(buf[0]=='>')  {
		   free(theLine);
		   strcpy(currentFname,strtok((char *)(buf+1),"\r\n")); 
		   if(opts->listFile) {
		     fputs("now in source file: '",fp2);
		     fputs(currentFname,fp2);
		     fputs("'\n",fp2);
		   }
		 }
	        else {
		 theLine->addr=hextodec((char *)strtok(buf," \n\r"));
                 theLine->addrend=hextodec((char *)strtok(NULL," \n\r"));
		 last=theLine->addrend;
		 theLine->mode=hextodec((char *)strtok(NULL," \n\r"));
		 theLine->linenum=atol((char *)strtok(NULL," \n\r"));
		 theLine->label=strdupit((char *)strtok(NULL," \n\r"));
		 if(opcode(theLine->label)) {
		  theLine->opcode=theLine->label;
		  theLine->label=NULL;
		 }
		 else
		  theLine->opcode=strdupit((char *)strtok(NULL," \n\r"));
		if(!((theLine->opcode[0]=='D')&&(theLine->opcode[1]=='C'))) {
		 theLine->first=strdupit((char *)strtok(NULL," \n\r"));
		 theLine->second=strdupit((char *)strtok(NULL," \n\r"));
		}
		else {
		 theLine->second=(char *)strdup("");
		 if(!theLine->second) memory_error();
		 theLine->first=strdupit((char *)strtok(NULL,"\n\r"));
		}
		 theLine->comment=NULL;
		if((theLine->mode!=IMPLIED)&&(theLine->mode!=DIRECTIVE)) {
           	 theLine->first=labelReplace(freeme=theLine->first);
           	 if(freeme)
                    free(freeme);
           	 if(theLine->first==NULL) {
                   printf("in line %d\n",theLine->linenum);
             	   return(1);
	       }
                 }
	        if(opts->listFile) {
                 sprintf(buf,"%-5d %s  ",theLine->linenum,dectohex(theLine->addr,6));
                 fputs(buf,fp2);
	        }

	 	 switch(theLine->mode) {
			case ABSOLUTE:
			  e=address_absolute(theLine,fp,fp2,opts->listFile);
			  break;
			case ABSOLUTEINDEXEDX:
			  e=address_absoluteindexedx(theLine,fp,fp2,opts->listFile);
			  break;
			case ABSOLUTEINDEXEDY:
			  e=address_absoluteindexedy(theLine,fp,fp2,opts->listFile);
			  break;
			case ABSOLUTEINDIRECTINDEXED:
			  e=address_absoluteindirectindexed(theLine,fp,fp2,opts->listFile);
			  break;
			case ABSOLUTEINDIRECT:
			  e=address_absoluteindirect(theLine,fp,fp2,opts->listFile);
			  break;
			case ABSOLUTEINDIRECTLONG:
			  e=address_absoluteindirectlong(theLine,fp,fp2,opts->listFile);
			  break;
			case ABSOLUTELONG:
			  e=address_absolutelong(theLine,fp,fp2,opts->listFile);
			  break;
			case ABSOLUTELONGINDEXEDX:
			  e=address_absolutelongindexedx(theLine,fp,fp2,opts->listFile);
			  break;
			case BLOCKMOVE:
			  e=address_blockmove(theLine,fp,fp2,opts->listFile);
			  break;
			case DIRECTPAGE:
			  e=address_directpage(theLine,fp,fp2,opts->listFile);
			  break;
			case DIRECTPAGEINDEXEDX:
			  e=address_directpageindexedx(theLine,fp,fp2,opts->listFile);
			  break;
			case DIRECTPAGEINDEXEDY:
			  e=address_directpageindexedy(theLine,fp,fp2,opts->listFile);
			  break;
			case DIRECTPAGEINDEXEDINDIRECTX:
			  e=address_directpageindexedindirectx(theLine,fp,fp2,opts->listFile);
			  break;
			case DIRECTPAGEINDIRECT:
			  e=address_directpageindirect(theLine,fp,fp2,opts->listFile);
			  break;
			case DIRECTPAGEINDIRECTLONG:
			  e=address_directpageindirectlong(theLine,fp,fp2,opts->listFile);
			  break;
			case DIRECTPAGEINDIRECTINDEXEDY:
			  e=address_directpageindirectindexedy(theLine,fp,fp2,opts->listFile);
			  break;
			case DIRECTPAGEINDIRECTLONGINDEXEDY:
			  e=address_directpageindirectlongindexedy(theLine,fp,fp2,opts->listFile);
			  break;
			case IMMEDIATE:
			  e=address_immediate(theLine,fp,fp2,opts->listFile);
			  break;
			case IMMEDIATEW:
			  e=address_immediatew(theLine,fp,fp2,opts->listFile);
			  break;
			case IMPLIED:
			  e=address_implied(theLine,fp,fp2,opts->listFile);
			  break;
			case STACKRELATIVE:
			  e=address_stackrelative(theLine,fp,fp2,opts->listFile);
			  break;
			case STACKRELATIVEINDIRECTINDEXEDY:
			  e=address_stackrelativeindirectindexedy(theLine,fp,fp2,opts->listFile);
			  break;
			case DIRECTIVE:
			  e=address_directive(theLine,fp,fp2,opts->listFile,opts);
			  break;
			default:
			  e=1;
			  break;
		}
		if(e) {
		 if(strcmp(opts->source,currentFname)==0)
		 printf("\nerror: syntax in line %d\n",theLine->linenum);
	         else {
		 printf("\nerror: syntax in line %d from included file '%s'\n",theLine->linenum,currentFname);
		 }
		 lineCleanUp(theLine);
		 free(theLine); 
		 fclose(in);
		 if(opts->listFile)
		  fclose(fp2);
		 return(1);
		}
		lineCleanUp(theLine);
		}
	      }
	   }
	printf("finished at $%s.\n",dectohex(last-1,6));
	free(theLine);
	fclose(in);
	if(opts->listFile)
	fclose(fp2);
	return(0);
}
int doDirectives(options *opts, line *theLine,char *fname,address *LC)
{
        FILE *fp;
        int size,type,count;
	char *freeme,*freeme2,*temp;
	char aname[255]=">";
	char temp3[255];
	address value,junk2;

	theLine->mode=0xff;
        if(strcmp(theLine->opcode,BIN)==0) {
          if(fp=fopen(theLine->first,"rb")) {
            fseek(fp,0,2);
            size=(int)ftell(fp);
            fseek(fp,0,0);
            fclose(fp);
          }
          else {
            printf("\nerror: in file included from '%s'\n",fname);
            printf("\tcan't open binary file '%s'\n",theLine->first);
            return(1);
          }
	  incrLCBIN(LC,size); 
        } 
	else if(strcmp(theLine->opcode,ORG)==0) {
	  *LC=(address)atol(convertHex(upper(theLine->first)));
	   opts->origin=*LC;
	   theLine->mode=0xfe;
	}
	else if(strcmp(theLine->opcode,EQU)==0) {
	   if(*theLine->first!='\0') {
           theLine->first=convertHex(freeme2=upper(freeme=theLine->first));     
           if(freeme)
             free(freeme);
           if(freeme2)
            free(freeme2);
	   value=evaluate(theLine->first,&size,&type);
	   if(size) {
	   	printf("\nerror: syntax in line %d\n",theLine->linenum);
		return(1);
	   }
	   freeme=NULL;
	   if((!labelDefined(theLine->label,&junk2))||(theLine->label[0]=='-'))
	   labelAdd((freeme=upper(theLine->label)),value);
	   if(freeme)
	   free(freeme);
	   theLine->mode=0xfe;
	  }
	  else {
		printf("\nerror: syntax in line %d\n",theLine->linenum);
		return(1);
	  }
	}
	else if(strcmp(theLine->opcode,PAD)==0) {
	  theLine->mode=0xff;
	  if(strcmp(theLine->first,"")==0) {
	    if(opts->hiRom)
	      *LC=(*LC&0xff0000)+0x010000;
	    else
              *LC=(*LC&0xff0000)+0x018000;
	  }
	  else  {
	    if((address)atol(convertHex(upper(theLine->first)))<(*LC&0x00ffff))
	     if(opts->hiRom)
                  *LC=(*LC&0xff0000)+0x010000;
              else {
                   if((address)atol(convertHex(upper(theLine->first)))<0x8000)
                        printf("\nerror: can't pad to lower bank memory when not in hirom mode\n");
                  *LC=(*LC&0xff0000)+0x018000;
		   return(1);
		}
	     *LC=(*LC&0xff0000)+(address)atol(convertHex(upper(theLine->first)));
	   }
	}
	else if((strcmp(theLine->opcode,DCB)==0)||
		(strcmp(theLine->opcode,DCW)==0)||
	        (strcmp(theLine->opcode,DCD)==0)) {
	     
	     temp=theLine->first; 
	     while(((*temp==' ')||(*temp=='\t'))  &&(temp!='\0'))
	        temp=temp+1;
	     if(temp!=theLine->first) {
             strncpy(temp3,temp,strlen(theLine->first)+1-(temp-theLine->first));
             temp3[strlen(theLine->first)+1-(temp-theLine->first)]='\0';
             strcpy(theLine->first,temp3);
	     }
	     count=strlen(theLine->first)-1;
	     while( ((theLine->first[count]==' ') ||
		    (theLine->first[count]=='\t')) &&
		    count>=0 ) {
		  theLine->first[count]='\0';
		  count--;
	     }
	     temp=theLine->first;
	      while(*temp!='\0') { 
		if(*temp=='"') {
		  freeme=temp+1; 
		  while((*freeme!='"')&&(*freeme!='\0'))
		    freeme++;
		  if(freeme==temp) {
		    printf("error: unterminated string constant in line %d\n",theLine->linenum);	
		    return(1);
		  }
		  incrLCBIN(LC,freeme-temp-1);
		  temp=freeme+1;
		}
		else if(*temp==' ') {
		 temp=temp+1;
		}
		else {
                  freeme=temp;
                  while((*freeme!=',')&&(*freeme!='\0'))
                    freeme++;
		  if(freeme>temp)
      	          if(strcmp(theLine->opcode,DCB)==0)
		    incrLCBIN(LC,1);
	          else if(strcmp(theLine->opcode,DCW)==0)
		    incrLCBIN(LC,2);
	          else if(strcmp(theLine->opcode,DCD)==0)
		    incrLCBIN(LC,4);
		  if(*freeme!='\0')
		  temp=freeme+1;
		  else
		  temp=freeme;
		}
	  }
	}
        else if(strcmp(theLine->opcode,INC)==0) {
	 theLine->mode=0xfe;
         if(strcmp(fname,theLine->first)==0) {
            printf("\nerror: recursive INL in file '%s'\n",fname);
            return(1);
         }
         if(firstPass(opts,theLine->first,LC))
            return(1);
	 strcat(aname,fname);
         strcat(aname,"\n");
         listPutLine(aname);
        }
	else if(strcmp(theLine->opcode,"")==0) {
	  /*  label */
	}	
	else {
	  if(*theLine->first!='\0') {
           theLine->first=convertHex(freeme2=upper(freeme=theLine->first));	
	   if(freeme)
	     free(freeme);
	   if(freeme2)
	    free(freeme2);
	   if(!theLine) {
	     if(strcmp(fname,opts->source)!=0)
	     printf("error: syntax in line %d in included file '%s'\n",theLine->linenum,fname);
	     else
	     printf("error: syntax in line %s\n",theLine->linenum);
	   }	
	   if(addressDetermineMode(theLine,LC,type)) {
             if(strcmp(fname,opts->source)!=0)
                printf("error: syntax in line %d in included file '%s'\n",theLine->linenum,fname);
	     else
	    	printf("error: syntax in line %d\n",theLine->linenum);
	     return(1);
	   }
	  }
	  else
           if(addressDetermineMode(theLine,LC,0)) {
             if(strcmp(fname,opts->source)!=0)
                printf("error: syntax in line %d in file '%s'\n",theLine->linenum,fname);
             else
                printf("error: syntax in line %d\n",theLine->linenum);
             return(1);
	   }
	}
        return(0);
}
void globalCleanUp(void)
{
	labelCleanUp();
	listCleanUp();
}
void incrLC(address *LC,address size)
{
	char *freeme;
	char buf[15];
	line *theLine=(line *)malloc(sizeof(line));
	if(!theLine) memory_error();
         if( ((*LC&0xffff) + size) > (0xffff-3) ) {
	  if( ((*LC&0xffff) +4) < (0x10000)) {
	   theLine->opcode=(char *)strdup("JMP");
	   if(!theLine->opcode) memory_error();
	   theLine->first=(char*)malloc(30);
	   if(!theLine->first) memory_error();
	   strcpy(theLine->first,"<");
	   if(!HiRom)
	   sprintf((char *)buf,"%u",(*LC&0xff0000)+(0x018000));
	   else
           sprintf((char *)buf,"%u",(*LC&0xff0000)+(0x010000));
	   strcat(theLine->first,buf);
	   theLine->addr=*LC;
	  if(!HiRom)
	   theLine->addrend=(*LC&0xff0000)+(0x018000);
	  else
           theLine->addrend=(*LC&0xff0000)+(0x010000);
	   theLine->linenum=0;
	   theLine->mode=0x06;
	   theLine->comment=NULL;
	   theLine->second=NULL;
	   theLine->label=(char *)strdup("");
	   if(!theLine->label) memory_error();
	   listPutLine(freeme=parserPrintline(theLine,*LC,0,theLine->addrend));
	   if(freeme)
		free(freeme);
	   lineCleanUp(theLine);
	  }
	  else 
	    printf("\nwarning: code crossed bank boundry, unable to insert jump long!\n");
	  if(!HiRom)
           *LC=(*LC&0xff0000)+(0x018000);
	  else
           *LC=(*LC&0xff0000)+0x010000;
	 }
	*LC+=size;
}

