/* $Id: fe4map.c,v 1.2 2001/08/31 18:41:52 j10 Exp $ */
/* Last modified: 02/05/04
   This file was last modified by Dark Twilkitri
   
   This file is used for determining the addresses for relative pointers. */
int id()
{
   return 0x51F500;
}

unsigned long mapfunc(unsigned long addr,int mapno)
{
   switch(mapno) {
      case 2:
         if(addr & 0x8000)
            return addr + 0x800000;
         else
            return addr + 0xC00000;
      case 3:
         return (addr - 0x3857B + 0x200) & 0xFFFF;
      case 4:
         return (addr - 0x38CB3 + 0x200) & 0xFFFF;
      case 5:
         return (addr - 0x39C59 + 0x200) & 0xFFFF;
      case 6:
         return (addr - 0x3DAE8 + 0x200) & 0xFFFF;
      case 7:
         return (addr - 0x3DE12 + 0x200) & 0xFFFF;
      case 8:
         return (addr - 0x3F717 + 0x200) & 0xFFFF;
      case 9:
         return (addr - 0x3E4E8 + 0x200) & 0xFFFF;
      case 10:
         return addr & 0xFFFF;
   }

   return addr;
}
