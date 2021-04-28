/*
   $Id: append.c,v 1.1 2001/07/02 05:28:26 j10 Exp $
   Methods 6 & 7: Compresses using the Zero Pre/Post Append method
*/

/*
 * preappend_analyse:
 * Analyses the data for preappended repetition byte
 * Returns number of matches
 */
int preappend_analyse(unsigned char *readahead,int len)
{
   int i;
   unsigned char match;

   match = *readahead;
   for(i = 0;i < 17 && i < len-1;i++) {
      if(readahead[i<<1] != match) break;
   }

   return i < 2 ? 0 : i;
}

/*
 * postappend_analyse:
 * Analyses the data for postappended repetition byte
 * Returns number of matches
 */
int postappend_analyse(unsigned char *readahead,int len)
{
   int i;
   unsigned char match;

   match = readahead[1];
   for(i = 0;i < 17 && i < len-1;i++) {
      if(readahead[(i<<1) + 1] != match) break;
   }

   return i < 2 ? 0 : i;
}
