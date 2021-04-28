/***     65816 Cross Assembler v2.0 (c)1994 Jeremy Gordon     ***/

#include <string.h>
#include "65816.h"
#include "directvs.h"
#include "machine.h"
#include "address.h"
#include "parser.h"
#include "evaluate.h"
#include "label.h"

int addressDetermineMode(line *theLine,address *LC,int type)
{
	if(strcmp(theLine->first,"")==0) {
	   	incrLC(LC,1);
		theLine->mode=IMPLIED;
	  }
	else if(theLine->second==NULL) {
	  if(theLine->first[0]=='[')  {
		if(theLine->first[1]=='<') {
		    incrLC(LC,2);
		    theLine->mode=DIRECTPAGEINDIRECTLONG;
		}
		else {
		    incrLC(LC,3);
		    theLine->mode=ABSOLUTEINDIRECTLONG;
		}
	  }
	  else if(theLine->first[0]=='(') {
		if(theLine->first[1]=='<') {
		    incrLC(LC,2);
		    theLine->mode=DIRECTPAGEINDIRECT;
		}
		else {
		    incrLC(LC,3);
		    theLine->mode=ABSOLUTEINDIRECT;
		}
	  }
	  else if(theLine->first[0]=='#') {
	    if(theLine->first[1]=='<') {
		incrLC(LC,2);
		theLine->mode=IMMEDIATE;
		}
	    else {
		if(strcmp((theLine->opcode+1),"EP")==0)
		 incrLC(LC,2);
	        else 
		 incrLC(LC,3);
		theLine->mode=IMMEDIATEW;	
	    }
	  }
	  else if(theLine->first[0]=='<') {
		incrLC(LC,2);
		theLine->mode=DIRECTPAGE;
	  }
	  else if(theLine->first[0]=='>'){
		incrLC(LC,4);
		theLine->mode=ABSOLUTELONG;
	  }
	  else {
	       if((theLine->opcode[0]!='B')||(strcmp(theLine->opcode,"BRL")==0)||
		   (strcmp(theLine->opcode,"BIT")==0))
                incrLC(LC,3);
	       else
		incrLC(LC,2);
                theLine->mode=ABSOLUTE;
	  }
	}
	else {
	  if(strcmp(theLine->second,"X")==0) {
		if(theLine->first[0]=='<') {
			incrLC(LC,2);
			theLine->mode=DIRECTPAGEINDEXEDX;
		}
		else if(theLine->first[0]=='>') {
			incrLC(LC,4);
			theLine->mode=ABSOLUTELONGINDEXEDX;
		}
		else  {
			incrLC(LC,3);
			theLine->mode=ABSOLUTEINDEXEDX;
		}
	  }
	  else if(strcmp(theLine->second,"Y")==0) {
		if(theLine->first[0]=='(') {
			incrLC(LC,2);
			theLine->mode=DIRECTPAGEINDIRECTINDEXEDY;
		}
		else if(theLine->first[0]=='[') {
			incrLC(LC,2);
			theLine->mode=DIRECTPAGEINDIRECTLONGINDEXEDY;
		}
		else if(theLine->first[0]=='<') {
			incrLC(LC,2);
			theLine->mode=DIRECTPAGEINDEXEDY;
		}
		else {
			incrLC(LC,3);
			theLine->mode=ABSOLUTEINDEXEDY;
		}
	  }
	  else if(strcmp(theLine->second,"X)")==0) {
		if(theLine->first[0]!='<') {
			incrLC(LC,3);
			theLine->mode=ABSOLUTEINDIRECTINDEXED;
		}
		else {
			incrLC(LC,2);
			theLine->mode=DIRECTPAGEINDEXEDINDIRECTX;
		}
	  }
	  else if(strcmp(theLine->second,"S")==0) {
			incrLC(LC,2);
			theLine->mode=STACKRELATIVE;
		}
	  else if(strcmp(theLine->second,"S)")==0) {
			incrLC(LC,2);
			theLine->mode=STACKRELATIVEINDIRECTINDEXEDY;
		}
	  else {
		incrLC(LC,3);
		theLine->mode=BLOCKMOVE;
	  }
	}
	return(0);
}
int search(char *array, char *thing, int size,int howmany)
{
	int found=0;
	int count=0;
	while(!found&&(count<howmany)) {
	  if(strcmp((char*)(array+(count*size)),thing)==0)
	    found=1;
	  count++;
	}
	if(found)
	  return(count-1);
	else
	  return(-1);
}




static char lookup_absolute_1[36][4]={"ADC","AND","ASL","BIT","CMP","CPX","CPY",
		"DEC","EOR","INC","JMP","JSR","LDA","LDX","LDY","LSR","ORA","ROL",
		"ROR","SBC","STA","STX","STY","STZ","TRB","TSB",
                        "BCC","BCS","BEQ","BMI","BNE","BPL","BRA",
                        "BVC","BVS","BRL"};
static byte lookup_absolute_2[36]={0x6d,0x2d,0x0e,0x2c,0xcd,0xec,0xcc,0xce,0x4d,
		0xee,0x4c,0x20,0xad,0xae,0xac,0x4e,0x0d,0x2e,0x6e,0xed,0x8d,0x8e,
		0x8c,0x9c,0x1c,0x0c,
		0x90,0xb0,0xf0,0x30,0xd0,0x10,0x80,
                              0x50,0x70,0x82};
int address_absolute(line *theLine,FILE *fp,FILE *fp2,int flag)
{
        int error,loc;
        address dontcare;
        word value=evaluate((char*)strtok(theLine->first,DIRECTIVES),&error,&dontcare);
	byte branch;
	word branch2;
	char buf[255],buf2[255];
        if(error)
          return(1);
        loc=search((char *)lookup_absolute_1,theLine->opcode,4,36);
        if(loc==-1)
          return(1);
        if(fputc(lookup_absolute_2[loc],fp)==EOF)
          return(1);
	if(loc<26){
        if(fputc(value&0xff,fp)==EOF)
          return(1);
        if(fputc((value>>8)&0xff,fp)==EOF)
          return(1);
	if(flag){
	sprintf(buf2,"%s %s %s",dectohex(lookup_absolute_2[loc],2),
                dectohex(value&0xff,2),dectohex((value>>8)&0xff,2));
	sprintf(buf,"%-15s %s $%s\n",buf2,lookup_absolute_1[loc],
		dectohex(value&0xffff,4));
	fputs(buf,fp2);
	}
	}
        else if(loc!=35){
          if(theLine->addrend>value) {
            if((theLine->addrend-value)<128) {
                printf("\nerror: attempt to branch less that 128 away");
                return(1);
            }
	    branch=theLine->addrend-value;
	    branch=(branch^0xff)+1;
          }
          else {
           if((value-theLine->addrend)>127) {
                printf("\nerror: attempt to branch greater than 127 away");
                return(1);
           }
           branch=value-theLine->addrend;
          }
          if(fputc(branch&0xff,fp)==EOF)
            return(1);
	if(flag) {
        sprintf(buf2,"%s %s",dectohex(lookup_absolute_2[loc],2),
                dectohex(value&0xff,2));
        sprintf(buf,"%-15s %s $%s\n",buf2,lookup_absolute_1[loc],
                dectohex(value&0xff,2));
	fputs(buf,fp2);
	}
        }
	else {
          if(theLine->addrend>value) {
            if((theLine->addrend-value)<32768) {
                printf("\nerror: attempt to branch less that 32768 away");
                return(1);
            }
            branch2=theLine->addrend-value;
            branch2=(branch2^0xffff)+1;
          }
          else {
           if((value-theLine->addrend)>32767) {
                printf("\nerror: attempt to branch greater than 32767 away");
                return(1);
           }
           branch2=value-theLine->addrend;
          }
	if(flag){
        sprintf(buf2,"%s %s %s",dectohex(lookup_absolute_2[loc],2),
                dectohex(value&0xff,2),dectohex((value>>8)&0xff,2));
        sprintf(buf,"%-15s %s $%s\n",buf2,lookup_absolute_1[loc],
                dectohex(value&0xffff,4));
	fputs(buf,fp2);
	}
          if(fputc(branch2&0xff,fp)==EOF)
            return(1);
	  if(fputc((branch2>>8)&0xff,fp)==EOF)
	    return(1);
	}
        return(0);
}
static char lookup_absoluteindexedx_1[17][4]={"ADC","AND","ASL","BIT","CMP",
                "DEC","EOR","INC","LDA","LDY","LSR","ORA","ROL",
                "ROR","SBC","STA","STZ"};
static byte lookup_absoluteindexedx_2[17]={0x7d,0x3d,0x1e,0x3c,0xdd,0xde,0x5d,
                0xfe,0xbd,0xbc,0x5e,0x1d,0x3e,0x7e,0xfd,0x9d,0x9e};
int address_absoluteindexedx(line *theLine,FILE *fp,FILE *fp2,int flag)
{
        int error,loc;
        address dontcare;
        word value=evaluate((char*)strtok(theLine->first,DIRECTIVES),&error,&dontcare);
	char buf[255],buf2[255];
        if(error)
          return(1);
        loc=search((char *)lookup_absoluteindexedx_1,theLine->opcode,4,17);
        if(loc==-1)
          return(1);
        if(fputc(lookup_absoluteindexedx_2[loc],fp)==EOF)
          return(1);
        if(fputc(value&0xff,fp)==EOF)
          return(1);
        if(fputc((value>>8)&0xff,fp)==EOF)
          return(1);
	if(flag) {
        sprintf(buf2,"%s %s %s",dectohex(lookup_absoluteindexedx_2[loc],2),
                dectohex(value&0xff,2),dectohex((value>>8)&0xff,2));
        sprintf(buf,"%-15s %s $%s,X\n",buf2,lookup_absoluteindexedx_1[loc],
                dectohex(value&0xffff,4));
        fputs(buf,fp2);
	}

        return(0);
}
static char lookup_absoluteindexedy_1[9][4]={"ADC","AND","CMP",
                "EOR","LDA","LDX","ORA",
                "SBC","STA"};
static byte lookup_absoluteindexedy_2[9]={0x79,0x39,0xd9,0x59,
                0xb9,0xbe,0x19,0xf9,0x99};
int address_absoluteindexedy(line *theLine,FILE *fp,FILE *fp2,int flag)
{
        int error,loc;
        address dontcare;
	char buf[255],buf2[255];
        word value=evaluate((char*)strtok(theLine->first,DIRECTIVES),&error,&dontcare);
        if(error)
          return(1);
        loc=search((char *)lookup_absoluteindexedy_1,theLine->opcode,4,9);
        if(loc==-1)
          return(1);
        if(fputc(lookup_absoluteindexedy_2[loc],fp)==EOF)
          return(1);
        if(fputc(value&0xff,fp)==EOF)
          return(1);
        if(fputc((value>>8)&0xff,fp)==EOF)
          return(1);
	if(flag){
        sprintf(buf2,"%s %s %s",dectohex(lookup_absoluteindexedy_2[loc],2),
                dectohex(value&0xff,2),dectohex((value>>8)&0xff,2));
        sprintf(buf,"%-15s %s $%s,Y\n",buf2,lookup_absoluteindexedy_1[loc],
                dectohex(value&0xffff,4));
        fputs(buf,fp2);
	}

        return(0);
}
static char lookup_absoluteindirectindexed_1[2][4]={"JMP","JSR"};
static byte lookup_absoluteindirectindexed_2[2]={0x7c,0xfc};
int address_absoluteindirectindexed(line *theLine,FILE *fp,FILE *fp2,int flag)
{
        int error,loc;
        address dontcare;
	char buf[255],buf2[255];
        word value=evaluate((char*)strtok(theLine->first,DIRECTIVES),&error,&dontcare);
        if(error)
          return(1);
        loc=search((char *)lookup_absoluteindirectindexed_1,theLine->opcode,4,2);
        if(loc==-1)
          return(1);
        if(fputc(lookup_absoluteindirectindexed_2[loc],fp)==EOF)
          return(1);
        if(fputc(value&0xff,fp)==EOF)
          return(1);
        if(fputc((value>>8)&0xff,fp)==EOF)
          return(1);
	if(flag){
        sprintf(buf2,"%s %s %s",dectohex(lookup_absoluteindirectindexed_2[loc],2),
                dectohex(value&0xff,2),dectohex((value>>8)&0xff,2));
        sprintf(buf,"%-15s %s ($%s,X)\n",buf2,lookup_absoluteindirectindexed_1[loc],
                dectohex(value&0xffff,4));
        fputs(buf,fp2);
	}
        return(0);
}
static char lookup_absoluteindirect_1[2][4]={"JML","JMP"};
static byte lookup_absoluteindirect_2[2]={0xdc,0x6c};
int address_absoluteindirect(line *theLine,FILE *fp,FILE *fp2,int flag)
{
        int error,loc;
        address dontcare;
	char buf[255],buf2[255];
        address value=evaluate((char*)strtok(theLine->first,DIRECTIVES),&error,&dontcare);
        if(error)
          return(1);
        loc=search((char *)lookup_absoluteindirect_1,theLine->opcode,4,2);
        if(loc==-1)
          return(1);
        if(fputc(lookup_absoluteindirect_2[loc],fp)==EOF)
          return(1);
        if(fputc(value&0xff,fp)==EOF)
          return(1);
        if(fputc((value>>8)&0xff,fp)==EOF)
          return(1);
	if(flag){
        sprintf(buf2,"%s %s %s",dectohex(lookup_absoluteindirect_2[loc],2),
                dectohex(value&0xff,2),dectohex((value>>8)&0xff,2));
        sprintf(buf,"%-15s %s ($%s)\n",buf2,lookup_absoluteindirect_1[loc],
                dectohex(value&0xffff,4));
        fputs(buf,fp2);
	}
        return(0);
}
int address_absoluteindirectlong(line *theLine,FILE *fp,FILE *fp2,int flag)
{
	return(address_absoluteindirect(theLine,fp,fp2,flag));
}
static char lookup_absolutelong_1[10][4]={"ADC","AND","CMP","EOR","JMP","JSL",
			"LDA","ORA","SBC","STA"};

static byte lookup_absolutelong_2[10]={0x6f,0x2f,0xcf,0x4f,0x5c,0x22,0xaf,0x0f,
			0xef,0x8f};
int address_absolutelong(line *theLine,FILE *fp,FILE *fp2,int flag)
{
        int error,loc;
        address dontcare;
	char buf[255],buf2[255];
        address value=evaluate((char*)strtok(theLine->first,DIRECTIVES),&error,&dontcare);
        if(error)
          return(1);
        loc=search((char *)lookup_absolutelong_1,theLine->opcode,4,10);
        if(loc==-1)
          return(1);
        if(fputc(lookup_absolutelong_2[loc],fp)==EOF)
          return(1);
        if(fputc(value&0xff,fp)==EOF)
          return(1);
        if(fputc((value>>8)&0xff,fp)==EOF)
          return(1);
	if(fputc((value>>16)&0xff,fp)==EOF)
	  return(1);
	if(flag) {
        sprintf(buf2,"%s %s %s %s",dectohex(lookup_absolutelong_2[loc],2),
                dectohex(value&0xff,2),dectohex((value>>8)&0xff,2),
		dectohex((value>>16)&0xff,2));
        sprintf(buf,"%-15s %s $%s\n",buf2,lookup_absolutelong_1[loc],
                dectohex(value&0xffffff,6));
        fputs(buf,fp2);
	}

        return(0);
}
static char lookup_absolutelongindexedx_1[8][4]={"ADC","AND","CMP","EOR",
                        "LDA","ORA","SBC","STA"};
static byte lookup_absolutelongindexedx_2[8]={0x7f,0x3f,0xdf,0x5f,0xbf,0x1f,
                        0xff,0x9f};
int address_absolutelongindexedx(line *theLine,FILE *fp,FILE *fp2,int flag)
{
        int error,loc;
        address dontcare;
	char buf[255],buf2[255];
        address value=evaluate((char*)strtok(theLine->first,DIRECTIVES),&error,&dontcare);
        if(error)
          return(1);
        loc=search((char *)lookup_absolutelongindexedx_1,theLine->opcode,4,8);
        if(loc==-1)
          return(1);
        if(fputc(lookup_absolutelongindexedx_2[loc],fp)==EOF)
          return(1);
        if(fputc(value&0xff,fp)==EOF)
          return(1);
        if(fputc((value>>8)&0xff,fp)==EOF)
          return(1);
        if(fputc((value>>16)&0xff,fp)==EOF)
          return(1);
	if(flag) {
        sprintf(buf2,"%s %s %s %s",dectohex(lookup_absolutelongindexedx_2[loc],2),
                dectohex(value&0xff,2),dectohex((value>>8)&0xff,2),
                dectohex((value>>16)&0xff,2));
        sprintf(buf,"%-15s %s $%s,X\n",buf2,lookup_absolutelongindexedx_1[loc],
                dectohex(value&0xffffff,6));
        fputs(buf,fp2);
	}
        return(0);
}
static char lookup_blockmove_1[2][4]={"MVP","MVN"};
static byte lookup_blockmove_2[2]={0x44,0x54};
int address_blockmove(line *theLine,FILE *fp,FILE *fp2,int flag)
{
        int error,loc,error2;
        address dontcare;
	char buf[255],buf2[255];
        address value=evaluate((char*)strtok(theLine->first,DIRECTIVES),&error,&dontcare);
	address value2;
	theLine->second=convertHex(upper(theLine->second));
	value2=evaluate((char *)strtok(theLine->second,DIRECTIVES),&error2,&dontcare);
        if(error|error2)
          return(1);
        loc=search((char *)lookup_blockmove_1,theLine->opcode,4,2);
        if(loc==-1)
          return(1);
        if(fputc(lookup_blockmove_2[loc],fp)==EOF)
          return(1);
        if(fputc(value&0xff,fp)==EOF)
          return(1);
        if(fputc(value2&0xff,fp)==EOF)
          return(1);
	if(flag) {
        sprintf(buf2,"%s %s %s",dectohex(lookup_blockmove_2[loc],2),
                dectohex(value&0xff,2),dectohex(value2&0xff,2));
        sprintf(buf,"%-15s %s #$%s #$%s\n",buf2,lookup_blockmove_1[loc],
                dectohex(value&0xff,2),dectohex(value2&0xff,2));
        fputs(buf,fp2);
	}

        return(0);
}
static char lookup_directpage_1[24][4]={"ADC","AND","ASL","BIT","CMP","CPX","CPY",
                "DEC","EOR","INC","LDA","LDX","LDY","LSR","ORA","ROL",
                "ROR","SBC","STA","STX","STY","STZ","TRB","TSB"};
static byte lookup_directpage_2[24]={0x65,0x25,0x06,0x24,0xc5,0xe4,0xc4,0xc6,0x45,
                0xe6,0xa5,0xa6,0xa4,0x46,0x05,0x26,0x66,0xe5,0x85,0x86,
                0x84,0x64,0x14,0x04};

int address_directpage(line *theLine,FILE *fp,FILE *fp2,int flag)
{
        int error,loc;
        address dontcare;
	char buf[255],buf2[255];
        word value=evaluate((char*)strtok(theLine->first,DIRECTIVES),&error,&dontcare);
        if(error)
          return(1);
        loc=search((char *)lookup_directpage_1,theLine->opcode,4,24);
        if(loc==-1)
          return(1);
        if(fputc(lookup_directpage_2[loc],fp)==EOF)
          return(1);
        if(fputc(value&0xff,fp)==EOF)
          return(1);
	if(flag){
        sprintf(buf2,"%s %s",dectohex(lookup_directpage_2[loc],2),
                dectohex(value&0xff,2));
        sprintf(buf,"%-15s %s $%s\n",buf2,lookup_directpage_1[loc],
                dectohex(value&0xff,2));
        fputs(buf,fp2);
	}
        return(0);
}
static char lookup_directpageindexedx_1[18][4]={"ADC","AND","ASL","BIT","CMP",
                "DEC","EOR","INC","LDA","LDY","LSR","ORA","ROL",
                "ROR","SBC","STA","STY","STZ"};
static byte lookup_directpageindexedx_2[18]={0x75,0x35,0x16,0x34,0xd5,0xd6,0x55,0xf6,0xb5,0xb4,0x56,0x15,
		0x36,0x76,0xf5,0x95,0x94,0x74};
int address_directpageindexedx(line *theLine,FILE *fp,FILE *fp2,int flag)
{
        int error,loc;
        address dontcare;
	char buf[255],buf2[255];
        word value=evaluate((char*)strtok(theLine->first,DIRECTIVES),&error,&dontcare);
        if(error)
          return(1);
        loc=search((char *)lookup_directpageindexedx_1,theLine->opcode,4,18);
        if(loc==-1)
          return(1);
        if(fputc(lookup_directpageindexedx_2[loc],fp)==EOF)
          return(1);
        if(fputc(value&0xff,fp)==EOF)
          return(1);
	if(flag){
        sprintf(buf2,"%s %s",dectohex(lookup_directpageindexedx_2[loc],2),
                dectohex(value&0xff,2));
        sprintf(buf,"%-15s %s $%s,X\n",buf2,lookup_directpageindexedx_1[loc],
                dectohex(value&0xff,2));
	fputs(buf,fp2);
	}
        return(0);
}
static char lookup_directpageindexedy_1[2][4]={"LDX","STX"};
static byte lookup_directpageindexedy_2[2]={0xb6,0x96};
int address_directpageindexedy(line *theLine,FILE *fp,FILE *fp2,int flag)
{
       int error,loc;
	char buf[255],buf2[255];
        address dontcare;
        word value=evaluate((char*)strtok(theLine->first,DIRECTIVES),&error,&dontcare);
        if(error)
          return(1);
        loc=search((char *)lookup_directpageindexedy_1,theLine->opcode,4,2);
        if(loc==-1)
          return(1);
        if(fputc(lookup_directpageindexedy_2[loc],fp)==EOF)
          return(1);
        if(fputc(value&0xff,fp)==EOF)
          return(1);
	if(flag){
        sprintf(buf2,"%s %s",dectohex(lookup_directpageindexedy_2[loc],2),
                dectohex(value&0xff,2));
        sprintf(buf,"%-15s %s $%s,Y\n",buf2,lookup_directpageindexedy_1[loc],
                dectohex(value&0xff,2));
        fputs(buf,fp2);
	}

        return(0);
}
static char lookup_dpindexedindirectx_1[8][4]={"ADC","AND","CMP","EOR",
                        "LDA","ORA","SBC","STA"};
static byte lookup_dpindexedindirectx_2[8]={0x61,0x21,0xc1,0x41,0xa1,0x01,
                        0xe1,0x81};
int address_directpageindexedindirectx(line *theLine,FILE *fp,FILE *fp2,int flag)
{
       int error,loc;
	char buf[255],buf2[255];
        address dontcare;
        word value=evaluate((char*)strtok(theLine->first,DIRECTIVES),&error,&dontcare);
        if(error)
          return(1);
        loc=search((char *)lookup_dpindexedindirectx_1,theLine->opcode,4,8);
        if(loc==-1)
          return(1);
        if(fputc(lookup_dpindexedindirectx_2[loc],fp)==EOF)
          return(1);
        if(fputc(value&0xff,fp)==EOF)
          return(1);
	if(flag) {
        sprintf(buf2,"%s %s",dectohex(lookup_dpindexedindirectx_2[loc],2),
                dectohex(value&0xff,2));
        sprintf(buf,"%-15s %s ($%s,X)\n",buf2,lookup_dpindexedindirectx_1[loc],
                dectohex(value&0xff,2));
        fputs(buf,fp2);
	}

        return(0);
}
static char lookup_directpageindirect_1[8][4]={"ADC","AND","CMP","EOR",
                        "LDA","ORA","SBC","STA"};
static byte lookup_directpageindirect_2[8]={0x72,0x32,0xd2,0x52,0xb2,0x12,
                        0xf2,0x92};
int address_directpageindirect(line *theLine,FILE *fp,FILE *fp2,int flag)
{
       int error,loc;
        address dontcare;
	char buf[255],buf2[255];
        word value=evaluate((char*)strtok(theLine->first,DIRECTIVES),&error,&dontcare);
        if(error)
          return(1);
        loc=search((char *)lookup_directpageindirect_1,theLine->opcode,4,8);
        if(loc==-1)
          return(1);
        if(fputc(lookup_directpageindirect_2[loc],fp)==EOF)
          return(1);
        if(fputc(value&0xff,fp)==EOF)
          return(1);
	if(flag) {
        sprintf(buf2,"%s %s",dectohex(lookup_directpageindirect_2[loc],2),
                dectohex(value&0xff,2));
        sprintf(buf,"%-15s %s ($%s)\n",buf2,lookup_directpageindirect_1[loc],
                dectohex(value&0xff,2));
        fputs(buf,fp2);
	}
        return(0);

}
static char lookup_directpageindirectlong_1[8][4]={"ADC","AND","CMP","EOR",
                        "LDA","ORA","SBC","STA"};
static byte lookup_directpageindirectlong_2[8]={0x67,0x27,0xc7,0x47,0xa7,0x07,
                        0xe7,0x87};
int address_directpageindirectlong(line *theLine,FILE *fp,FILE *fp2,int flag)
{
       int error,loc;
        address dontcare;
	char buf[255],buf2[255];
        word value=evaluate((char*)strtok(theLine->first,DIRECTIVES),&error,&dontcare);
        if(error)
          return(1);
        loc=search((char *)lookup_directpageindirectlong_1,theLine->opcode,4,8);
        if(loc==-1)
          return(1);
        if(fputc(lookup_directpageindirectlong_2[loc],fp)==EOF)
          return(1);
        if(fputc(value&0xff,fp)==EOF)
          return(1);
	if(flag){
        sprintf(buf2,"%s %s",dectohex(lookup_directpageindirectlong_2[loc],2),
                dectohex(value&0xff,2));
        sprintf(buf,"%-15s %s [$%s]\n",buf2,lookup_directpageindirectlong_1[loc],
                dectohex(value&0xff,2));
        fputs(buf,fp2);
	}

        return(0);
}
static char lookup_dpindirectindexedy_1[8][4]={"ADC","AND","CMP","EOR",
                        "LDA","ORA","SBC","STA"};
static byte lookup_dpindirectindexedy_2[8]={0x71,0x31,0xd1,0x51,0xb1,0x11,
                        0xf1,0x91};
int address_directpageindirectindexedy(line *theLine,FILE *fp,FILE *fp2,int flag)
{
       int error,loc;
        address dontcare;
	char buf[255],buf2[255];
        word value=evaluate((char*)strtok(theLine->first,DIRECTIVES),&error,&dontcare);
        if(error)
          return(1);
        loc=search((char *)lookup_dpindirectindexedy_1,theLine->opcode,4,8);
        if(loc==-1)
          return(1);
        if(fputc(lookup_dpindirectindexedy_2[loc],fp)==EOF)
          return(1);
        if(fputc(value&0xff,fp)==EOF)
          return(1);
	if(flag){
        sprintf(buf2,"%s %s",dectohex(lookup_dpindirectindexedy_2[loc],2),
                dectohex(value&0xff,2));
        sprintf(buf,"%-15s %s ($%s),Y\n",buf2,lookup_dpindirectindexedy_1[loc],
                dectohex(value&0xff,2));
        fputs(buf,fp2);
	}

        return(0);
}
static char lookup_dpindirectlongindexedy_1[8][4]={"ADC","AND","CMP","EOR",
                        "LDA","ORA","SBC","STA"};
static byte lookup_dpindirectlongindexedy_2[8]={0x77,0x37,0xd7,0x57,0xb7,0x17,
                        0xf7,0x97};
int address_directpageindirectlongindexedy(line *theLine,FILE *fp,FILE *fp2,int flag)
{
       int error,loc;
        address dontcare;
	char buf[255],buf2[255];
        word value=evaluate((char*)strtok(theLine->first,DIRECTIVES),&error,&dontcare);
        if(error)
          return(1);
        loc=search((char *)lookup_dpindirectlongindexedy_1,theLine->opcode,4,8);
        if(loc==-1)
          return(1);
        if(fputc(lookup_dpindirectlongindexedy_2[loc],fp)==EOF)
          return(1);
        if(fputc(value&0xff,fp)==EOF)
          return(1);
	if(flag) {
        sprintf(buf2,"%s %s",dectohex(lookup_dpindirectlongindexedy_2[loc],2),
                dectohex(value&0xff,2));
        sprintf(buf,"%-15s %s [$%s],Y\n",buf2,lookup_dpindirectlongindexedy_1[loc],
                dectohex(value&0xff,2));
        fputs(buf,fp2);
	}
        return(0);
}
static char lookup_immediate_1[14][4]={"ADC","AND","BIT","CMP","CPX","CPY",
                        "EOR","LDA","LDX","LDY","ORA","SBC","SEP","REP"};
static byte lookup_immediate_2[14]={0x69,0x29,0x89,0xc9,0xe0,0xc0,0x49,0xa9,0xa2,
                              0xa0,0x09,0xe9,0xe2,0xc2};
int address_immediate(line *theLine,FILE *fp,FILE *fp2,int flag)
{
        int error,loc;
        address dontcare;
	char buf[255],buf2[255];
        word value=evaluate((char*)strtok(theLine->first,DIRECTIVES),&error,&dontcare);
        if(error)
          return(1);
	loc=search((char *)lookup_immediate_1,theLine->opcode,4,12);
	if(loc==-1)
	  return(1);
        if(fputc(lookup_immediate_2[loc],fp)==EOF)
          return(1);
        if(fputc(value&0xff,fp)==EOF)
          return(1);
	if(flag) {
        sprintf(buf2,"%s %s",dectohex(lookup_immediate_2[loc],2),
                dectohex(value&0xff,2));
        sprintf(buf,"%-15s %s #$%s\n",buf2,lookup_immediate_1[loc],
                dectohex(value&0xff,2));
        fputs(buf,fp2);
	}

        return(0);
}
int address_immediatew(line *theLine,FILE *fp,FILE *fp2,int flag)
{
        int error,loc;
        address dontcare;
	char branch;
	char buf[255],buf2[255];
        address value=evaluate((char *)strtok(theLine->first,DIRECTIVES),&error,&dontcare);
        if(error)
          return(1);
        loc=search((char *)lookup_immediate_1,theLine->opcode,4,14);
        if(loc==-1)
          return(1);
        if(fputc(lookup_immediate_2[loc],fp)==EOF)
          return(1);
	if(loc<12) {
        if(fputc(value&0xff,fp)==EOF)
                return(1);
         if(fputc((value>>8)&0xff,fp)==EOF)
          return(1);
	if(flag){
        sprintf(buf2,"%s %s %s",dectohex(lookup_immediate_2[loc],2),
                dectohex(value&0xff,2),dectohex((value>>8)&0xff,2));
        sprintf(buf,"%-15s %s #$%s\n",buf2,lookup_immediate_1[loc],
                dectohex(value&0xffff,4));
        fputs(buf,fp2);
	}

	}
	else {
	if(fputc(value&0xff,fp)==EOF)
		return(1);
	if(flag){
        sprintf(buf2,"%s %s",dectohex(lookup_immediate_2[loc],2),
                dectohex(value&0xff,2));
        sprintf(buf,"%-15s %s #$%s\n",buf2,lookup_immediate_1[loc],
                dectohex(value&0xff,2));
        fputs(buf,fp2);
	}

	}

        return(0);
}
static char lookup_implied_1[58][4]={"CLC","CLD","CLI","CLV","DEX","DEY","INX","INY",
		"NOP","SEC","SED","SEI","STP","TAX","TAY","TCD","TCS","TDC","ASL",
		"DEC","INC","LSR","ROL","ROR","TSC","TSX","TXA","TXS","TXY",
		"TYA","TYX","WAI","WDM","XBA","XCE","BRK","COP","LDA","PEA","PEI",
		"PER","PHA","PHB","PHD","PHK","PHP","PHX","PHY","PLA","PLB","PLD",
		"PLP","PLX","PLY","RTI","RTL","RTS"};		
static byte lookup_implied_2[58]={0x18,0xd8,0x58,0xb8,0xca,0x88,0xe8,0xc8,0xea,0x38,
		0xf8,0x78,0xdb,0xaa,0xa8,0x5b,0x1b,0x7b,0x0a,0x3a,0x1a,0x4a,0x2a,0x6a,
		0x3b,0xba,0x8a,0x9a,0x9b,0x98,0xbb,0xcb,0x42,0xeb,0xfb,0x00,0x02,
		0xa3,0xf4,0xd4,0x62,0x48,0x8b,0x0b,0x4b,0x08,0xda,0x5a,0x68,0xab,0x2b,
		0x28,0xfa,0x7a,0x40,0x6b,0x60};
int address_implied(line *theLine,FILE *fp,FILE *fp2,int flag)
{
        int error,loc;
	char buf[255],buf2[255];
        address dontcare;
        loc=search((char *)lookup_implied_1,theLine->opcode,4,58);
        if(loc==-1)
          return(1);
        if(fputc(lookup_implied_2[loc],fp)==EOF)
          return(1);
	if(flag){
        sprintf(buf2,"%s",dectohex(lookup_implied_2[loc],2));
        sprintf(buf,"%-15s %s\n",buf2,lookup_implied_1[loc]);
        fputs(buf,fp2);
	}


        return(0);
}
static char lookup_stackrelative_1[8][4]={"ADC","AND","CMP","EOR","LDA","ORA","SBC","STA"};
static byte lookup_stackrelative_2[8]={0x63,0x23,0xc3,0x43,0xb3,0x03,0xe3,0x83};

int address_stackrelative(line *theLine,FILE *fp,FILE *fp2,int flag)
{
        int error,loc;
        address dontcare;
	char buf[255],buf2[255];
        char branch;
        address value=evaluate((char *)strtok(theLine->first,DIRECTIVES),&error,&dontcare);
        if(error)
          return(1);
        loc=search((char *)lookup_stackrelative_1,theLine->opcode,4,8);
        if(loc==-1)
          return(1);
        if(fputc(lookup_stackrelative_2[loc],fp)==EOF)
          return(1);
        if(fputc(value&0xff,fp)==EOF)
          return(1);
	if(flag){
        sprintf(buf2,"%s %s",dectohex(lookup_absolute_2[loc],2),
                dectohex(value&0xff,2));
        sprintf(buf,"%-15s %s $%s,S\n",buf2,lookup_absolute_1[loc],
                dectohex(value&0xff,2));
        fputs(buf,fp2);
	}


        return(0);
}
static char lookup_srindirectindexedy_1[7][4]={"ADC","AND","CMP","EOR","ORA","SBC","STA"};
static byte lookup_srindirectindexedy_2[7]={0x73,0x33,0xd3,0x53,0x13,0xf3,0x93};

int address_stackrelativeindirectindexedy(line *theLine,FILE *fp,FILE *fp2,int flag)
{
        int error,loc;
        address dontcare;
	char buf[255],buf2[255];
        char branch;
        address value=evaluate((char *)strtok(theLine->first,DIRECTIVES),&error,&dontcare);
        if(error)
          return(1);
        loc=search((char *)lookup_srindirectindexedy_1,theLine->opcode,4,7);
        if(loc==-1)
          return(1);
        if(fputc(lookup_srindirectindexedy_2[loc],fp)==EOF)
          return(1);
        if(fputc(value&0xff,fp)==EOF)
          return(1);
	if(flag){
        sprintf(buf2,"%s %s",dectohex(lookup_srindirectindexedy_2[loc],2),
                dectohex(value&0xff,2));
        sprintf(buf,"%-15s %s ($%s,S),Y\n",buf2,lookup_srindirectindexedy_1[loc],
                dectohex(value&0xff,2));
        fputs(buf,fp2);
	}


        return(0);
}
int address_directive(line *theLine,FILE *fp,FILE *fp3,int flag,options *opts)
{
	FILE *fp2;
	int size,temp2,hirom=0,count,err;
	address value,dontcare;
	char *buf,expr[255],temp3[255];
	char *freeme,*temp,*junk,*junk3,*freeme2;

	if(strcmp(theLine->opcode,BIN)==0) {
          if(fp2=fopen(theLine->first,"rb")) {
            fseek(fp2,0,2);
            size=(int)ftell(fp2);
	    if(size<0x8000)
	      buf=(char*)malloc(0x8000);
	    else
	      buf=(char*)malloc(size);
            fseek(fp2,0,0);
	    value=theLine->addr;
	    fread(buf,size,1,fp2);
	    fwrite(buf,size,1,fp);
	    free(buf);
	    fclose(fp2);
          }
          else {
            printf("\nerror: can't read binary file '%s'\n",theLine->first);
            return(1);
          }
	if(flag) {
	  fputs("binary include: ",fp3);
	  fputs(theLine->first,fp3);
	  fputs("\n",fp3);
	}
        }
        else if((strcmp(theLine->opcode,DCB)==0)||
                (strcmp(theLine->opcode,DCW)==0)||
                (strcmp(theLine->opcode,DCD)==0)) {
	if(flag) {
	     fputs("                ",fp3);
	     if(strcmp(theLine->opcode,DCB)==0)
		fputs(DCB,fp3);
	     else if(strcmp(theLine->opcode,DCW)==0)
		fputs(DCW,fp3);
	     else
	        fputs(DCD,fp3);
	     fputs(" ",fp3);
	}
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
	    if(flag){
	     fputs(theLine->first,fp3);	
	     fputs("\n",fp3);
	    }
              while(*temp!='\0') {
                if(*temp=='"') {
                  freeme=temp+1; 
                  while((*freeme!='"')&&(*freeme!='\0'))
                    freeme++;
		  fwrite(temp+1,(freeme-temp-1),1,fp);
                  temp=freeme+1;
                }
		else if(*temp==' ')
		 temp++;
                else {
                  freeme=temp;
                  while((*freeme!=',')&&(*freeme!='\0')) 
                    freeme++;
		    
                 if(freeme>temp) {
		  strncpy(expr,temp,(freeme-temp));
		  expr[freeme-temp]='\0';

		  junk=convertHex(freeme2=upper(expr));
		  if(freeme2)
		    free(freeme2);
		  freeme2=labelReplace(junk);
		  if(junk)
		    free(junk);
		  value=evaluate(freeme2,&err,&dontcare);
		  if(freeme2)
		    free(freeme2);
		 if(err) {
		   printf("in line %d\n",theLine->linenum);
		   return(1);
		 }
                  if(strcmp(theLine->opcode,DCB)==0) {
		    fputc(value,fp);
		  }
                  else if(strcmp(theLine->opcode,DCW)==0) {
		   fputc(value,fp);
		   fputc((value>>8)&0xff,fp);
		  }
                  else if(strcmp(theLine->opcode,DCD)==0) {
                   fputc(value,fp);
                   fputc((value>>8)&0xff,fp);
		   fputc((value>>16)&0xff,fp);
		   fputc((value>>24)&0xff,fp);
		  }
		  }
		  if(*freeme!='\0')
                   temp=freeme+1;
		  else
		   temp=freeme;
                }
          }
	}
	else if(strcmp(theLine->opcode,PAD)==0) {
	  if((theLine->addrend&0xff0000)-(theLine->addr&0xff0000)) {
	   if(!opts->hiRom)
	   size=(theLine->addrend-0x8000)-theLine->addr;
	   else
	   size=(theLine->addrend-theLine->addr);
	  }
	  else
	   size=theLine->addrend-theLine->addr;
	  buf=(char*)malloc(size);
	  memset(buf,0,size);
	  fwrite(buf,size,1,fp);
	  free(buf);
	  if(flag){
	  fputs("                ",fp3);
	  fputs(PAD,fp3);
	  if(*theLine->first!='(') {
	    fputs(" $",fp3);
	    fputs(dectohex(theLine->addrend,4),fp3);
	  }
	  fputs("\n",fp3);
	}
	}
	else
	  return(1);
	return(0);
}
