/*
	Tbl.h
	Header file for loading tables
*/

#ifndef __TBL_H
#define __TBL_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __DJGPP__
#define STDCALL __attribute__((stdcall))
#else
#define STDCALL __stdcall
#endif

#ifdef __WIN32__
#define TBLAPICALL STDCALL
#define TBLEXPORT __export
#ifdef __DJGPP__
#define EXPORTNAME(name) asm(name)
#else
#define EXPORTNAME(name)
#endif
#else
#define EXPORTNAME(name)
#define TBLAPICALL
#define TBLEXPORT
#endif


#ifndef true
#define true 1
#endif
#ifndef false
#define false 0
#endif

struct __tag_TBLINFO;
typedef struct __tag_TBLINFO * TBLINFO;

/* Functions */

/**
	Returns a string reporting any errors that occured in on of the operations
 */
const char * TBLEXPORT TBLAPICALL TBL_GetErrorString(void) EXPORTNAME("TBL_GetErrorString");

/**
	Sets the size of the entry table.
	The entry table size should generally be 2x the maximum supported entries.
	Default: 2000
	@param size The size of the entry table
*/
void TBLEXPORT TBLAPICALL TBL_SetEntrySize(unsigned int size) EXPORTNAME("TBL_SetEntrySize");

/**
	Loads a table file into memory and returns the handle
	@param filename Pointer to the string that contains the filename of the table to load
	@return A handle to a TBLINFO struct requried for TBL operations or NULL if an error occured
	@example
		TBLINFO tblfile;
		tblfile = TBL_LoadTable("mytable.tbl");
*/
TBLINFO TBLEXPORT TBLAPICALL TBL_LoadTable(const char *filename) EXPORTNAME("TBL_LoadTable");

/**
	Closes a table file and frees up all memory used by the table
	@param table A handle to the TBLINFO to close
*/
void TBLEXPORT TBLAPICALL TBL_CloseTable(TBLINFO table) EXPORTNAME("TBL_CloseTable");

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
int TBLEXPORT TBLAPICALL TBL_GetString(TBLINFO tbl,unsigned long value,int size,const char **pstr) EXPORTNAME("TBL_GetString");

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
		found = TBL_GetValue(tbl,"A",&value,NULL); // gets the value for the string "A"
*/
int TBLEXPORT TBLAPICALL TBL_GetValue(TBLINFO tbl,const char *string,unsigned long *value,int *size)  EXPORTNAME("TBL_GetValue");

#ifdef __cplusplus
}
#endif

#endif
