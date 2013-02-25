/**
 *  @file testLoggerDisk.c
 *  @brief Logs all info to a file on disk
 *  @details Has functions to initialize the disk logger, get messages to log to disk, and to "finalize" the disk logging by closing the file being written to.
 *  @author Clark Wachsmuth
 *  @date February 8th, 2013
 */


#include <stdio.h>

FILE *fp = NULL;
char * file;
char * filename = "logfile.log";

/**
 *  @brief Initializes the disk logging function
 *  @details Initializes function by copying filename into private data and opening the file. Error produced if file can't be opened.  
 *  @param filename Character pointer to name of file to be written to.
 */
// Initializes function by copying filename into private data and 
// opening file. Error produced if file can't be opened. (NOTE!) Not
// sure about passing only be reference.
int init_diskLogger (void) {
	// Need to check filename!!!

	if(!filename){
		printf("Could not open file for writing.\n");
		return -2;
	}
	file = filename;
	fp = fopen(file, "w+");
	if(!fp){
		return -1;
	}
	setbuf(fp, NULL);
	return 0;
}

/**
 *  @brief Gets message to write to file.
 *  @param src Name of source
 *  @param buffer Message to be written
 *  @param len Length of data in buffer
 */
// Writes data to file.
void diskLogger_getMessage(const char *src, char *buffer, int len) {
	//fprintf(fp, "%s: %s\n", src, buffer);
	fwrite(src, 1, sizeof(src)-1, fp);
	fwrite(": ", 1, 2, fp);
	fwrite(buffer, 1, sizeof(buffer)-1, fp);
	fwrite("\n", 1, 1, fp);

} 

/**
 *  @brief Closes the file stream.
 */
// Closes file stream.
int finalize_disklogger(){
	fclose(fp);
	return 0;
}
