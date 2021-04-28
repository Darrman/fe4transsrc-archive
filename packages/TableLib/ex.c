/*
	Example for using tbl-lib
*/
#include <stdio.h>
#include <tbl.h>

int main()
{
	TBLINFO tbl;
	int found;
	unsigned long value;
	int size;
	const char *s;

	tbl = TBL_LoadTable("ex.tbl");
	if(!tbl) {
		printf("%s\n",TBL_GetErrorString());
		return 1;
	}

	// grab string at 5c, should be 'Hello!!!!'
	found = TBL_GetString(tbl,0x5c,1,&s);
	if(found) {
		printf("5C=%s\n",s);
	} else {
		printf("Did not find any string at 5C.\n");
	}

	// grab the value
	found = TBL_GetValue(tbl,"Three byte support",&value,&size);
	if(found) {
		printf("%X=Three byte support\n",value);
	} else {
		printf("Three byte support not found.\n");
	}
	TBL_CloseTable(tbl);
	return 0;
}