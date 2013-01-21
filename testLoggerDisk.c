
/***
*  Test Module to Log data to Disk
*
*/

#include <stdio.h>

FILE *fp = NULL;
char * file;


// Initializes function by copying filename into private data and 
// opening file. Error produced if file can't be opened. (NOTE!) Not
// sure about passing only be reference.
int init_diskLogger (char *filename) {
	// Need to check filename!!!
	if(!filename){
		return -2;
	}
	file = filename;
	fp = fopen(file, "w");
	if(!fp){
		return -1;
	}
	return 0;
}

// Writes data to file.
void diskLogger_getMessage(const char *src, char *buffer, int len) {
	fprintf(fp, "%s: %s\n", src, buffer);

} 

// Closes file stream.
int finalize_disklogger(){
	fclose(fp);
	return 0;
}

// Other private functions to do stuff.
