/***     65816 Cross Assembler v2.0 (c)1994 Jeremy Gordon     ***/


#include <stdio.h>
#include <string.h>
#include "label.h"
#include "smc.h"

/* this relies on info from Norman Yen's trasm.p */

int makeSMCFile(char *infname,char *outfname,smcHeader header,word org)
{
	FILE *in,*out;
	char *buf=(char*)malloc(32768);
	int size,length,current,temp;
	address value;
        if(!buf) memory_error();


	memset(buf,0,512);
	if((in=fopen(infname,"rb"))==NULL) {
	  printf("error: can't open output file '%s'\n",infname);
	  free(buf);
	  return(1); 
	}
	if((out=fopen(outfname,"wb"))==NULL) {
	  printf("error: can't open smc file '%s'\n",outfname);
	  free(buf);
	  return(1);
	}
        fseek(in,0,2);
        size=(int)ftell(in);
        fseek(in,0,0);
	length=(size-(size%8192))/8192;
	if(size%8192)
	 length++;
	if(length<4)
	 length=4;
	buf[0]=(0xff&length); 		/* the low byte of the size */
	buf[1]=(0xff&(length>>8));	/* the high byte of the size */
	buf[2]=0;
	buf[8]=0xaa;
	buf[9]=0xbb;
 	buf[10]=0x04;
	fwrite(buf,512,1,out);
	memset(buf,0,32768);

        if(size<32768)
           current=size;
        else
           current=32768;
        size=size-current;
        fread(buf,current,1,in);



	if(header.title)
	 strcpy((char *)(buf+0x8000-0x40),header.title);
	else
	 strcpy((char *)(buf+0x8000-0x40),"65816(c)jgordon");
	temp=size*8;
	length=(temp-(temp%1048576))/1048576;
	if(length<=4)
	  buf[0x8000-0x40+0x17]=0x09;
	else if(length<=8)
	  buf[0x8000-0x40+0x17]=0x0a;
	else
	  buf[0x8000-0x40+0x17]=0x0b;

	if(labelDefined("_COUNTRY",&value))
	   header.country=value;
	else
	   header.country=0x01;
	if(labelDefined("_VERSION",&value))
	   header.version=value;
	else
	   header.version=0;

	if(labelDefined("_RESET",&value)) {
	   header.native_reset=value;
	   header.emulation_reset=value;
	}
	else {
          if(labelDefined("N_RESET",&value))
           header.native_reset=value;
          else
           header.native_reset=org;
          if(labelDefined("E_RESET",&value))
           header.emulation_reset=value;
          else
           header.emulation_reset=org;
	}

        if(labelDefined("_COPROCESSOR",&value)) {
           header.native_coprocessor=value;
	   header.emulation_coprocessor=value;
	}
        else {
          if(labelDefined("N_COPROCESSOR",&value))
           header.native_coprocessor=value;
          else
           header.native_coprocessor=0;
          if(labelDefined("E_COPROCESSOR",&value))
           header.emulation_coprocessor=value;
          else
           header.emulation_coprocessor=0;
	}

        if(labelDefined("_BREAK",&value)) {
           header.native_break=value;
	   header.emulation_break=value;
	}
        else {
	  if(labelDefined("N_BREAK",&value)) 
	   header.native_break=value;
	  else
           header.native_break=0;
	  if(labelDefined("E_BREAK",&value))
	   header.emulation_break=value;
	  else
	   header.emulation_break=0;
	}

        if(labelDefined("_ABORT",&value)) {
           header.native_abort=value;
	   header.emulation_abort=value;
	}
        else {
	  if(labelDefined("N_ABORT",&value))
	   header.native_abort=value;
	  else
           header.native_abort=0;
	  if(labelDefined("E_ABORT",&value))
	   header.emulation_abort=value;
  	  else
	   header.emulation_abort=0;
	}

        if(labelDefined("_NMI",&value)) {
           header.native_NMI=value;
	   header.emulation_NMI=value;
	}
        else {
	  if(labelDefined("N_NMI",&value))
           header.native_NMI=value;
	  else
	   header.native_NMI=0;
	  if(labelDefined("E_NMI",&value))
	   header.emulation_NMI=value;
	  else
	   header.emulation_NMI=0;
	}

        if(labelDefined("_IRQ",&value)) {
           header.native_irq=value;
	   header.emulation_break=value;
	}
        else {
          if(labelDefined("N_IRQ",&value))
           header.native_irq=value;
          else
           header.native_irq=0;
          if(labelDefined("E_IRQ",&value))
           header.emulation_break=value;
          else
           header.emulation_break=0;
	}

	
	buf[0x8000-0x40+0x19]=header.country;
	buf[0x8000-0x40+0x1b]=header.version;

        if(buf[0x8000-0x40+0x24]|buf[0x8000-0x40+0x25])
          printf("warning: code present at native coprocessor vector($%s)...not overwritten.\n",
                dectohex(buf[0x8000-0x40+0x24]+(buf[0x8000-0x40+0x25]<<8),4));
        else {
	  buf[0x8000-0x40+0x24]=header.native_coprocessor&0xff;
	  buf[0x8000-0x40+0x25]=(header.native_coprocessor>>8)&0xff;
	}
        if(buf[0x8000-0x40+0x26]|buf[0x8000-0x40+0x27])
          printf("warning: code present at native break vector($%s)...not overwritten.\n",
                dectohex(buf[0x8000-0x40+0x26]+(buf[0x8000-0x40+0x27]<<8),4));
        else {
	  buf[0x8000-0x40+0x26]=header.native_break&0xff;
	  buf[0x8000-0x40+0x27]=(header.native_break>>8)&0xff;
	}
        if(buf[0x8000-0x40+0x28]|buf[0x8000-0x40+0x29])
          printf("warning: code present at native abort vector($%s)...not overwritten.\n",
                dectohex(buf[0x8000-0x40+0x28]+(buf[0x8000-0x40+0x29]<<8),4));
        else {
	  buf[0x8000-0x40+0x28]=header.native_abort&0xff;
	  buf[0x8000-0x40+0x29]=(header.native_abort>>8)&0xff;
	}
	if(buf[0x8000-0x40+0x2a]|buf[0x8000-0x40+0x2b]) 
          printf("warning: code present at native nmi vector($%s)...not overwritten.\n",
                dectohex(buf[0x8000-0x40+0x2a]+(buf[0x8000-0x40+0x2b]<<8),4));
	else {
	  buf[0x8000-0x40+0x2a]=header.native_NMI&0xff;
	  buf[0x8000-0x40+0x2b]=(header.native_NMI>>8)&0xff;
	}
        if(buf[0x8000-0x40+0x2e]|buf[0x8000-0x40+0x2f])
          printf("warning: code present at native irq vector($%s)...not overwritten.\n",
                dectohex(buf[0x8000-0x40+0x2e]+(buf[0x8000-0x40+0x2f]<<8),4));
        else {
	  buf[0x8000-0x40+0x2e]=header.native_irq&0xff;
	  buf[0x8000-0x40+0x2f]=(header.native_irq>>8)&0xff;
	}
        if(buf[0x8000-0x40+0x34]|buf[0x8000-0x40+0x35])
          printf("warning: code present at emulation coprocessor vector($%s)...not overwritten.\n",
                dectohex(buf[0x8000-0x40+0x34]+(buf[0x8000-0x40+0x35]<<8),4));
        else {
          buf[0x8000-0x40+0x34]=header.emulation_coprocessor&0xff;
          buf[0x8000-0x40+0x35]=(header.emulation_coprocessor>>8)&0xff;
	}
        if(buf[0x8000-0x40+0x38]|buf[0x8000-0x40+0x39])
          printf("warning: code present at emulation abort vector($%s)...not overwritten.\n",
                dectohex(buf[0x8000-0x40+0x38]+(buf[0x8000-0x40+0x39]<<8),4));
        else {
          buf[0x8000-0x40+0x38]=header.emulation_abort&0xff;
          buf[0x8000-0x40+0x39]=(header.emulation_abort>>8)&0xff;
	}
	if(buf[0x8000-0x40+0x3a]|buf[0x8000-0x40+0x3b]) 
          printf("warning: code present at emulation nmi vector($%s)...not overwritten.\n",
                dectohex(buf[0x8000-0x40+0x3a]+(buf[0x8000-0x40+0x3b]<<8),4));
	else {
          buf[0x8000-0x40+0x3a]=header.emulation_NMI&0xff;
          buf[0x8000-0x40+0x3b]=(header.emulation_NMI>>8)&0xff;
	}
	if(buf[0x8000-0x40+0x3c]|buf[0x8000-0x40+0x3d])
	  printf("warning: code present at reset vector($%s)...not overwritten.\n",
		dectohex(buf[0x8000-0x40+0x3c]+(buf[0x8000-0x40+0x3d]<<8),4));
	else {
         buf[0x8000-0x40+0x3c]=header.emulation_reset&0xff;
         buf[0x8000-0x40+0x3d]=(header.emulation_reset>>8)&0xff;
	}
        if(buf[0x8000-0x40+0x3e]|buf[0x8000-0x40+0x3f])
          printf("warning: code present at emulation break/irq vector($%s)...not overwritten.\n",
                dectohex(buf[0x8000-0x40+0x3e]+(buf[0x8000-0x40+0x3f]<<8),4));
        else {
         buf[0x8000-0x40+0x3e]=header.emulation_break&0xff;
         buf[0x8000-0x40+0x3f]=(header.emulation_break>>8)&0xff;
	}
          if(fwrite(buf,32768,1,out)!=1) {
            printf("error: writing to smc file '%s'\n",outfname);
            free(buf);
            return(1);
          }


	do {
          if(size<32768)
            current=size;
          else
            current=32768;
          memset(buf,0,32768);
	  if(current!=0) {
           fread(buf,current,1,in);
           if(fwrite(buf,32768,1,out)!=1) {
            printf("error: writing to smc file '%s'\n",outfname);
            free(buf);
            return(1);
           }
	  }
	  size=size-current;
	} while(size!=0);
	fclose(in);
	fclose(out);
	free(buf);
	return(0);
}
