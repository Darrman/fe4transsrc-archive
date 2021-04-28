/*
   $Id: fe4p.c,v 1.11 2002/02/05 05:13:54 j10 Exp $
   Fe4 Script Parser
*/

/* Last modified: 22/08/04
   This file was modified by Dark Twilkitri */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include <sif.h>
#include <tbl.h>

typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned int dword;

int WINDOWLIMIT = 225;
int ROWLIMIT    = 3;

#define false 0
#define true  1
#define swap(x) (((x & 0xFF) << 8) | ((x & 0xFF00) >> 8))

char SECTION[256] = "0";
int MAP = 2;
int FLAGS = 0;

FILE *script;
SifWriter *sif;
byte *block;
dword blockcnt;
int linecount = 1;
TBLINFO tbl = NULL;
char ofname[256];
 
int exitcode = 0;

/*
unsigned char widths[256] = {
	0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	0x06, 0x03, 0x08, 0x08, 0x08, 0x08, 0x08, 0x03, 0x06, 0x06, 0x08, 0x08, 0x03, 0x09, 0x03, 0x08,
	0x09, 0x05, 0x07, 0x07, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x08, 0x08, 0x0C, 0x08, 0x08, 0x0A,
	0x08, 0x09, 0x08, 0x08, 0x08, 0x07, 0x07, 0x08, 0x08, 0x05, 0x06, 0x08, 0x07, 0x09, 0x09, 0x09,
	0x08, 0x09, 0x08, 0x08, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x08, 0x08, 0x08, 0x08, 0x08,
	0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x07, 0x08, 0x08, 0x03, 0x05, 0x07, 0x03, 0x09, 0x08, 0x08,
	0x08, 0x08, 0x08, 0x07, 0x06, 0x08, 0x07, 0x09, 0x07, 0x07, 0x07, 0x06, 0x08, 0x06, 0x08, 0x08,
	0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08
} ;
*/
#include "widths.c"

unsigned short sjistbl[256] =
{
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x82D3, 0x0000, 0x0000, 0x0000, 0x0000, 0x82D0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x815B, 0x0000, 0x0000,
	0x824F, 0x8250, 0x8251, 0x8252, 0x8253, 0x8254, 0x8255, 0x8256, 0x8257, 0x8258, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x82A0, 0x82A2, 0x82A4, 0x82A6, 0x82A8, 0x82A9, 0x82AB, 0x82AD, 0x82AF, 0x82B1, 0x82B3, 0x82B5, 0x82B7, 0x82B9, 0x82BB,
	0x82BD, 0x82BF, 0x82C2, 0x82C4, 0x82C6, 0x82C8, 0x82C9, 0x82CA, 0x82CB, 0x82CC, 0x82CD, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x8341, 0x8343, 0x8345, 0x8347, 0x8349, 0x834A, 0x834C, 0x834E, 0x8350, 0x8352, 0x8354, 0x8356, 0x8358, 0x835A, 0x835C,
	0x835E, 0x8360, 0x8363, 0x8365, 0x8367, 0x8369, 0x836A, 0x836B, 0x836C, 0x836D, 0x836E, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
};

void SkipWS();
int getNext(char *buf);
void parse();
void parseSJIS();
void parsenotext();
int pixellength(unsigned char *string);

int main(int argc,char *argv[])
{
  const char *sectn[2];
   char scname[256];
   int c, sn, mn;
   char pound;

	if(argc < 2) {
		printf("Usage: fe4p [options..] <script.txt>\n");
      printf("Options:\n");
      printf("-o filename           Send output to filename\n");
      printf("-i filename           Include table\n");
		return 1;
	}

   if(strlen(argv[optind]) >= 256) {
      fprintf(stderr,"%s: File name too long.\n",argv[optind]);
      return 1;
   }

   strcpy(ofname,argv[optind]);
	ofname[strlen(argv[optind])-3] = 0;
	strcat(ofname,"sif");
   
   while((c = getopt(argc,argv,"o:i:")) > 0) {
      switch(c) {
         case 'o':
            strcpy(ofname,optarg);
            break;
         case 'i':
            tbl = TBL_LoadTable(optarg);
            if(!tbl) {
               fprintf(stderr,"%s\n",TBL_GetErrorString());
               return 1;
            }
            break;
         default:
            fprintf(stderr,"Invalid option '%c'.\n",c);
            return 1;
      }
   }

	block = (byte *)malloc(65536);
	if(!block) {
		fprintf(stderr,"fe4p: Out of memory.\n");
		return 1;
	}

	script = fopen(argv[optind],"r");
	if(!script) {
		fprintf(stderr,"%s: File not found.\n",argv[optind]);
		free(block);
		return 1;
	}

   if(fscanf(script,"%c %d %d %s",&pound,&sn,&mn,scname) == 4 && pound == '#') {
      sectn[0] = scname;
      sectn[1] = NULL;
      sif = CreateSifWriter(ofname,sectn,"fe4map.so");
   } else {
      sif = CreateSifWriter(ofname,NULL,"fe4map.so");
   }

   if(!sif) {
      fprintf(stderr,"%s: Could not open file.\n",ofname);
      free(block); fclose(script);
      return 1;
   }

   fseek(script,0L,SEEK_SET);

	parse();

   TBL_CloseTable(tbl);
	fclose(script);
	CloseSifWriter(sif);
	free(block);

	return exitcode;
}

// skips any white space
void SkipWS()
{
   int ch, ch2;

   ch = getc(script);
top:
   while((isspace(ch) || ch == '#') && !feof(script)) {
      if(ch == '#') {
         ch2 = ch;
         while(ch != '\n' && !feof(script)) {
            ch = getc(script);
            if(ch == '@' && ch2 == '@') {
               fseek(script,-2L,SEEK_CUR);
               return;
            } else if(ch2 == '*' && ch == '#') { /* uncomment tag */
               ch = getc(script);
               goto top;
            }
            ch2 = ch;
         }
      }
      if(ch == '\n') linecount++;
      ch = getc(script);
   }
   ungetc(ch,script);
}

// gets the next token, returns true if it is parser directive
int getNext(char *buf)
{
	int i, ch;

	SkipWS();

   ch = getc(script);
   for(i = 0; i < 0x3F && !isspace(ch); i++) {
		if(ch == '[' && i == 0) {
			ch = getc(script);
			while(ch != ']' && ch != EOF) {
				buf[i++] = ch;
            if(i > 0x3F) {
               fprintf(stderr,"Warning %d: Directive string exceeds maximum length.\n",linecount);
               //exitcode = 1;
               return false;
            }
				ch = getc(script);
			}
			buf[i] = 0;
			return true;
		} else if(ch == '[') {
			ungetc(ch,script);
			break;
		}
		buf[i] = ch;
		ch = getc(script);
      if(ch == '\n') linecount++;
	}
	buf[i] = 0;

	return false;
}

void parse()
{
   int spaces = 1, blocked = 0, curblock, tarblock;
   int forcewipe = 0;
   byte buffer[64];
   char code[64];
   int colt, rowt, rowb, colb, twin;
   int i, j, param;
   unsigned long val;
   long ppt = 0;
   FILE *bpr, *bls;

#define SWITCH(x) if(false) {
#define CASE(str) } else if(!strcmp(code,str)) {
#define ENDSWITCH }
#define emit(val) block[blockcnt++] = (val)
#define emitw(val) *(word *)(block + blockcnt) = (val), blockcnt += 2
#define emitParamB() sscanf((char *)(buffer + strlen(code)),"%X",&param), emit(param);
#define row (twin ? rowb : rowt)
#define col (twin ? colb : colt)

   while(!feof(script)) {
looptop:
      blockcnt = 0;
      twin = 0;
      colt = rowt = rowb = colb = 0;
      while(!feof(script)) {
         if(getNext((char *)buffer)) {
            if(feof(script)) return;
#ifdef DEBUG
            printf("%s\n",buffer);
            getch();
#endif
            sscanf((char *)buffer,"%s",code);
            SWITCH(buffer)
               CASE("exit")
                  emit(0x01);
                  break;
               CASE("EOF")
                  return;
               CASE("n")
                  if(row >= ROWLIMIT-1) {
                     emit(0x08);
                     row = 0;
                  }
                  emit(0x02);
                  col = 0; row++;
               CASE("sp")
                  *buffer = 0;
                  goto emittext;
               CASE("c")
                  emit(0x03);
                  col = row = 0;
               CASE("cs")
                  emit(0x04);
                  col = row = 0;
               CASE("Targetwin")
                  emit(0x05);
                  emitParamB();
                  twin = (param & 1);
               CASE("TWinTop")
                  emit(0x06);
                  twin = 0;
                  row = 0;
               CASE("TWinBtm")
                  emit(0x07);
                  twin = 1;
                  row = 0;
               CASE("k")
                  emit(0x08);
               CASE("TextSpd")
                  emitw(0x1200);
                  emitParamB();
               CASE("BGM")
                  emitw(0x1300);
                  emitParamB();
               CASE("ck")
                  emitw(0x1700);
                  col = row = 0;
               CASE("kk")
                  emitw(0x1800);
               CASE("dcs")
                  emitw(0x2900);
                  colb = rowb = rowt = colt = 0;
               CASE("Wait")
                  emitw(0x1D00);
                  emitParamB();
               CASE("DlgOpn")
                  emitw(0x3D00);
               CASE("DlgCls")
                  emitw(0x3E00);
               CASE("xChr")
                  emitw(0x4000);
               CASE("winlimit")
                  sscanf((char *)(buffer + strlen(code)),"%d",&WINDOWLIMIT);
                  if(!WINDOWLIMIT) WINDOWLIMIT = 225;
               CASE("rowlimit")
                  sscanf((char *)(buffer + strlen(code)),"%d",&ROWLIMIT);
                  if(!ROWLIMIT) ROWLIMIT = 3;
               CASE("reloc")
                  sscanf((char *)(buffer + strlen(code)),"%lX",&ppt);
               CASE("ignore") // ignore block
                  while(!feof(script)) {
                     param = getNext((char *)buffer);
                     if(param && !strcmp(buffer,"exit")) { i++; goto looptop; }
                  }
               CASE("section");
                  sscanf((char *)(buffer + strlen(code)),"%s",SECTION);
               CASE("map")
                  sscanf((char *)(buffer + strlen(code)),"%d",&MAP);
               CASE("bits16")
                  FLAGS = FLAGS | SIF_USE16;
	       CASE("ASMPTR")
	          FLAGS = FLAGS | SIF_ASM;
	       CASE("ASMPTR3")
	          FLAGS = FLAGS | SIF_ASM3;
	       CASE("nocompress")
	          /* some scripts don't like compression, and we don't want
		  to decompress the entire lot of them */
	          if(tbl)
		  {
	             TBL_CloseTable(tbl);
		     tbl = NULL;
		  }
	       CASE("noptr")
	          /* for scripts with no pointers */
		  FLAGS = FLAGS | SIF_NOPTR;
	       CASE("blocked")
	          /* for scripts in Block Format */
		  FLAGS = FLAGS | SIF_BLOCK;
		  blocked = 1;
		  //create Block Preliminary file
		  ofname[strlen(ofname)-3] = 0;
		  strcat(ofname,"bpr");
		  bpr = fopen(ofname, "wb");
		  //create Block Listing file
		  ofname[strlen(ofname)-3] = 0;
		  strcat(ofname,"bls");
		  bls = fopen(ofname, "wb");
		  //assume they were created properly
	       CASE("block")
	          /* This is the block number, store it in the block listing */
		  sscanf((char*)(buffer + strlen(code)), " b%u", &curblock);
		  fprintf(bls, "%04u\n", curblock);
	       CASE("resolve")
	          /* Store this in the Block Preliminary */
		  //caller
		  fprintf(bpr, "%04u|", curblock);
		  //target
		  sscanf((char*)(buffer + strlen(code)), " b%u", &tarblock);
		  fprintf(bpr, "%04u|", tarblock);
		  //emit 2A code
		  emit(0x00);
		  emit(0x2A);
		  //offset
		  fprintf(bpr, "%02u%02u%02u\n", (blockcnt & 0xFF0000) >> 16, (blockcnt & 0x00FF00) >> 8, blockcnt & 0x0000FF);
		  //dummy address
		  emit(0xFF);
		  emit(0xFF);
		  emit(0xFF);
	       CASE("nospaces")
	          /* for things we don't want spaces after */
		  spaces = 0;
	       CASE("respaces")
	          /* to turn them back on in the same script */
		  spaces = 1;
	       CASE("nomerge")
	          /* turn off merging */
		  FLAGS = FLAGS | SIF_NOMERGE;
               CASE("mode-sjis")
                  /* cheap hack to invoke a second parser in the same program */
                  parseSJIS();
                  return;
	       CASE("mode-notext")
	          /* for if we're not altering text
		     technically, a script inserter shouldn't have to worry
		     about this, but where else am I going to put it? */
		  parsenotext();
		  return;
	       CASE("forcewipe")
	          forcewipe = 1;
               CASE("Merge")
                  ;
            } else if(code[0] == '\'') {
               for(j = 2; buffer[j] != '\'' && buffer[j]; j++);
               sscanf(&buffer[j + 1],"%X",&param);
               emitw(0x3F00);
               emitw(param);
               if(col || row) printf("Warning %d: No clear screen code used prior to character change.\n",linecount);
            } else if(code[0] == '$') {
	       if(buffer[3] == '$') /* check for 2+ bytes */
	       {
	          if((buffer[4] == '2') && ((buffer[5] == '2') || (buffer[5] == '3') || (buffer[5] == '8') || (buffer[5] == 'C'))) /* let's format the dynamic names a bit better, shall we? */
		  {
		     col += 80;  /* totally arbitrary; I have no idea how long they are (should be) */
		     if(col >= WINDOWLIMIT)
		     {
		        row++;
			if(row >= ROWLIMIT)
			{
			   emit(0x08);
            	           if(forcewipe)
	                   {
	                      emit(0x04);
		              col = 0;
	                   }
	                   else
                              emit(0x02);
			   row = 0;
			}
			else
			{
			   emit(0x02);
			}
			col = 80;
		     }
		  }
	       }
               for(j = 0; buffer[j] && buffer[j] == '$'; j += 3) {
                  sscanf(&buffer[j + 1],"%2X",&param);
                  emit(param);
               }
            } else {
               fprintf(stderr,"Warning %d: Unknown directive `%s'. Skipping.\n",linecount,code);
            }
            continue;
         } // end if
emittext:
         if(feof(script)) return;

         if(buffer[0] == '@' && buffer[1] == '@') {
            ppt = strtol(buffer+2,NULL,10);
            continue;
         }
#ifdef DEBUG
            printf("%s\n",buffer);
            getch();
#endif
         col += pixellength(buffer);
         if(col >= WINDOWLIMIT) {
            row++;
            if(row >= ROWLIMIT) {
               emit(0x08);
	       if(forcewipe)
	       {
	          emit(0x04);
		  col = 0;
	       }
	       else
                  emit(0x02);
               row = 0;
            } else {
               emit(0x02);
            }
            col = pixellength(buffer);
         }

         if(tbl) {
            if(TBL_GetValue(tbl,buffer,&val,NULL)) {
               emit((val >> 8) & 0xFF);
               emit(val & 0xFF);
               continue;
            }
         }

         for(j = 0; j < (int)strlen((char *)buffer); j++) {
				if(!isascii(buffer[j])) {
               fprintf(stderr,"Warning %d: Jap characters found in script! Skipping block.\n",linecount);
					while(!feof(script)) {
						param = getNext((char *)buffer);
						if(param && !strcmp((char *)buffer,"exit")) {
   						i++;
//							if(i >= (int)nPointers) return;
							goto looptop;
						}
					}
				} else if(isupper(buffer[j])) buffer[j] -= 'A' - 0x10;
				else if(islower(buffer[j])) buffer[j] -= 'a' - 0x2A;
				else if(isdigit(buffer[j]) && buffer[j] != '0') buffer[j] -= '0' - 0xB1;
				else {
					switch(buffer[j]) {
						case '0': buffer[j] = 0xB1; break;
						case '.': buffer[j] = 0xBC; break;
						case ',': buffer[j] = 0xBB; break;
                  case '?': buffer[j] = 0xBE; break;
                  case '!': buffer[j] = 0xBD; break;
                  case '\'': buffer[j] = 0xC5; break;
                  case '-': buffer[j] = 0xB0; break;
                  case '{': buffer[j] = 0xC0; break;
                  case '}': buffer[j] = 0xC1; break;
                  case '(': buffer[j] = 0xC2; break;
                  case ')': buffer[j] = 0xC3; break;
                  case '<': buffer[j] = 0xC4; break;
                  default :
                     fprintf(stderr,"Error %d: Non-convertable character `%c' found in script.\n",linecount,buffer[j]);
                     exitcode = 1;
                     break;
               }
            }
         }

         memmove(block + blockcnt,buffer,strlen((char *)buffer));
         blockcnt += strlen((char *)buffer);
	 if(spaces)
	 {
            emit(0xBF);
	 }
      } // end of while

      if(isdigit(*SECTION)) WriteSifBlock(sif,block,blockcnt,ppt,(const char *)strtol(SECTION,NULL,10),MAP,FLAGS);
      else WriteSifBlock(sif,block,blockcnt,ppt,SECTION,MAP,FLAGS);
   } // end of for
   
   //clean up
   if(blocked)
   {
     fclose(bls);
     fclose(bpr);
   }
}

void parseSJIS()
{
   byte buffer[64];
   char code[64];
   int i, j, param;
   long addr = 0;

   while(!feof(script)) {
looptop:
      blockcnt = 0;
      while(!feof(script)) {
         if(getNext((char *)buffer)) {
            if(feof(script)) return;

            sscanf((char *)buffer,"%s",code);
            SWITCH(buffer)
               CASE("exit")
                  emitw(0x00);
                  break;
               CASE("sp")
                  emitw(swap(sjistbl[' ']));
                  break;
               CASE("spx")
	          emitw(swap(sjistbl[' ']));
               } else if(code[0] == '$') {
                  for(j = 0; buffer[j] && buffer[j] == '$'; j += 3) {
                     sscanf(&buffer[j + 1],"%2X",&param);
                     emit(param);
                  }
            ENDSWITCH
            continue;
         } else {
            if(feof(script)) return;

            if(buffer[0] == '@' && buffer[1] == '@') {
               addr = strtol(buffer+2,NULL,10);
               continue;
            }

            for(i = 0; i < 64 && buffer[i]; i++) {
               if(!(isascii(buffer[i]) | isdigit(buffer[i]))) {
                  while(!feof(script)) {
                     if(getNext((char *)buffer) && !strcmp((char *)buffer,"exit")) {
                        goto looptop;
                     }
                  }
                  return;
               }
               emitw(swap(sjistbl[buffer[i]]));
            }
         }
      }
      if(isdigit(*SECTION)) WriteSifBlock(sif,block,blockcnt,addr,(const char *)strtol(SECTION,NULL,10),MAP,FLAGS);
      else WriteSifBlock(sif,block,blockcnt,addr,SECTION,MAP,FLAGS);
   }
}

void parsenotext()
{
   byte buffer[64];
   char code[64];
   long addr = 0;
   byte toinsert;

   while(!feof(script)) {
      blockcnt = 0;
      while(!feof(script)) {
         if(getNext((char *)buffer)) {
            if(feof(script)) return;

            sscanf((char *)buffer,"%s",code);
            SWITCH(buffer)
               CASE("exit")
                  break;
            ENDSWITCH
            continue;
         } else {
            if(feof(script)) return;

            if(buffer[0] == '@' && buffer[1] == '@') {
               addr = strtol(buffer+2,NULL,10);
               continue;
            }

	    toinsert = (byte)strtol(buffer, NULL, 16);
	    emit(toinsert);
         }
      }
      if(isdigit(*SECTION)) WriteSifBlock(sif,block,blockcnt,addr,(const char *)strtol(SECTION,NULL,10),MAP,FLAGS);
      else WriteSifBlock(sif,block,blockcnt,addr,SECTION,MAP,FLAGS);
   }
}

int pixellength(unsigned char *string)
{
   int i;
   int len = widths[' '];

   for(i = 0; i < (int)strlen((char *)string); i++)
      len += widths[string[i]];

   return len;
}
