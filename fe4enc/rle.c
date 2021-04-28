/*
   $Id: rle.c,v 1.1 2001/07/02 05:28:26 j10 Exp $
   RLE-Length Encoding
*/

/*
 * rle_analyse:
 * Analyses the buffer for RLE patterns
 * Returns the length of repeated bytes
 */
int rle_analyse(unsigned char *readahead,int len)
{
   int i;

   for(i = 0;i < 10 && i < len;i++) {
      if(*readahead != readahead[i]) break;
   }

   return i < 3 ? 0 : i;
}
