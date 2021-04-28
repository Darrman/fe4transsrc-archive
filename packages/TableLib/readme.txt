#######################################################################
#                          TblLib v1.0                                #
#                A Table Loading Library for Compilers                #
#                          (c)2000 Jay                                #
#######################################################################

Disclaimer
----------
I am not responsible for any bugs in the source code or anything that
happens to your computer cause by the source. Use at own risk.

What is it?
-----------
Of my many years on the scene, I have never use *.tbl files once ever!
Still I do favour the old style EUC tables use back a long time ago
since rom hacking was new and I still do. But of ever person I meet and
help, they keep sending me tbl files and I have to write tbl loading
routines to convert them to a tbl format that I use. Writing TBL loader's
are annoying!!!!
Finally I put an end to this by writing a library that will facilate the
process of table loading.

What this library is good for?
------------------------------
This library is good for loading tables, converting TBL's to another
format, dumping scripts using tbl files, and translating values from
one to another. This library is not good for run time insertion, well
maybe you'll find out why.

Installation and compiling instructions
---------------------------------------
If you decide to compile the sources yourself then read on otherwise
skip to the installation instructions.

You will need a ANSI C compiler.
Makefiles are available for DJGPP, and Borland C compilers.

To compile with DJGPP, in dos prompt type:
make -f makefile.gcc

For Borland Compilers,
make -f makefile.bcc		; 16-bit version
make -f makefile.b32		; Win32 DLL version

* For Borland Compilers Win32 version, please modify the makefile variable
* BCPATH in "makefile.b32" to point to the path of your Borland
* installation.

If makefiles don't work or you got a different compiler:
Compile the sources: tbl.c hash.c string.c
Then archive them into a library.

To install the build, place the header file "tbl.h" into the include/
directory of your compiler, and the library file "libtbl.a" or "tbllib.lib"
in the lib/ directory.

Using TblLib
------------
When using TblLib, always include the header file <tbl.h>.
For DJGPP compilers, compile with the -ltbl flag. For ex.
gcc main.c -ltbl -o main.exe
For others compilers, include the "tbllib.lib" in your project or create
an import lib for the dll.

TblLib Reference
----------------

const char * TBLEXPORT TBLAPICALL TBL_GetErrorString(void);

	Returns a string reporting any errors that occured in one of the operations


void TBLEXPORT TBLAPICALL TBL_SetEntrySize(unsigned int size);

	Sets the size of the entry table. This is use by the internal hash table to
	determine how large of a hash to use.
	The entry table size should generally be 2x the maximum supported entries.
	Default: 2000
	param:
		'size' The size of the entry table


TBLINFO TBLEXPORT TBLAPICALL TBL_LoadTable(const char *filename

	Loads a table file into memory and returns the handle
	param:
		'filename' Pointer to the string that contains the filename of the table to load
	returns:
		A handle to a TBLINFO struct requried for TBL operations or NULL if an error occured
	example:
		TBLINFO tblfile;
		tblfile = TBL_LoadTable("mytable.tbl");



void TBLEXPORT TBLAPICALL TBL_CloseTable(TBLINFO table)

	Closes a table file and frees up all memory used by the table
	'param'
		table A handle to the TBLINFO to close


int TBLEXPORT TBLAPICALL TBL_GetString(TBLINFO tbl,unsigned long value,int size,const char **pstr)

	Gets the pointer to the string given the value
	param:
		'tbl' Handle to the TBLINFO struct
		'value' The value to find the string in the table
		'size' The size in bytes of the value to be searched. Should be between 1-3
			If size = 0, then it'll return the value regardless of size
		'pstr' Address of a const char pointer to be updated to the string
			If this field is NULL, then only the found status is returned
	return:
		0 if the string is found, -1 if the string was not found
	example:
		TBLINFO tbl;
		const char *str;
		int error;
			.
			.
		error = TBL_GetString(tbl,0x1000,0,&str); // gets the string at 1000

int TBLEXPORT TBLAPICALL TBL_GetValue(TBLINFO tbl,const char *string,unsigned long *value,int *size)

	Gets the value specified by the string
	param:
		'tbl' Handle to the TBLINFO struct
		'string' The string to search for
		'value' Pointer to an unsigned long to update the value
			If this field is NULL, then value is not updated
		'size' Pointer to an int to recieve the size of the value
   			If this field is NULL, then size is not updated
	return:
		0 if the value is found, -1 if the value was not found
	example:
   		TBLINFO tbl;
		unsigned long value;
		int error;
			.
			.
		error = TBL_GetValue(tbl,"A",&value,0); // gets the value for the string "A"