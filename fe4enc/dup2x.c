/*
   $Id: dup2x.c,v 1.1 2001/07/02 05:28:26 j10 Exp $
   Method 5: Attemps to detect data in the sequence of aa aa bb bb cc cc ...
*/

/*
 * dup2x_analyse:
 * Analyses the data for duplicated 2x bytes
 * Returns number of pairs found
 */
int dup2x_analyse(unsigned char *readahead,int len)
{
   int i;

   for(i = 0;i < 32 && i < len-1;i += 2) {
      if(readahead[i] != readahead[i+1]) break;
   }

   return i >> 1;
}
