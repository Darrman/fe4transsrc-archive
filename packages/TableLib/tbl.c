/*
	Table Loading Library
	(c)2000 Jay
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include "tbl.h"
#include "tbintern.h"

static char TBL_errormessage[80] = "No errors.";

/**
	Set the error message
	@param msg The error message
	@return Numbers of items printed
*/
static int TBL_Error(const char *msg,...)
{
	va_list va;
	int c;

	va_start(va,msg);
	c = vsprintf(TBL_errormessage,msg,va);
	va_end(va);

	return c;
}

/**
	Inserts a value into the HASH table
	@param tbl Pointer to a TBLINFO struct
	@param entry Pointer to the entry to hash
	@return Returns 0 on success, -1 on error
*/
static int TBLAPICALL InsertHash(TBLINFO tbl,TBLENTRY *entry)
{
	unsigned int h, p, max, collide;

	h = hash(entry->value);
	p = hinc(entry->value);

	max = hashsize / p;
	collide = 0;
	do {
		if(!tbl->hv[h]) { /* if there is a space in the table, enter it*/
			tbl->hv[h] = entry;
			if(collide > tbl->nMaxCollisionsV) tbl->nMaxCollisionsV = collide;
			goto hashstr;
		}

		collide++; /* increment collide count */

		h += p; /* hash increment to next */
		if(h >= hashsize) h %= hashsize;
	} while(max--);

	TBL_Error("Internal: Hash overflow.");
	return -1;

hashstr:
	collide = 0;

	h = hashs(entry->string);
	p = hincs(entry->string);

	max = hashsize / p;
	do {
		if(!tbl->hs[h]) { /* enter string on table */
			tbl->hs[h] = entry;
			if(collide > tbl->nMaxCollisionsS) tbl->nMaxCollisionsS = collide;
			return 0;
		}

		collide++;
		h += p; /* hash increment to next */
		if(h >= hashsize) h %= hashsize;
	} while(max--);

	TBL_Error("Internal: Hash overflow.");
	return -1;
}

/**
	Returns a string reporting any errors that occured in on of the operations
 */
const char * TBLAPICALL TBL_GetErrorString(void)
{
	return (const char *)TBL_errormessage;
}

/**
	Sets the size of the entry table.
	The entry table size should generally be 2x the maximum supported entries.
	Default: 2000
	@param size The size of the entry table
*/
void TBLAPICALL TBL_SetEntrySize(unsigned int size)
{
	hashsize = size;
}

/**
	Loads a table file into memory and returns the handle
	@param filename Pointer to the string that contains the filename of the table to load
	@return A handle to a TBLINFO struct requried for TBL operations or NULL if an error occured
	@example
		TBLINFO tblfile;
		tblfile = TBL_LoadTable("mytable.tbl");
*/
TBLINFO TBLAPICALL TBL_LoadTable(const char *filename)
{
	TBLINFO tbl;
	FILE *fp; /* handle to file */
	char buffer[80], hex[10];
	char *p, cntlval;
	int linecount;
	uint32 value;
	int size, len, n;

	/* allocate memory */
	tbl = (TBLINFO)malloc(sizeof(__tag_TBLINFO));
	if(!tbl) {
		TBL_Error("Out of memory.");
		goto TBL_LoadTableError0;
	}
	memset(tbl,0,sizeof(__tag_TBLINFO));

	/* allocate memory for each entries */
	tbl->entry = (TBLENTRY *)malloc(sizeof(TBLENTRY) * hashsize);
	if(!tbl->entry) {
		TBL_Error("Out of memory.");
		goto TBL_LoadTableError1;
	}
	memset(tbl->entry,0,sizeof(TBLENTRY) * hashsize);

	/* allocate memory for hash tables */
	tbl->hv = (TBLENTRY **)malloc(sizeof(TBLENTRY *) * hashsize);
	if(!tbl->hv) {
		TBL_Error("Out of memory.");
		goto TBL_LoadTableError1;
	}
	memset(tbl->hv,0,sizeof(TBLENTRY *) * hashsize);

	tbl->hs = (TBLENTRY **)malloc(sizeof(TBLENTRY *) * hashsize);
	if(!tbl->hs) {
		TBL_Error("Out of memory.");
		goto TBL_LoadTableError1;
	}
	memset(tbl->hs,0,sizeof(TBLENTRY *) * hashsize);

	tbl->nEntries = 0;

	/* open file for reading */
	fp = fopen(filename,"r");
	if(!fp) {
		TBL_Error("%s: File not found.",filename);
		goto TBL_LoadTableError1;
	}

	linecount = 0;

	/* Read one line from file */
	while(fgets(buffer,80,fp) != NULL) {
		linecount++; /* increment line count */

		p = buffer;

		while(isspace(*p) && *p) p++;
		if(!*p) continue; /* if is empty line then continue */

		cntlval = *p;
		if(cntlval == '*' || cntlval == '/') { /* if is a newline value */
			p++;

			/* read values */
			for(n = 0; n < 6 && isxdigit(*p); n++) {
				hex[n] = *p;
				p++;
			}
			hex[n] = 0;

			/* clear to endof line */
			while(*p && isspace(*p)) p++;
			if(*p) {
				TBL_Error("%s: Line %d: Junk after hex value.",filename,linecount);
				goto TBL_LoadTableError2;
			}

			/* convert the values */
			value = strtol(hex,NULL,16);
			size = n / 2;
			switch(cntlval) {
				case '*': p = "\n"; break;
				case '/': p = ""; break;
				default : TBL_Error("%s: Line %d: Internal error occured. Please report bug.",filename,linecount);
							 goto TBL_LoadTableError2;
			}
		} else {
			/* the first token should be a hex digit */
			if(!isxdigit(*p)) {
				TBL_Error("%s: Line %d: Expecting a value here.",filename,linecount);
				goto TBL_LoadTableError2;
			}

			for(n = 0; n < 6 && isxdigit(*p); n++) {
				hex[n] = *p;
				p++;
			}
			hex[n] = 0;

			size = n / 2; /* calculate size by the amount of digits */

			value = strtol(hex,NULL,16); /* convert to value */

			/* scan next; expecting '=' sign */
			if(*p != '=') {
				TBL_Error("%s: Line %d: Expecting '='.",filename,linecount);
				goto TBL_LoadTableError2;
			}

			p++;

			len = strlen(p); /* get length of remaining data */
			if(p[len - 1] == '\n') { /* kill the new line if there is one */
				len--;
				p[len] = 0;
			}
		}

		if(tbl->nEntries >= hashsize) {
			TBL_Error("%s: Table overflow.",filename);
			goto TBL_LoadTableError2;
		}

		/* test for duplicate value definitions */
		if(TBL_GetString(tbl,value,size,NULL)) {
			TBL_Error("%s: Line %d: Duplicate definition for value `%X'.",filename,linecount,value);
			goto TBL_LoadTableError2;
		}

		/* add the entry to the table */
		n = tbl->nEntries;
		tbl->entry[n].value = value;
		tbl->entry[n].size = size;
		tbl->entry[n].string = strdup(p);
		if(!tbl->entry[n].string) {
			TBL_Error("Out of memory.");
			goto TBL_LoadTableError2;
		}

      /* Insert it into the hash table */
		if( InsertHash(tbl,&tbl->entry[n]) ) goto TBL_LoadTableError2;

		tbl->nEntries++;
	}

	fclose(fp);
	return tbl;

TBL_LoadTableError2:
	fclose(fp);
TBL_LoadTableError1:
	TBL_CloseTable(tbl);
TBL_LoadTableError0:
	return NULL;
}

/**
	Closes a table file and frees up all memory used by the table
	@param table A handle to the TBLINFO to close
*/
void TBLAPICALL TBL_CloseTable(TBLINFO table)
{
	int i;

	if(table) {
		if(table->hs) free(table->hs);
		if(table->hv) free(table->hv);
		if(table->entry) {
			for(i = 0; i < table->nEntries; i++)
				if(table->entry[i].string) free(table->entry[i].string);
			free(table->entry);
		}
		free(table);
	}
}

/**
	Gets the pointer to the string given the value
	@param tbl Handle to the TBLINFO struct
	@param value The value to find the string in the table
	@param size The size in bytes of the value to be searched. Should be between 1-3
					If size = 0, then it'll return the value regardless of size
	@param pstr Address of a const char pointer to be updated to the string
					If this field is NULL, then only the found status is returned
	@return 1 if the string is found, 0 if the string was not found
	@example
		TBLINFO tbl;
		const char *str;
		int found;
			.
			.
		found = TBL_GetString(tbl,0x1000,0,&str); // gets the string at 1000
*/
int TBLAPICALL TBL_GetString(TBLINFO tbl,unsigned long value,int size,const char **pstr)
{
	register unsigned int h, p, max;

	h = hash(value);
	p = hinc(value);

	max = tbl->nMaxCollisionsV;

	do {
		if(!tbl->hv[h]) break;
		if(tbl->hv[h]->value == value && (tbl->hv[h]->size == size || !size)) {
			if(pstr) *pstr = tbl->hv[h]->string;
			return 1;
		}

		h += p;
		if(h >= hashsize) h %= hashsize;
	} while(max--);

	TBL_Error("No string matching `%X' was found.",value);
	return 0;
}

/**
	Gets the value specified by the string
	@param tbl Handle to the TBLINFO struct
	@param string The string to search for
	@param value Pointer to an unsigned long to update the value
					 If this field is NULL, then value is not updated
	@param size Pointer to an int to recieve the size of the value
					If this field is NULL, then size is not updated
	@return 1 if the value is found, 0 if the value was not found
	@example
		TBLINFO tbl;
		unsigned long value;
		int found;
			.
			.
		found = TBL_GetValue(tbl,"A",&value,0); // gets the value for the string "A"
*/
int TBLAPICALL TBL_GetValue(TBLINFO tbl,const char *string,unsigned long *value,int *size)
{
	register unsigned int h, p, max;

	h = hashs(string);
	p = hincs(string);

	max = tbl->nMaxCollisionsS;

	do {
		if(!tbl->hs[h]) break;
		if(strcmp(tbl->hs[h]->string,string) == 0) {
			if(value) *value = tbl->hs[h]->value;
			if(size) *size = tbl->hs[h]->size;
			return 1;
		}

		h += p;
		if(h >= hashsize) h %= hashsize;
	} while(max--);

	TBL_Error("No value matching `%s' was found.",string);
	return 0;
}
