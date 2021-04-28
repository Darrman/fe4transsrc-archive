/***     65816 Cross Assembler v2.0 (c)1994 Jeremy Gordon     ***/


#include "machine.h"
#ifndef AMIGA
#include <malloc.h>
#endif
#include <stdlib.h>
#include <string.h>

static int HiRom=0;

char *dectohex(address l, byte w)
{
        byte b;
	char *s;
        const char hextable[17]="0123456789ABCDEF";
	s=(char *)malloc(w+2);
	if(!s) memory_error();
        for(b=0; b<w; b++)
          s[w-b-1]=(char)hextable[(l >> (b*4)) & 0xf];
	s[b]='\0';
	return(s);
}
address hextodec(char *s)
{
        int b;
        address l,m;
        char hextable[17]="0123456789ABCDEF";
        l=0;m=1;
        for(b=strlen(s)-1;b>=0;b--) {
          l=l+( (strrchr(hextable,toupper(s[b]))-hextable) )*m;
          m=m*16;
        }
        return(l);
}
void setHiRom(int flag)
{
	HiRom=flag;
} 
void incrLCBIN(address *LC, address size)
{
        int flag=0;
        if(!HiRom) {
         if( ((*LC&0xffff) + size) > (0xffff)) {
           size-=(0xffff-((*LC&0xffff)));
           *LC=(*LC&0xff0000)+0x017fff;
           while( size > 0xffff ) {
             size-=(0x10000);
             *LC+=0x010000;
           }
         }
         *LC+=size;
        }
        else {
         if( ((*LC&0xffff) + size) > (0xffff)) {
           size-=(0xffff-((*LC&0xffff)));
           *LC=(*LC&0xff0000)+0x010000;
           while( size > 0xffff ) {
             size-=(0x10000);
             *LC+=0x010000;
           }
         }
         *LC+=size;
        }
}
char *convertHex(char *what)
{
	char little[2]=" ";
	char temp[100];
	char buf[100];
	char *result=(char *)malloc(30);
	int count=0,subcount;
	if(!result) memory_error();

	strcpy(result,"");

	if(what!=NULL) {
	while(count<strlen(what)) {
	  if(what[count]=='$') {
		subcount=0;
		count++;
		while( (count<strlen(what))&&
		  (((what[count]>='0')&&(what[count]<='9')) ||
                   ((what[count]>='A')&&(what[count]<='F')) ) ) {
			temp[subcount]=what[count];
			subcount++;
			count++;
		}
		temp[subcount]='\0';
		sprintf(buf,"%u",hextodec(temp));
		strcat(result,buf);
	  }
	  else {
	    little[0]=what[count];
	    strcat(result,little);
	    count++;
	  }
	}
	}
	return(result);
}
void memory_error(void)
{
	printf("\nerror: can't allocate memory!\n");
	exit(0);
}
