/*
   $Id: fe4cv.c,v 1.3 2004/04/05 09:59:13 j10 Exp $

   FE4 Character Viewer!!!!
   Compiled with SDL
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SDL.h"

#define byte unsigned char
#define word unsigned short
#define dword unsigned long

byte *data; // pointer to character data
short *palette; // pointer to array of palettes
int chrnum = 0; // current character
#define DATSIZE 0x80000 // define size of data

void DrawTile8x8_4bpp(int x,int y,byte *ptr,short *pal);
void DrawFE4Chr(int x,int y,int chr);

int snapindex = 0;

SDL_Surface *screen;

int main(int argc,char **argv)
{
   int quit = 0, i, j, chr, ch;
   FILE *fp;
   SDL_Event event;

   printf("FE4 Character Viewer!\n");
   printf("Compiled with GCC, SDL\n\n");

   if(argc != 3) {
      printf("Usage: fechrvw <char.dat> <palette.cg>\n\n");
      return 1;
   }

   printf("Reading file to memory...");
   fflush(stdout);

   // open character dat file
   fp = fopen(argv[1],"rb");
   if(!fp) {
      printf("Can't find file `%s'.\n",argv[1]);
      return 1;
   }

   // allocate memory for character dat
   data = (byte *)malloc(DATSIZE);
   if(!data) {
      fclose(fp);
      printf("Not enough memory!!\n");
      return 1;
   }

   // read data into memory
   fread(data,1,DATSIZE,fp);
   fclose(fp);

   printf("OK\nReading CG Palette...");
   fflush(stdout);

   // open character palette file
   fp = fopen(argv[2],"rb");
   if(!fp) {
      free(data);
      printf("Can't find file `%s'.\n",argv[2]);
      return 1;
   }

   palette = (short *)malloc(256*sizeof(short)*16);
   if(!palette) {
      free(data);
      fclose(fp);
      printf("Not enough memory!!\n");
      return 1;
   }

   // read the palette
   fread(palette,256,sizeof(short)*16,fp);
   fclose(fp);

   // SNES BGR -> RGB reordering
   for(i=0;i<256*16;i++) {
      ch = palette[i];
      palette[i] = ((ch & 0x1F) << 11) | ((ch & 0x3E0) << 1) | ((ch & 0x7C00) >> 10);
      //palette[i] = RGB16(GETB15(ch),GETG15(ch),GETR15(ch));
   }

   ch = 0;
   printf("OK\n");

   // Initialize the graphics mode
   if(SDL_Init(SDL_INIT_VIDEO) == -1) {
      fprintf(stderr,"Could not initialize SDL: %s.\n", SDL_GetError());
      exit(1);
   }
   atexit(SDL_Quit);

   screen = SDL_SetVideoMode(320,200,16,SDL_SWSURFACE);
   if(screen == NULL) {
      fprintf(stderr,"Couldn't set video mode to 320x200x16.\n");
      exit(1);
   }

   while(!quit) { // while not ESC key
      chr = chrnum;

      if(SDL_MUSTLOCK(screen)) {
         if(SDL_LockSurface(screen) < 0) {
            fprintf(stderr,"Can't lock surface: %s\n",SDL_GetError());
            exit(1);
         }
      }

      for(i=0;i<3;i++) {
         for(j=0;j<6;j++) {
            // draw the character to screen
            DrawFE4Chr((j<<2)+(j<<1),i<<3,chr);
            chr++;
         }
      }

      // update screen
      if(SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);
      SDL_UpdateRect(screen,0,0,0,0);
      if(SDL_PollEvent(&event)) {
         do {
            switch(event.type) {
               case SDL_KEYDOWN:
                  switch(event.key.keysym.sym) {
                     case SDLK_UP         : chrnum -= 6; break;
                     case SDLK_DOWN       : chrnum += 6; break;
                     case SDLK_LEFT       : chrnum--; break;
                     case SDLK_RIGHT      : chrnum++; break;
                     case SDLK_PAGEUP     : chrnum -= 18; break;
                     case SDLK_PAGEDOWN   : chrnum += 18; break;
                     case SDLK_HOME       : chrnum = 0; break;
                     case SDLK_ESCAPE     : quit = 1; break;
                     default              : break;
                  }
                  break;
               case SDL_QUIT:
                  quit = 1;
                  break;
            }
         } while(SDL_PollEvent(&event));
      }

      if(chrnum < 0) chrnum = 0;
      if(chrnum > 0xF7-18) chrnum = 0xF7-18;
   }

   free(data);
   free(palette);
   printf("Programmed by JAY!!\n");

   return 0;
}

/*
   void DrawTile8x8_4bpp(int x,int y,byte *ptr,short *pal);
   Draws 1 8x8 4BitPlane tile at an x,y coordinate.
*/
void DrawTile8x8_4bpp(int x,int y,byte *ptr,short *pal)
{
   asm volatile (
      "movl %3, %%esi;"
      "movl %4, %%edi;"
      "movl $8, %%ecx;"

      "movl %1, %%eax;"     	// eax = y
      "shll $3, %%eax;"     	// left shift y by 3 to get tile coordinates
      "movl %%eax, %%edx;"
      "shll $9, %%eax;"     	// do somes shifts to multiply by 640
      "shll $7, %%edx;"
      "addl %%edx, %%eax;"
      "movl %0, %%edx;"     	// edx = x
      "shll $4, %%edx;"    	// calculate x coordinate in tiles
      "addl %%edx, %%eax;"
      "addl %%eax, %%edi;" 	// update edi to screen position of (x,y)

      "xorl %%ebx, %%ebx;"
"0:"
      "movl $8, %%edx;"      // edx = loop counter
      "movw 16(%%esi), %%ax;"// transfer hi data to ax
      "shll $16, %%eax;"     // transfer to hi end of eax
      "movw (%%esi), %%ax;"  // get lo data
"1:"
      "pushl %%eax;"
      "andb $1, %%al;"       // reorganize data from bitplanes to a byte
      "andb $1, %%ah;"
      "shlb $1, %%ah;"
      "orb  %%ah, %%al;"
      "movb %%al, %%bl;"
      "rorl $16, %%eax;"
      "andb $1, %%al;"
      "andb $1, %%ah;"
      "shlb $2, %%al;"
      "shlb $3, %%ah;"
      "orb %%ah, %%al;"
      "orb %%al, %%bl;"

      "pushl %%ecx;"
      "movl %2, %%ecx;"      // ecx = palette

      "movw (%%ecx,%%ebx,2), %%ax;" 	// move colour from palette
      "movw %%ax, -2(%%edi,%%edx,2);" // put pixel with colour to the screen

      "popl %%ecx;"
"2:"
      "popl %%eax;"
      "shrw $1, %%ax;"       // shift 1 bit over for next pixel
      "rorl $16, %%eax;"
      "shrw $1, %%ax;"
      "rorl $16, %%eax;"
      "decl %%edx;"          // decrements edx
      "jnz 1b;"

      "addl $2, %%esi;"
      "addl $640, %%edi;"
      "decl %%ecx;"
      "jnz 0b;"
      :
      : "m" (x), "m" (y), "m" (pal), "m" (ptr), "m" (screen->pixels)
      : "%ebx", "%ecx", "%edx", "%edi", "%esi", "memory"
   );
}

/*
   void DrawFE4Chr(int x,int y,int chr);
   Draws a character from the dat file to the screen
   Characters are 6x8 tiles; does not draw the character mouths even
   though the mouths were included in the dat file.
*/
void DrawFE4Chr(int x,int y,int chr)
{
   register int rindex = 0;
   register int tx;
   register int ty;

   for(ty = 0;ty < 8;ty++) {
      for(tx = 0;tx < 6;tx++) {
         // Draw the tile
         DrawTile8x8_4bpp(tx+x,ty+y,(data+(chr<<11)+rindex),&palette[chr<<4]);
         rindex += 32;
      }
      rindex += 320;
      if(ty == 3) rindex = 192;
   }
}
