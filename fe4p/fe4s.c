/*
   $Id: fe4s.c,v 1.3 2001/08/27 22:20:23 j10 Exp $
   Script scanner
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include <sif.h>

typedef struct StringTable
{
   int len;
   int count;
   char *string;
} StringTable;

int nstrings = 0;
StringTable *tab = NULL;

int InsertString(const char *string)
{
   int i;

   for(i = 0; i < (int)strlen(string); i++) {
      if(!isascii(string[i])) return -1;
   }

   for(i = 0; i < nstrings; i++) {
      if(strcmp(string,tab[i].string) == 0) {
         tab[i].count++;
         return 0;
      }
   }
   tab = realloc(tab,sizeof(StringTable) * (nstrings + 1));
   if(!tab) {
      fprintf(stderr,"Out of memory\n");
      exit(1);
   }
   tab[nstrings].len = strlen(string);
   tab[nstrings].count = 1;
   tab[nstrings].string = (char *)malloc(strlen(string) + 1);
   if(!tab[nstrings].string) {
      fprintf(stderr,"Out of memory\n");
      exit(1);
   }
   strcpy(tab[nstrings].string,string);
   nstrings++;

   return 0;
}

int sorter(const StringTable *a,const StringTable *b)
{
   return (a->count > b->count ? -1 :
            (a->count < b->count ? 1 :
               (a->len > b->len ? -1 :
                  (a->len < b->len ? 1 :
                     0))));
}

// skips any white space
void SkipWS(FILE *script)
{
   int ch;

   ch = getc(script);
   while((isspace(ch) || ch == '#' || ch == '@') && !feof(script)) {
      if(ch == '#' || ch == '@') {
         while(ch != '\n' && !feof(script)) ch = getc(script);
      }
//      if(ch == '\n') linecount++;
      ch = getc(script);
   }
   ungetc(ch,script);
}

// gets the next token, returns true if it is parser directive
int getNext(char *buf,FILE *script)
{
	int i, ch;

	SkipWS(script);

   ch = getc(script);
   for(i = 0; i < 0x3F && !isspace(ch); i++) {
      if(ch == '[' && i == 0) {
			ch = getc(script);
			while(ch != ']' && ch != EOF) {
				buf[i++] = ch;
				ch = getc(script);
			}
			buf[i] = 0;
			return 1;
		} else if(ch == '[') {
			ungetc(ch,script);
			break;
		}
		buf[i] = ch;
		ch = getc(script);
/*       if(ispunct(ch)) { */
/*          ungetc(ch,script); */
/*          break; */
/*       } */
//      if(ch == '\n') linecount++;
	}
	buf[i] = 0;

	return 0;
}

char buffer[1024];
void ScanFile(const char *filename)
{
   FILE *fp;

   fp = fopen(filename,"r");
   if(!fp) {
      fprintf(stderr,"File '%s' not found\n",filename);
      exit(1);
   }

   while(!feof(fp)) {
      if(getNext(buffer,fp) == 0) {
         if(feof(fp)) return;
         if(strlen(buffer) >= 2) InsertString(buffer);
      }
   }
   fclose(fp);
}

const char *translate(const char *string)
{
   static char buffer[1024];
   int j;

   for(j = 0; j < (int)strlen(string); j++) {
      if(isupper(string[j])) buffer[j] = string[j] - 'A' + 0x10;
      else if(islower(string[j])) buffer[j] = string[j] - 'a' + 0x2A;
      else if(isdigit(string[j]) && string[j] != '0') buffer[j] = string[j] - '0' + 0xB1;
      else {
         switch(string[j]) {
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
               fprintf(stderr,"Warning: Non-convertable character `%c' found in script.\n",string[j]);
               break;
         }
      }
   }
   return buffer;
}

int main(int argc,char *argv[])
{
   int i;
   FILE *out, *sif = NULL;
   char outfn[256] = "-", siffn[256] = {0};
   long t1 = 0, t2 = 0, t3 = 0, t4 = 0;
   int c;
   SIF_HEADER h;
   SIF_BLOCK_HEADER sh;
   SIF_SECTION_HEADER sc;
   const char strtab[] = "table1\0table2\0table3\0table4";

   if(argc <= 2) {
      fprintf(stderr,"Usage: fe4s [options...][files...]\n");
      fprintf(stderr,"Options:\n");
      fprintf(stderr,"-o filename             Table output\n");
      fprintf(stderr,"-s filename             Sif output\n");
      fprintf(stderr,"-1 addr                 Table 1 start address \n");
      fprintf(stderr,"-2 addr                 Table 2 start address \n");
      fprintf(stderr,"-3 addr                 Table 3 start address \n");
      fprintf(stderr,"-4 addr                 Table 4 start address \n");
      return 1;
   }

   while((c = getopt(argc,argv,"o:s:1:2:3:4:")) > 0) {
      switch(c) {
         case 'o':
            strcpy(outfn,optarg);
            break;
         case 's':
            strcpy(siffn,optarg);
            break;
         case '1':
            t1 = strtol(optarg,NULL,16);
            break;
         case '2':
            t2 = strtol(optarg,NULL,16);
            break;
         case '3':
            t3 = strtol(optarg,NULL,16);
            break;
         case '4':
            t4 = strtol(optarg,NULL,16);
            break;
         default:
            fprintf(stderr,"Invalid option '%c'.\n",c);
            return 1;
      }
   }

   for(i = optind; i < argc; i++) {
      ScanFile(argv[i]);
   }

   if(*outfn == '-') {
      out = stdout;
   } else {
      if(strncmp(outfn + strlen(outfn) - 4,".tbl",4)) {
         fprintf(stderr,"Output file '%s' does not have a .tbl extension.\n",outfn);
         return 1;
      }
      out = fopen(outfn,"w");
      if(!out) {
         fprintf(stderr,"Error, disk full?\n");
         exit(1);
      }
   }

   if(*siffn) {
      sif = fopen(siffn,"wb");
      if(!sif) {
         fclose(out);
         fprintf(stderr,"Error, disk full?\n");
         exit(1);
      }
   }

   memset(&h,0,sizeof(h));
   h.magic = SIF_MAGIC;
   h.version = SIF_VERSION;
   h.time = time(NULL);
   h.strptr = sizeof(h);
   h.scnptr = h.strptr + sizeof(strtab) + 4;
   h.blkptr = h.scnptr + sizeof(SIF_SECTION_HEADER) * 4 + 4;

   if(sif) {
      long a;
      fwrite(&h,sizeof(h),1,sif);
      a = sizeof(strtab);
      fwrite(&a,4,1,sif);
      fwrite(strtab,sizeof(strtab),1,sif);
      a = 4;
      fwrite(&a,4,1,sif);
      sc.scn = 4;
      sc.offset = 0;
      fwrite(&sc,sizeof(SIF_SECTION_HEADER),1,sif);
      sc.scn = 5;
      sc.offset = 7;
      fwrite(&sc,sizeof(SIF_SECTION_HEADER),1,sif);
      sc.scn = 6;
      sc.offset = 14;
      fwrite(&sc,sizeof(SIF_SECTION_HEADER),1,sif);
      sc.scn = 7;
      sc.offset = 21;
      fwrite(&sc,sizeof(SIF_SECTION_HEADER),1,sif);
   }

   qsort(tab,nstrings,sizeof(StringTable),(int (*)(const void *,const void *))sorter);
   for(i = 0; i < 256 && i < nstrings; i++) {
      memset(&sh,0,sizeof(sh));
      sh.flag.section = 4;
      sh.flag.use16 = 1;
      sh.address = t1;
      sh.size = strlen(tab[i].string) + 2;
      t1 += 2;
      if(sif) {
         fwrite(&sh,SIF_BLOCK_HEADER_SIZE,1,sif);
         fwrite(translate(tab[i].string),strlen(tab[i].string),1,sif);
         putc(0xBF,sif);
         putc(0xFF,sif);
      }
      fprintf(out,"FB%.2X=%s\n",i,tab[i].string);
   }
   for(i = 256; i < 512 && i < nstrings; i++) {
      memset(&sh,0,sizeof(sh));
      sh.flag.section = 5;
      sh.flag.use16 = 1;
      sh.address = t2;
      sh.size = strlen(tab[i].string) + 2;
      t2 += 2;
      if(sif) {
         fwrite(&sh,SIF_BLOCK_HEADER_SIZE,1,sif);
         fwrite(translate(tab[i].string),strlen(tab[i].string),1,sif);
         putc(0xBF,sif);
         putc(0xFF,sif);
      }
      fprintf(out,"FC%.2X=%s\n",i-256,tab[i].string);
   }
   for(i = 512; i < 768 && i < nstrings; i++) {
      memset(&sh,0,sizeof(sh));
      sh.flag.section = 6;
      sh.flag.use16 = 1;
      sh.address = t3;
      sh.size = strlen(tab[i].string) + 2;
      t3 += 2;
      if(sif) {
         fwrite(&sh,SIF_BLOCK_HEADER_SIZE,1,sif);
         fwrite(translate(tab[i].string),strlen(tab[i].string),1,sif);
         putc(0xBF,sif);
         putc(0xFF,sif);
      }
      fprintf(out,"FD%.2X=%s\n",i-512,tab[i].string);
   }
   for(i = 768; i < 1024 && i < nstrings; i++) {
      memset(&sh,0,sizeof(sh));
      sh.flag.section = 7;
      sh.flag.use16 = 1;
      sh.address = t4;
      sh.size = strlen(tab[i].string) + 2;
      t4 += 2;
      if(sif) {
         fwrite(&sh,SIF_BLOCK_HEADER_SIZE,1,sif);
         fwrite(translate(tab[i].string),strlen(tab[i].string),1,sif);
         putc(0xBF,sif);
         putc(0xFF,sif);
      }
      fprintf(out,"FE%.2X=%s\n",i-768,tab[i].string);
   }

   if(sif) {
      memset(&sh,0,sizeof(sh));
      sh.flag.eof = 1;
      fwrite(&sh,SIF_BLOCK_HEADER_SIZE,1,sif);
      fclose(sif);
   }
   if(out != stdout) fclose(out);

   return 0;
}
