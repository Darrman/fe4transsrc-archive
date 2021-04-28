/***     65816 Cross Assembler v2.0 (c)1994 Jeremy Gordon     ***/


#include <stdio.h>
#include "machine.h"

main(int argc,char **argv)
{
	FILE *fp,*fp2;
	char buf,buf2;
	int count=0;

	if(argc<3)  {
	  printf("use:\n\t%s <file1> <file2>\n",argv[0]);
	  return(1);
	}

	if((fp=fopen(argv[1],"rb"))==NULL) {
	  printf("can't open file '%s'\n",argv[1]);
	  exit(1);
	}
	if((fp2=fopen(argv[2],"rb"))==NULL) {
	  printf("can't open file '%s'\n",argv[2]);
	  exit(1);
	}
	while(!feof(fp)) {
	  fread(&buf,1,1,fp);
	  fread(&buf2,1,1,fp2);
	  if(buf!=buf2)
	    printf("$%s(%d) difference $%s(%d) and $%s(%d)\n",dectohex(count,6),count,dectohex(buf,2),buf,dectohex(buf2,2),buf2);
	  count++;
	}
	fclose(fp);
	fclose(fp2);
}
