#ifndef __address_
#define __address_

#include <stdio.h>
#include "machine.h"
#include "parser.h"

#define DIRECTIVES "()[]<>#"

#define ABSOLUTE 			0x00
#define ABSOLUTEINDEXEDX 		0x01
#define ABSOLUTEINDEXEDY 		0x02
#define ABSOLUTEINDIRECTINDEXED 	0x03
#define ABSOLUTEINDIRECT 		0x04
#define ABSOLUTEINDIRECTLONG		0x05
#define ABSOLUTELONG			0x06
#define ABSOLUTELONGINDEXEDX		0x07
#define BLOCKMOVE			0x08
#define DIRECTPAGE			0x09
#define DIRECTPAGEINDEXEDX		0x0a
#define DIRECTPAGEINDEXEDY		0x0b	
#define DIRECTPAGEINDEXEDINDIRECTX	0x0c	
#define DIRECTPAGEINDIRECT		0x0d	
#define DIRECTPAGEINDIRECTLONG		0x0e	
#define DIRECTPAGEINDIRECTINDEXEDY	0x0f	
#define DIRECTPAGEINDIRECTLONGINDEXEDY	0x10	
#define IMMEDIATE			0x11	
#define IMMEDIATEW			0x12	
#define IMPLIED				0x13	
#define STACKRELATIVE			0x14	
#define STACKRELATIVEINDIRECTINDEXEDY  	0x15	
#define DIRECTIVE			0xff

int addressDetermineMode(line *,address *,int type);

int search(char *, char *,int, int);


int address_absolute(line *,FILE *,FILE *,int);
int address_absoluteindexedx(line *,FILE *,FILE *,int);
int address_absoluteindexedy(line *,FILE *,FILE *,int);
int address_absoluteindirectindexed(line *,FILE *,FILE *,int);
int address_absoluteindirect(line *,FILE *,FILE *,int);
int address_absoluteindirectlong(line *,FILE *,FILE *,int);
int address_absolutelong(line *,FILE *,FILE *,int);
int address_absolutelongindexedx(line *,FILE *,FILE *,int);
int address_blockmove(line *,FILE *,FILE *,int);
int address_directpage(line *,FILE *,FILE *,int);
int address_directpageindexedx(line *,FILE *,FILE *,int);
int address_directpageindexedy(line *,FILE *,FILE *,int);
int address_directpageindexedindirectx(line *,FILE *,FILE *,int);
int address_directpageindirect(line *,FILE *,FILE *,int);
int address_directpageindirectlong(line *,FILE *,FILE *,int);
int address_directpageindirectindexedy(line *,FILE *,FILE *,int);
int address_directpageindirectlongindexedy(line *,FILE *,FILE *,int);
int address_immediate(line *,FILE *,FILE *,int);
int address_immediatew(line *, FILE *,FILE *,int);
int address_implied(line *,FILE *,FILE *,int);
int address_stackrelative(line *,FILE *,FILE *,int);
int address_stackrelativeindirectindexedy(line *,FILE *,FILE *,int);
int address_directive(line *, FILE *,FILE *,int,options *);

#endif
