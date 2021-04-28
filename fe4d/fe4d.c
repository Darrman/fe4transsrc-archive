/*
   $Id: fe4d.c,v 1.7 2002/06/23 03:16:53 j10 Exp $
   Fire Emblem 4 Script Dumper
   Compiled with GCC
   (c)1999-2001 Jay2E
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Typedef basic types */
typedef char int8;
typedef unsigned char uint8;
typedef short int16;
typedef unsigned short uint16;
typedef int int32;
typedef unsigned int uint32;
typedef long long int64;
typedef unsigned long long uint64;

// Enter the character string table here
const char * const fechar[] =
{
	/*00*/NULL,
	/*01*/"Sigurd", "Noish", "Alec", "Adan", "Finn", 
	/*06*/"Cuan", "Mideel", "Levin", "Holin", "Azel", "Jamuka", 
	/*0C*/"Claude", "Beowulf", "Lex", "Deu", "Diadora", "Ethrin", 
	/*12*/"Laches", "Ayra", "Fury", "Tiltyu", "Sylvia", "Edin", 
	/*18*/"Bridget", "Selis", "Shanan", "Rodol", "Asaello", "Leif", 
	/*1E*/"Johann", "Charlo", "Hawk", "Tristan", "Finn", "Dimuna", 
	/*24*/"Hannibal", "Ares", "Amid", "Oifaye", "Daisy", "Rodney", 
	/*2A*/"Mana", "Yuria", "Altena", "Femina", "Linda", "Leilia", 
	/*30*/"Jeanne", "Jolaver", "Sukasa", "Faval", "Couple", "Sety", 
	/*36*/"Delmud", "Lester", "Arthur", "Patty", "Lakche", "Ranna", 
	/*3C*/"Fee", "Tinny", "Lynn", "Nanna", "Gerard", "Demagio", 
	/*42*/"Verdane", "Verdane", "Verdane", "Verdane", "Verdane", "Verdane", 
	/*48*/"Kinbois", "Verdane", "Verdane", "Verdane", "Verdane", "Gandolf", 
	/*4E*/"Captain", "Verdane", "Verdane", "Verdane", "Sandima", "Verdane", 
	/*54*/"Verdane", "Captain", "Verdane", "Bandit", "Elliot", "Hiline", 
	/*5A*/"Eltosan", "CrossKn", "CrossKn", "Boldo", "Hiline", "Phillip", 
	/*60*/"Hiline", "Hiline", "Hiline", "Hiline", "Hiline", "Hiline", 
	/*66*/"Elliot", "Hiline", "Hiline", "Hiline", "Captain", "Amphony", 
	/*6C*/"Bandit", "Mackily", "Iv", "Macbeth", "Amphony", "Amphony", 
	/*72*/"Amphony", "Voltz", "Mercin", "Clement", "MacKily", "MacKily", 
	/*78*/"MacKily", "Captain", "MacKilly", "Shagall", "Agsti", "Agsti", 
	/*7E*/"Captain", "Agsti", "Captain", "Agsti", "FurySqd", "Zain", 
	/*84*/"Agsti", "Agsti", "Agsti", "Agsti", "Eva", "Alva", 
	/*8A*/"Shagobn", "Mardino", "Mardino", "Mardino", "Mardino", "Mardino", 
	/*90*/"Captain", "Mardino", "Captain", "Mardino", "Captain", "Mardino", 
	/*96*/"Captain", "Mardino", "Captain", "Mardino", "Captain", "Mardino", 
	/*9C*/"Pirate", "Captain", "Seabell", "Eltosan", "CrossKn", "Shagall",
	/*A2*/"Seabell", "Seabell", "Papilon", "Thracan", "Pirate", "Pirate", 
	/*A8*/"Pirate", "Pirate", "Doball", "Pirate", "Pisar", "Pirate", 
	/*AE*/"Pirate", "Pirate", "Pirate", "Arvis", "Lanbard", "G Ritter", 
	/*B4*/"Leptor", "G Ritter", "Manya", "Silesian", "Maios", "Thove", 
	/*BA*/"Thove", "Captain", "Thove", "Kenbuli", "Thove", "Captain", 
	/*C0*/"Thove", "Thove", "Detvar", "DetvarS", "Pirate", "Silesan", 
	/*C6*/"Dakkar", "Manya", "ManyaS", "Pamela", "PamelaS", "Andrei", 
	/*CC*/"WeisRit", "Civilian", "Civilian", "Donovan", "Saxon", "Saxon", 
	/*D2*/"Saxon", "Saxon", "Saxon", "Leimia", "Mercin", "Mercin", 
	/*D8*/"Mercin", "Mercin", "Mercin", "Lanbard", "Child", "Lubeck", 
	/*DE*/"Slader", "Lubeck", "Captain", "Lubeck", "Lubeck", "Bandit", 
	/*E4*/"Captain", "Lubeck", "Lubeck", "Lubeck", "Byron", "Andrei", 
	/*EA*/"WeisRit", "Vaha", "Verthom", "Priest", "Verthom", "Torabant", 
	/*F0*/"Magone", "Thracan", "Lenstar", "Aieda", "Verthom", "Leptor", 
	/*F6*/"Freigan", "Freigan", "Freigan", "Freigan", "Freigan", "Freigan", 
	/*FC*/"Freigan", "Freigan", "Captain", "RotRit", "Captain", "RotRit", 
	/*102*/"Captain", "RotRit", "Arvis", "Captain", "RotRit", "Harold", 
	/*108*/"Ganish", "Ganish", "Bandit", "Sophar", "Isacc", "Isacc", 
	/*10E*/"Sophar", "Shumit", "Riboh Solider", "Danan", "Ganish", "Ganish", 
	/*114*/"Cotozov", "YeidMag", "YeidMag", "Mercin", "YeidMag", "Captain", 
	/*11A*/"Alster", "Alster", "Bandit", "Blume", "Alster", "Darna", 
	/*120*/"Ishuto", "Melgnan", "Laiza", "Melgnan", "Melgnan", "Melgnan", 
	/*126*/"Melgnan", "Melgnan", "Melgnan", "Jabarro", "Mercin", "Mercin", 
	/*12C*/"Bramzel", "Darna", "Captain", "Alster", "Alster", "Vampa", 
	/*132*/"Fetora", "Elliu", "HighMag", "HighMag", "HighMag", "Muhumed", 
	/*138*/"Connote", "Connote", "Minister", "Minister", "Ovoh", "Connote", 
	/*13E*/"Connote", "Bandit", "Captain", "Connote", "Ishtar", "Captain", 
	/*144*/"Connote", "Blume", "Connote", "Civilian", "Civilian", "Captain", 
	/*14A*/"Thracan", "Coluda", "Thracan", "Captain", "Thracan", "Captain", 
	/*150*/"Thracan", "Mikoff", "Vampa", "Fetora", "Elliu", "Capetan", 
	/*156*/"Capetan", "Capetan", "Capetan", "Thracan", "Thracan", "Bandit",
	/*15C*/"Kannatz", "Disler", "Gurutan", "Mercin", "Mercin", "Torbant", 
	/*162*/"Thracan", "Judah", "Captain", "Gurutan", "Arion", "DragonK", 
	/*168*/"DragonK", "Captain", "Thracan", "Captain", "Thracan", "Captain", 
	/*16E*/"Thracan", "Musar", "Royal", "Royal", "Royal", "Royal", 
	/*174*/"Lideel", "LideelS", "LideelS", "LideelS", "LideelS", "LideelS", 
	/*17A*/"Morigan", "Dark Mage", "Pirate", "Hilda", "DPriest", "DarkMag", 
	/*180*/"DarkMag", "Mercin", "Mercin", "Child", "Child", "Zagam", 
	/*186*/"DarkMag", "DarkMag", "DarkMag", "DarkMag", "DarkMag", "Mercin", 
	/*18C*/"Mercin", "Mercin", "Yurius", "Ishtar", "Arvis", "Rot Rit", 
	/*192*/"Rot Rit", "Rot Rit", "DarkMag", "Palmark", "Child", "Child", 
	/*198*/"Captain", "Rot Rit", "Rot Rit", "Captain", "Rot Rit", "Rot Rit", 
	/*19E*/"Captain", "Rot Rit", "Rot Rit", "Rot Rit", "DarkMag", "DarkMag", 
	/*1A4*/"Roberto", "Mercin", "Mercin", "Mercin", "Mercin", "Mercin", 
	/*1AA*/"Boyce", "Mercin", "Mercin", "Rodan", "Eddanr", "Eddanr", 
	/*1B0*/"Eddanr", "Eddanr", "Yupheel", "DarkMag", "DarkMag", "Fisher", 
	/*1B6*/"Dozel", "Dozel", "Brian", "GrauRit", "GrauRit", "Dagoban", 
	/*1BC*/"Scorpio", "WeisRit", "WeisRit", "Hilda", "GelpRit", "GelpRit", 
	/*1C2*/"DPriest", "DarkMag", "Manfroi", "DarkMag", "Yurius", "Eins", 
	/*1C8*/"Zwei", "Drei", "Vier", "Funf", "Sechs", "Seiben", 
	/*1CE*/"Aght", "Neun", "Zehn", "Elf", "Zwolf", "Arion", 
	/*1D4*/"DragonK", "Ishtar", "Meng", "Maybell", "Bleg", "Baharan", 
	/*1DA*/"Baharan", "Baharan", "Baharan", "Balan", "Cotozov", "Bandit", 
	/*1E0*/"Gazak", "Clotho", "Weissman", "Shark", "Hood", "Bacchus", 
	/*1E6*/"Emmile", "Darez", "Jirou", "Mahatma", "Lovin", "Backnun", 
	/*1EC*/"Milley", "Hertzog", "Chekov", "Keimos", "Marilyn", "Taylor", 
	/*1F2*/"Ripp", "Perio", "Trevic", "Gellar", "Bazan", "Duma", 
	/*1F8*/"Keller", "Torton", "Mario", "Kemall", "Karedin", "Keith", 
	/*1FE*/"Sangor", "Nikita", "Nene", "Kraff", "Nikias", "Atlas", 
	/*204*/"Jackal", "Ripp", "Greias", "Geese", "Lee", "Shishel", 
	/*20A*/"Hawks", "Nazar", "Theif", "Tolstoy", "Botman", "Kulyuge", 
	/*210*/"Mansten", "Kashim", "Hepner", "Randok", "Volf", "Duey",
	/*216*/"Fet", "Marshak", "Luis", "Heste", "Madah", "Jismunt", 
	/*21C*/"Zenon", "Jakson", "Indra", "Neizche", "Apostle", "Millia", 
	/*222*/"Siron", "Grein", "Slayton", "Torstar", "Kanan", "Hasman", 
	/*228*/"Grantz", "Krosloy", "Rolan", "Mashgli", "Riva", "Gloria", 
	/*22E*/"Nothe", "Indra", "Dares", "Keimos", "Marilyn", "Zenon", 
	/*234*/"Jakson", "Torton", "Mario", "Kanan", "Atlas", "Zeus", 
	/*23A*/"Folsety", NULL, NULL, NULL, NULL, NULL, 
	/*240*/NULL, NULL, NULL, NULL, NULL, NULL, 
	/*246*/NULL, NULL, NULL, NULL, NULL, NULL, 
	/*24C*/NULL, NULL, NULL, NULL, NULL, NULL, 
	/*252*/NULL, NULL, NULL, NULL, NULL, NULL, 
	/*258*/NULL, NULL, NULL, NULL, NULL, NULL, 
	/*25E*/NULL, NULL, NULL, NULL, NULL, NULL, 
	/*264*/NULL, NULL, NULL, NULL, NULL, NULL, 
	/*26A*/NULL, NULL, NULL, NULL, NULL, NULL, 
	/*270*/NULL, NULL, NULL, NULL, NULL, NULL, 
	/*276*/NULL, NULL, NULL, NULL, NULL, NULL, 
	/*27C*/NULL, NULL, NULL, "Oifaye", NULL, NULL, 
	/*282*/NULL, NULL, NULL, NULL, NULL, NULL, 
	/*288*/NULL, NULL, NULL, NULL, NULL, NULL, 
	/*28E*/NULL, NULL, NULL, NULL, NULL, NULL, 
	/*294*/NULL, NULL, NULL, NULL, NULL, NULL, 
	/*29A*/NULL, NULL, NULL, NULL, NULL, NULL, 
	/*2A0*/NULL, NULL, NULL, NULL, NULL, NULL, 
	/*2A6*/NULL, NULL, NULL, NULL, NULL, NULL, 
	/*2AC*/NULL, NULL, NULL, NULL, NULL, NULL, 
	/*2B2*/NULL, NULL, NULL, NULL, NULL, NULL, 
	/*2B8*/NULL, NULL, NULL, NULL, NULL, NULL, 
	/*2BE*/NULL, NULL, NULL, NULL, NULL, NULL, 
	/*2C4*/NULL, NULL, NULL, NULL, NULL, NULL, 
	/*2CA*/NULL, NULL, NULL, NULL, NULL, NULL,
	/*2D0*/NULL, NULL, NULL, NULL, NULL, NULL, 
	/*2D6*/NULL, NULL, NULL, NULL, NULL, NULL, 
	/*2DC*/NULL, NULL, NULL, NULL, NULL, NULL, 
	/*2E2*/NULL, NULL, NULL, NULL, NULL, NULL, 
	/*2E8*/NULL, NULL, NULL, NULL, NULL, NULL, 
	/*2EE*/NULL, NULL, NULL, NULL, NULL, NULL, 
	/*2F4*/NULL, NULL, NULL, NULL, NULL, NULL, 
	/*2FA*/NULL, NULL, NULL, NULL, NULL, NULL 
};

#include "evil.c"

// 24-bit struct to hold a snes address
typedef struct snes_addr
{
	uint16 offset	__attribute__((packed));
	uint8  bank		__attribute__((packed));
} snes_addr;

// converts an fe mapped snes address to valid pc address
inline uint32 snestopc(struct snes_addr a)
{
	uint32 pcbank, pcoff;

	pcbank = a.bank - (a.bank < 0xc0 ? 0x80 : 0xc0);
	pcoff = a.offset;

	return ((pcbank << 16) | pcoff) + 0x200;
}

unsigned int hash[1000] = {0};

// hash increment function
inline int PINC(unsigned int value)
{
   return (value & 31) + 1;
}

// hash function
inline int HASH(unsigned int value)
{
   return value % 1000;
}

int InsertHash(unsigned int value)
{
   unsigned int h, p, maxtrav;

   h = HASH(value);
   p = PINC(value);

   if(!hash[h]) {
      hash[h] = value;
      return 1;
   }

   maxtrav = 1000 / p;

   while(maxtrav) {
      h += p;
      if(h >= 1000) h %= 1000;
      if(!hash[h]) {
         hash[h] = value;
         return 1;
      }
      maxtrav--;
   }

   return 0;
}

int FindHash(unsigned int value)
{
   unsigned int h, p, maxtrav;

   h = HASH(value);
   p = PINC(value);

   if(!hash[h]) return 0;

   if(hash[h] == value) return 1;

   maxtrav = 1000 / p;

   while(maxtrav) {
      h += p;
      if(h >= 1000) h %= 1000;
      if(!hash[h]) break;
      if(hash[h] == value) return 1;
      maxtrav--;
   }

   return 0;
}

int FindDup(uint32 value)
{
   if(FindHash(value)) return 1;
   if(!InsertHash(value)) fprintf(stderr,"Warning: Hash overflow.\n");
   return 0;
}

uint32 MergePPT(uint32 *ppt,uint8 *rom,uint32 offset,int n)
{
   int i;
   snes_addr a;

   for(i = 0;i < n;i++)  {
      a.offset = *(uint16 *)(rom + ppt[i]);
		a.bank = *(uint8 *)(rom + ppt[i] + 2);
      if(offset == snestopc(a)) return ppt[i];
   }
   return 0;
}

int main(int argc,char **argv)
{
	FILE *fp;
	uint8 *rom, *euc; // pointer to memory of rom and euc
	uint32 *ppt;  // pointer to pointer table
	uint32 romsize, offset;
   int bits16 = 0;
	int ch, pos, jmp, i, nsec, cnt, rem;
	snes_addr pt; // a snes pointer
	int sptop = -1, spbot = -1, sp = 0; // speaker top and bottom

	if(argc < 4 || argc > 5) {
		fprintf(stderr,"Usage: fe4dump <fe4.smc> <fe4.euc> <pttable.pt> [16] [> output.txt]\n\n");
		return 1;
	}

   printf("# Fire Emblem 4 Script Dump\n");
   printf("# %s\n\n",argv[3]);

   if(argc == 5) {
      if(argv[4][0] == '1' && argv[4][1] == '6') {
         printf("[bits16]\n");
         bits16 = 1;
      }
   }

	fp = fopen(argv[1],"rb");
	if(!fp) {
		fprintf(stderr,"`%s' not found.\n\n",argv[1]);
		return 1;
	}

	// get file size
   fseek(fp,0,SEEK_END);
	romsize = ftell(fp);
   fseek(fp,0,SEEK_SET);
	printf("# Filesize: %d bytes\n\n",romsize);

	rom = (uint8 *)malloc(romsize);
	if(!rom) {
		fprintf(stderr,"There is not enough memory.\n\n");
		fclose(fp);
		return 1;
	}

	// read to memory
	fread(rom,1,romsize,fp);
	fclose(fp);

	// open EUC table
	fp = fopen(argv[2],"rb");
	if(!fp) {
		free(rom);
		fprintf(stderr,"`%s' not found.\n\n",argv[2]);
		return 1;
	}

	/* allocate memory for EUC, 3168 should be enough for 6 banks
	 * because 256 characters times 2 bytes per charcter, added with
	 * 8*2 for the new lines at the end of each line. And we multiply
	 * by 6 banks. So,
	 * ( 256 x 2 + 8 x 2 ) x 6 = 3168 bytes
    */
	euc = (uint8 *)malloc(3168);
	if(!euc) {
		free(rom);
		fclose(fp);
		fprintf(stderr,"There is not enough memory.\n\n");
		return 1;
	}

	// clear it to spaces
	memset(euc,' ',3168);
	fread(euc,1,3168,fp); // read
	fclose(fp);

	// open the pointer to pointer table
	fp = fopen(argv[3],"rb");
	if(!fp) {
		free(rom); free(euc);
		fprintf(stderr,"`%s' not found.\n\n",argv[3]);
		return 1;
	}

   fseek(fp,0,SEEK_END);
	nsec = ftell(fp) / 4;
   fseek(fp,0,SEEK_SET);

	ppt = (uint32 *)malloc(nsec * sizeof(uint32));
	if(!ppt) {
		fclose(fp); free(rom); free(euc);
		fprintf(stderr,"There is not enough memory.\n\n");
		return 1;
	}

	// read in pointer to pointer table
	fread(ppt,sizeof(uint32),nsec,fp);
	fclose(fp);

	for(i = 0;i < nsec;i++) {
		// get snes pointer using the ppt table
		pt.offset = *(uint16 *)(rom + ppt[i]);
		if(!bits16) pt.bank = *(uint8 *)(rom + ppt[i] + 2);
      else pt.bank = (ppt[i] >> 16) + 0x80;

		offset = snestopc(pt); // convert to valid pc address
		jmp = 0;

		printf("# 0x%x:\n",offset);
      printf("# @@%d@@\n",ppt[i]);

      if(FindDup(offset)) printf("[Merge %x]\n",MergePPT(ppt,rom,offset,nsec));

		while(offset < romsize) {
			ch = rom[offset++]; // get hex
			if(ch == 0x00) { // if is evil 00 character
				ch = rom[offset++]; // fetch next byte
				/*if(ch < 0x10) {
					if(ch < 0x9) printf("[$00]");
					goto breakifloop; // is special code, handle as normal
				}*/

				switch(ch) {
					case 0x12:	printf("[TextSpd %.2X]",rom[offset]);
									offset++;
									break;
					case 0x13:	printf("[BGM %.2X]",rom[offset]);
									offset++;
									break;
					case 0x17:	printf("[ck]\n"); break;
					case 0x18:	printf("[kk]\n"); break;
					case 0x1C:	for(cnt=0;cnt<rom[offset];cnt++) printf(" ");
									offset++;
									break;
               case 0x29:  printf("[dcs]\n"); break;
					case 0x1D:	printf("[Wait %.2X]",rom[offset]);
									offset++;
									break;
					case 0x3D:	printf("[DlgOpn]"); break;
					case 0x3E:	printf("[DlgCls]"); break;
					case 0x3F:	printf("[\'%s\' %.4X]",fechar[*(uint16 *)(rom + offset)],*(uint16 *)(rom + offset));
									if(sp) sptop = *(uint16 *)(rom + offset);
                           else spbot = *(uint16 *)(rom + offset);
									offset += 2;
									break;
					case 0x40:	printf("[xChr]"); if(sp) sptop = -1; else spbot = -1; break;
					default:    printf("[$00$%.2X",ch);
									for(rem=0;rem<evil[ch];rem++) {
										printf("$%.2X",rom[offset]);
										offset++;
									}
									printf("]");
									break;
				}
				continue;
			}
//breakifloop:
			switch(ch) {
				case 0x0:	printf("\n"); break;
				case 0x1:	printf("\n[exit]\n"); sptop = spbot = -1; goto breakloop;
				case 0x2:	printf("\n"); break;
				case 0x3:	printf("[c]\n"); break;
				case 0x4:	printf("[cs]\n"); break;
				case 0x5:	printf("\n[Targetwin %.2X]\n",rom[offset]);
								offset++;
								break;
				case 0x6:	printf("\n[TWinTop]"); sp = 1; if(sptop >= 0) printf(" # %s\n",fechar[sptop]); break;
				case 0x7:	printf("\n[TWinBtm]"); sp = 0; if(spbot >= 0) printf(" # %s\n",fechar[spbot]); break;
				case 0x8:	printf("[k]\n"); break;
				case 0x9:	jmp = 0; break;
				case 0xA:   jmp = 0x210; break;
				case 0xB:	jmp = 0x420; break;
				case 0xC:	jmp = 0x630; break;
				case 0xD:	jmp = 0x840; break;
				case 0xE:
				case 0xF:	jmp = 0xA50; break;
				default:		pos = jmp + ((ch * 2) + ((ch / 32) * 2));
								printf("%c%c",euc[pos],euc[pos+1]);
								break;
			}
		}
breakloop:
		printf("\n\n");
	}
	free(rom);
	free(euc);
	free(ppt);

   return 0;
}
