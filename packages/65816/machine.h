#ifndef __machine_
#define __machine_

typedef unsigned char 	byte;
typedef unsigned short	word;
typedef unsigned int	address;

#define OFF 0				/* offset for jml padding */

char *dectohex(address, byte); 		/* must dealloc space when done */
address  hextodec(char *);		/* must dealloc space when done */
void incrLCBIN(address *,address);
void setHiRom(int);
char *convertHex(char *); 		/* must dealloc space when done */
void memory_error(void);
#endif


