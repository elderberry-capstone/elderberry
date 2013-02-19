/**
 *  @file testLoggerScreen.c
 *  @brief logs all info into a terminal session.
 *  @details Simple piece of code that passes data to be written to the active terminal session.
 *  @author Clark Wachsmuth
 *  @date February 8th, 2013
 */

#include <stdio.h>
#include <string.h>


void init_screenLogger () {
	
}

// src... name of source
// buffer... message
// len... length of data in buffer

void screenLogger_getMessage(const char *src, char *buffer, int len) {
	printf("%s", (const char *)buffer);
}


// Other private functions to do stuff.
