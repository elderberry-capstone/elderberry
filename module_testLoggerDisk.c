/**
 *  @file testLoggerDisk.c
 *  @brief Logs all info to a file on disk
 *  @details Has functions to initialize the disk logger, get messages to log to disk, and to "finalize" the disk logging by closing the file being written to.
 *  @author Clark Wachsmuth
 *  @date February 8th, 2013
 */


#include <stdio.h>
#include <errno.h>
#include <string.h>

static FILE *fp = NULL;
static const char *filename = "logfile.log";

/**
 *  @brief Initializes the disk logging function
 *  @details Initializes function by copying filename into private data and opening the file. Error produced if file can't be opened.  
 *  @param filename Character pointer to name of file to be written to.
 */
int init_diskLogger (void) {

	fp = fopen(filename, "w+");
	if(!fp){
		fprintf (stderr, "disk logger: could not open file %s for writing: %s\n", filename, strerror(errno));
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
void finalize_diskLogger(){
	if (fp != NULL) {
		int rc = fclose(fp);
		if (rc == -1) {
			fprintf (stderr, "disk logger: error closing file %s: %s\n", filename, strerror(errno));
		}
		fp = NULL;
	}
}
