/*
	string.c
	Some non-ansi string functions
*/
#include <stdlib.h>
#include <string.h>

/**
	Duplicates a string
	@param s Pointer to string to duplicate
	@return Returns a pointer to a newly allocated string
	@note Users should deallocate the memory used by the string with free()
*/
char *strdup(const char *s)
{
	char *newstring;

	/* allocate memory for new string */
	newstring = (char *)malloc(strlen(s) + 1);
	if(!newstring) return NULL;

	strcpy(newstring,s);

   return newstring;
}
