/*
   $Id: fe4pg.c,v 1.2 2001/08/26 06:07:17 j10 Exp $
   FE4 PPT Tabel generator
   Takes in a template for scanning, and generates a ppt table
   This program takes in patterns to look for to determing whether a pointer is exists
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

static int hirom = 0;
static int header = 0x200;
static int overlap = 0;
static int pointer80 = 0;

int match_exp(const char *exp,const unsigned char *data,long recursiveoff,long *offset,long *address)
{
   int hex = 0;
   int n = 0;
   int i, brace = 0, with_or = 0, start, end, match;
   char *expr, *p;
   int explen = strlen(exp);

   // duplicate the string
   expr = (char *)malloc(strlen(exp) + 1);
   if(!expr) {
      fprintf(stderr,"Out of memory.\n");
      exit(1);
   }
   strcpy(expr,exp);

   // process all the or's
   p = expr;
   for(i = 0; i < explen; i++) {
      // skip all stuff in parenthesis
      if(expr[i] == '(') {
         brace++;
         i++;
         while(brace) {
            if(expr[i] == '(') brace++;
            else if(expr[i] == ')') brace--;
            else if(expr[i] == 0) goto free_and_exit;
            i++;
         }
      }

      // sees an or
      if(expr[i] == '|') {
         with_or = 1;
         expr[i] = 0; // null it
         match = match_exp(p,data,recursiveoff + n,offset,address); // match it
         if(match) {  // if found match, then return it
            n += match;
            free(expr);
            return n;
         }
         p = &expr[i + 1]; // otherwise, next
      }
   }
   // match the last block
   if(with_or) {
      match = match_exp(p,data,recursiveoff + n,offset,address); // match it
      if(match) {  // if found match, then return it
         n += match;
         free(expr);
         return n;
      }
      goto free_and_exit;
   }

   for(i = 0; i < explen; i++) {
      switch(expr[i]) {
         case '(':
            brace = 1;
            i++;
            start = i;
            while(brace) {
               if(expr[i] == '(') brace++;
               else if(expr[i] == ')') brace--;
               else if(expr[i] == 0) goto free_and_exit;
               i++;
            }
            i--;
            end = i;
            expr[end] = 0;
            match = match_exp(&expr[start],data,recursiveoff + n,offset,address);
            if(!match) goto free_and_exit;
            n += match;
            break;
         case '.':
            n++;
            break;
         case '@':
            if(offset) *offset = recursiveoff + n;
            break;
         case 'l':
            if(address) {
               *address = (*address & 0xFFFFFF00) | data[recursiveoff + n];
               n++;
            }
            break;
         case 'm':
            if(address) {
               *address = (*address & 0xFFFF00FF) | (data[recursiveoff + n] << 8);
               n++;
            }
            break;
         case 'h':
            if(address) {
               *address = (*address & 0xFF00FFFF) | (data[recursiveoff + n] << 16);
               n++;
            }
            break;
         case 'x':
            if(address) {
               *address = (*address & 0x00FFFFFF) | (data[recursiveoff + n] << 24);
               n++;
            }
            break;
         default:
            if(isxdigit(expr[i]) && isxdigit(expr[i + 1])) {
               sscanf(&expr[i],"%2X",&hex);
               if(data[recursiveoff + n] == hex) n++;
               else goto free_and_exit;
               i++;
            }
            break;
      }
   }

   free(expr);
   return n;

free_and_exit:
   free(expr);
   return 0;
}

long snes2pc(long addr)
{
   long pcaddr;

   pcaddr = addr & 0x3FFFFF;
   if(hirom) {
      return pcaddr + header;
   }
   return (((pcaddr & 0xFF0000) >> 1) | (pcaddr & 0x7FFF)) + header;
}

long pc2snes(long addr)
{
   unsigned short offset;
   unsigned char bank;

   if(hirom) {
      offset = addr & 0xFFFF;
      bank = 0xC0 + ((addr >> 16) & 0xFF);
   } else {
      offset = (addr & 0xFFFF) | 0x8000;
      bank = 0x80 + ((addr >> 15) & 0xFF);
   }

   return (bank << 16) | offset;
}

int main(int argc,char *argv[])
{
   long i, start = 0, end = 0, filesize, ppt, addr, pcaddr;
   char exp[256] = {0}, mexp[256] = {0}, ofn[256] = {0};
   int c;
   int match;
   FILE *fp;
   unsigned char *rom;

   if(argc < 2) {
      fprintf(stderr,"Usage: fe4pg [[-r range] [-e exp] [-m exp] [-o output] [-h] [-n] [-l] [-p]] <romfile>\n");
      return 1;
   }

   while((c = getopt(argc,argv,"r:e:m:o:hnlp")) > 0) {
      switch(c) {
         case 'r':   if(optarg[0] == '\"') sscanf(optarg,"\"%lX-%lX\"",&start,&end);
                     else sscanf(optarg,"%lX-%lX",&start,&end);
                     break;
         case 'e':   strncpy(exp,optarg,256);
                     exp[255] = 0;
                     break;
         case 'm':   strncpy(mexp,optarg,256);
                     mexp[255] = 0;
                     break;
         case 'o':   strncpy(ofn,optarg,256);
                     ofn[255] = 0;
                     break;
         case 'h':   hirom = 1;
                     break;
         case 'n':   header = 0;
                     break;
         case 'l':   overlap = 1;
                     break;
         case 'p':   pointer80 = 1;
                     break;
         default:    fprintf(stderr,"Invalid option '%c'\n",c);
                     return 1;
      }
   }

   // load file into memory
   fp = fopen(argv[optind],"rb");
   if(!fp) {
      fprintf(stderr,"%s: File not found.\n",argv[optind]);
      return 1;
   }

   fseek(fp,0L,SEEK_END);
   filesize = ftell(fp);
   fseek(fp,0L,SEEK_SET);

   if(start == 0 && end == 0) end = filesize - 1;

   rom = (unsigned char *)malloc(filesize);
   if(!rom) {
      fprintf(stderr,"Out of memory.\n");
      fclose(fp);
      return 1;
   }
   fread(rom,1,filesize,fp);
   fclose(fp);

   fp = NULL;
   if(*ofn) {
      fp = fopen(ofn,"wb");
      if(!fp) {
         fprintf(stderr,"%s: Can't open file for writing.\n",ofn);
         return 1;
      }
   }

   for(i = start; i <= end; i++) {
      addr = pc2snes(i - header);
      if((match = match_exp(exp,&rom[i],0,&ppt,&addr))) {
         if(pointer80 && addr < 0x800000) continue;
         pcaddr = snes2pc(addr);
         if(!*mexp || match_exp(mexp,&rom[pcaddr],0,NULL,NULL)) {
            if(pcaddr < 0 || pcaddr >= filesize) continue;
            ppt += i;
            if(fp) fwrite(&ppt,4,1,fp);
            else printf("%lX\n",ppt);
            if(!overlap) i += match - 1;
         }
      }
   }

   if(fp) fclose(fp);

   return 0;
}
