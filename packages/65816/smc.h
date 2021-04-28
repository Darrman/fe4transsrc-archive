#ifndef __SMC_
#define __SMC_

#include "machine.h"

typedef struct {
	char *title;
	byte country;
	byte version;
	word native_coprocessor;
	word native_break;
	word native_abort;
	word native_NMI;
	word native_irq;
	word native_reset;
	word emulation_coprocessor;
	word emulation_abort;
	word emulation_NMI;
	word emulation_reset;
	word emulation_break;
	} smcHeader;

int makeSMCFile(char *,char *,smcHeader,word);

#endif
