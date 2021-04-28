/*
   $Id: fe4rippl.c,v 1.1 2001/07/02 05:28:21 j10 Exp $
	FE4 Palette RIPPER
*/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>

#ifndef O_BINARY
#define O_BINARY 0
#endif

struct snes_addr
{
	unsigned short offset __attribute__((packed));
	unsigned char bank    __attribute__((packed));
} pt[256];
typedef struct snes_addr snes_addr;

unsigned short tb[16];

inline int snestopc(snes_addr a)
{
	return (((a.bank - 0x80) << 16) | a.offset) + 0x200;
}

int main()
{
	int i, fp, out;

	fp = open("fe4.smc",O_BINARY|O_RDONLY);
	if(fp < 0) {
      fprintf(stderr,"fe4.smc: File not found.\n");
      return 1;
   }

	out = open("fe4pal.cg",O_CREAT|O_TRUNC|O_BINARY|O_WRONLY,0666);
	if(fp < 0) {
      fprintf(stderr,"fe4pal.cg: File could not be opened for writing.\n");
      close(fp);
      return 1;
   }

	lseek(fp,0xab9e1,SEEK_SET);
	read(fp,pt,256*3);
	for(i=0;i<256;i++) {
		lseek(fp,snestopc(pt[i]),SEEK_SET);
		read(fp,tb,32);
		write(out,tb,32);
	}

	close(fp);
   close(out);

   return 0;
}
